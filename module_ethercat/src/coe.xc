#include <xs1.h>
#include <print.h>

#include "coe.h"
#include "canod/canod.h"

static unsigned char reply[COE_MAX_DATA_SIZE];
static int replyPending;
static int replyDataSize;


struct _sdo_info_header {
	unsigned char opcode;
	unsigned char incomplete;
	unsigned fragmentsleft; /* number of fragments which will follow - Q: in this or the next packet? */
};

struct _sdo_request_header {
	unsigned char command;
	unsigned char complete;
	unsigned index;
	unsigned subindex;
};

struct _sdo_response_header {
	unsigned char command;
	unsigned char complete;
	unsigned char dataSetSize;
	unsigned char transfereType; /* normal (0x00) or expedited (0x01) */
	unsigned char sizeIndicator; /* 0x01 */
};

static void build_sdoinfo_reply(struct _sdo_info_header sdo_header, unsigned char data[], unsigned datasize)
{
	int i,j;
//printstr("[trace build_sdoinfo_reply] building reply\n");
	reply[0] = 0x00;
	reply[1] = 0x80;

	reply[2] = (sdo_header.opcode&0x7f) | ((sdo_header.incomplete&0x01)<<8);
	reply[3] = 0;
	reply[4] = sdo_header.fragmentsleft&0xff;
	reply[5] = (sdo_header.fragmentsleft>>8)&0xff;

	for (i=0,j=6; i<datasize; i++, j++) {
		reply[j] = data[i];
	}

	for (; j<COE_MAX_DATA_SIZE; j++) /* padding */
		reply[j] = 0;

	replyPending = 1;
	replyDataSize = datasize+6;
}

static void build_sdo_reply(struct _sdo_response_header header, unsigned char data[], unsigned datasize)
{
	unsigned datas = 0;
	//printstr("[trace build_sdo_reply] building reply\n");

	reply[datas++] = 0;
	reply[datas++] = 0x30; /* SDO Response - handcrafted */

	reply[datas++] = (header.sizeIndicator&0x1) |
		   ((header.transfereType&0x1)<<1) |
		   ((header.dataSetSize&0x3)<<2) |
		   ((header.complete&0x1)<<4) |
		   ((header.command&0x7)<<5);

	for (int i=0; i<datasize; i++) {
		reply[datas++] = data[i];
	}

	for (int i=datas; i<COE_MAX_DATA_SIZE-1; i++)
		reply[i] = 0;

	replyPending = 1;
	replyDataSize = datas;
}

#if 0
static void parse_packet(unsigned char buffer[], ...)
{
}
#endif

static inline void parse_coe_header(unsigned char buffer[], struct _coe_header &head)
{
	unsigned tmp = buffer[1];
	tmp = (tmp<<8) | buffer[0];

	head.sdonumber = tmp&0x1ff;
	head.sdoservice = (tmp>>12)&0x0f;
}


/* sdo information request handler */
static int getODListRequest(unsigned listtype)
{
	int i,k;
	unsigned lists[5];
	unsigned olists[100]; /* check length */
	unsigned size;
	unsigned char data[COE_MAX_DATA_SIZE];
	struct _sdo_info_header sdo_header;

	if (replyPending) {
		return -1; /* previous reply is pending */
	}

	sdo_header.opcode = COE_SDOI_GET_ODLIST_RSP;

	if (listtype == 0) { /* list of length is replied */
		canod_get_all_list_length(lists);

		/* FIXME build reply */
		sdo_header.incomplete = 0;
		sdo_header.fragmentsleft = 0;

		for (i=0, k=0; i<5; i++, k++) {
			data[k] = lists[i]&0xff;
			k++;
			data[k] = (lists[i]>>8)&0xff;
		}

	} else {
		size = canod_get_list(olists, 100 /* == maxsize*/, listtype);

		if (size*2>COE_MAX_DATA_SIZE-3) {
			printstr("more segments will follow\n");
		}

		/* FIXME build reply */
		sdo_header.incomplete = 0;
		sdo_header.fragmentsleft = 0;

		k=0;
		data[k++] = listtype&0xff;
		data[k++] = (listtype>>8)&0xff;
		for (i=0; i<size; i++) {
			data[k++] = olists[i]&0xff;
			data[k++] = (olists[i]>>8)&0xff;
		}
	}

	build_sdoinfo_reply(sdo_header, data, k);

	return 0;
}

static int sdo_request(unsigned char buffer[], unsigned size)
{
	unsigned i, k;
	unsigned index;
	unsigned subindex;
	unsigned type;
	unsigned value;
	struct _sdo_response_header header;
	unsigned char tmp[COE_MAX_DATA_SIZE];
	unsigned opcode = (buffer[2]>>5)&0x03;
	unsigned completeAccess = (buffer[2]>>4)&0x01; /* completeAccess only in SDO Upload Req/Rsp */
	unsigned maxSubindex = 0;
	unsigned dataSize = 0;

	switch (opcode) {
	case COE_CMD_UPLOAD_REQ:
		index = (buffer[3]&0xff) | (((unsigned)buffer[4]<<8)&0xff00);
		subindex = buffer[5];

#if 0
		printstr("[DEBUG] index: "); printhexln(index);
		printstr("[DEBUG] subindex: "); printhexln(subindex);
		printstr("[DEBUG] complete access: "); printhexln(completeAccess);
#endif
		if (completeAccess == 1) {
			if (buffer[5] != 0 || buffer[5] != 1) {
				/* error complete access requested, but subindex is not zero or one*/
				printstr("[ERROR] Complete Access with wrong subindex field\n");
				return -1;
			}
		}

		header.command = COE_CMD_UPLOAD_RSP;
		header.complete = completeAccess;
		header.dataSetSize = 0x00; /* fixed to 0 */
		header.transfereType = 0x00; /* normal transfer */
		header.sizeIndicator = 0x01;

		tmp[0/*dataSize++*/] = index&0xff;
		tmp[1/*dataSize++*/] = (index>>8)&0xff;
		tmp[2/*dataSize++*/] = subindex&0xff;
		tmp[3/*dataSize++*/] = 0x00; //type&0xff; // index 3
		tmp[4/*dataSize++*/] = 0x00; //(type>>8)&0xff;
		tmp[5/*dataSize++*/] = 0x00; //(type>>16)&0xff;
		tmp[6/*dataSize++*/] = 0x00; //(type>>24)&0xff;

		dataSize = 7;

		if (completeAccess==1) {
			canod_get_entry(index, 0, value, type);
			maxSubindex = value;
			if (subindex==0x00) {
				tmp[dataSize++] = value&0xff; /* subindex 0 is alway UNSIGNED8 */
			}

			for (i=1; i<maxSubindex; i++) {
				canod_get_entry(index, i, value, type);

//				printstr("[DEBUG complete object values: "); printintln(i); printstr(": ");
				for (k=0; k<(type/8); k++) {
					tmp[dataSize++] = (value>>(k*8))&0xff;
//					printhexln(tmp[dataSize-1]);
				}
			}

		} else {
			if (canod_get_entry(index, subindex, value, type)) { // type is bitlength
				printstr("error, entry not found\n");
				return 1;
			}

			//printstr("[DEBUG] single value: ");
			for (k=0; k<(type/8); k++) {
				tmp[dataSize++] = (value>>(k*8))&0xff;
				//printhexln(tmp[dataSize-1]);
			}
		}

		type = dataSize-7; /* recycling of variable type */
		tmp[3] = type&0xff;
		tmp[4] = (type>>8)&0xff;
		tmp[5] = (type>>16)&0xff;
		tmp[6] = (type>>24)&0xff;
#if 0
		printstr("[DEBUG] datasize to send: ");
		printhex(tmp[6]);
		printhex(tmp[5]);
		printhex(tmp[4]);
		printhexln(tmp[3]);
#endif
		build_sdo_reply(header, tmp, dataSize);
		break;

#if 0
	case COE_CMD_UPLOAD_RSP:
		break;
#endif

	case COE_CMD_UPLOAD_SEG_REQ:
		return -1; /* currently unsupported */

	case COE_CMD_UPLOAD_SEG_RSP:
		return -1; /* currently unsupported */

	case COE_CMD_ABORT_REQ:
		/* FIXME handle abort request appropriately */
		break;

	default:
		return -1; /* unknown command specifier */
	}

	return 0;
}

static int sdoinfo_request(unsigned char buffer[], unsigned size)
{
	struct _sdo_info_header infoheader;
	struct _sdo_info_header response;
	unsigned char data[COE_MAX_DATA_SIZE-6]; /* quark */
	unsigned datasize = COE_MAX_DATA_SIZE-6;
	unsigned abortcode = 0;
	unsigned servicedata = 0;
	int i;

	unsigned index, subindex, valueinfo;
	struct _sdoinfo_entry_description desc;
	struct _sdoinfo_object_description objdesc;

	infoheader.opcode = buffer[2]&0x07;
	infoheader.incomplete = (buffer[2]>>7)&0x01;
	infoheader.fragmentsleft = buffer[4] | ((unsigned)buffer[5]>>8);

	if (size>(COE_MAX_DATA_SIZE-6)) {
		printstrln("[sdoinfo_request()] error size is much larger than expected\n");
		return 0;
	}

	switch (infoheader.opcode) {
	case COE_SDOI_GET_ODLIST_REQ: /* answer with COE_SDOI_GET_ODLIST_RSP */
		servicedata = (((unsigned)buffer[6])&0xff) | ((((unsigned)buffer[7])>>8)&0xff);
		/* DEBUG output: */
		//printstr("[DEBUG SDO INFO] get OD list: 0x"); printhexln(servicedata);
		getODListRequest(servicedata);
		break;

	case COE_SDOI_OBJDICT_REQ: /* answer with COE_SDOI_OBJDICT_RSP */
		servicedata = ((unsigned)buffer[6]&0xff) | (((unsigned)buffer[7]<<8)&0xff00);
		/* here servicedata holds the index of the requested object description */
		canod_get_object_description(objdesc, servicedata);

		data[0] = objdesc.index&0xff;
		data[1] = (objdesc.index>>8)&0xff;
		data[2] = objdesc.dataType&0xff;
		data[3]	= (objdesc.dataType>>8)&0xff;
		data[4]	= objdesc.maxSubindex;
		data[5]	= objdesc.objectCode;
		for (i=0; objdesc.name[i] != '\0'; i++) {
			data[i+6]	= objdesc.name[i];
		}

		response.fragmentsleft = 0;
		response.incomplete = 0;
		response.opcode = COE_SDOI_OBJDICT_RSP;

		build_sdoinfo_reply(response, data, 6+i);
		break;

	case COE_SDOI_ENTRY_DESCRIPTION_REQ: /* answer with COE_SDOI_ENTRY_DESCRIPTION_RSP */
		index = ((unsigned)buffer[6]&0xff) | ((((unsigned)buffer[7])<<8)&0xff00);
		subindex = buffer[8];
		valueinfo = buffer[9]; /* bitmask which elements should be in the response - bit 1,2 and 3 = 0 (reserved) */

		canod_get_entry_description(index, subindex, valueinfo, desc);
		response.fragmentsleft = 0;
		response.incomplete = 0;
		response.opcode = COE_SDOI_ENTRY_DESCRIPTION_RSP;

		data[0] = index&0xff;
		data[1]	= (index>>8)&0xff;
		data[2] = subindex&0xff;
		data[3] = 0x18; /* valueinfo&0xff;*/
		data[4] = desc.dataType&0xff;
		data[5] = (desc.dataType>>8)&0xff;
		data[6] = desc.bitLength&0xff;
		data[7]	= (desc.bitLength>>8)&0xff;
		data[8]	= desc.objectAccess&0xff;
		data[9] = (desc.objectAccess>>8)&0xff;
		//data[10] = desc.value&0xff;
		//data[11] = (desc.value>>8)&0xff;
		datasize = 10;

#if 1
		/* repeat data type (know as unit type) [DWORD] */
		data[datasize++] = desc.dataType&0xff;
		data[datasize++] = (desc.dataType>>8)&0xff;
		//data[datasize++] = 0x00;
		//data[datasize++] = 0x00;

		/* FIXME refactor for more generality */
		switch (desc.bitLength/8) {
		case 1:
			data[datasize++] = desc.value&0xff;
#if 0
			/* min value */
			data[datasize++] = 0x00;
			/* max value */
			data[datasize++] = 0xff;
#endif
			break;

		case 2:
			data[datasize++] = desc.value&0xff;
			data[datasize++] = (desc.value>>8)&0xff;
			//data[datasize] = 0x00;
			//data[datasize] = 0x00;
#if 0
			/* min value */
			data[datasize++] = 0x00;
			data[datasize++] = 0x00;
			/* max value */
			data[datasize++] = 0xff;
			data[datasize++] = 0xff;
#endif
			break;

		case 4:
			data[datasize++] = desc.value&0xff;
			data[datasize++] = (desc.value>>8)&0xff;
			data[datasize++] = (desc.value>>16)&0xff;
			data[datasize++] = (desc.value>>24)&0xff;
#if 0
			/* min value */
			data[datasize++] = 0x00;
			data[datasize++] = 0x00;
			data[datasize++] = 0x00;
			data[datasize++] = 0x00;
			/* max value */
			data[datasize++] = 0xff;
			data[datasize++] = 0xff;
			data[datasize++] = 0xff;
			data[datasize++] = 0xff;
#endif
			break;
		}

		/* this should only be included if enough space is available in the report - FIXME include in official packet??? */
		if ((datasize+6)<COE_MAX_DATA_SIZE) {
			for (i=0; desc.name[i] != '\0'; i++) {
				data[datasize+i] = desc.name[i];
			}
		}
#endif
		build_sdoinfo_reply(response, data, datasize);

//		printstr("[trace] request index: 0x"); printhex(index);
//		printstr(" subindex 0x"); printhexln(subindex);
//		printstr("[trace] length: "); printintln(replyDataSize);
//		printstr("[trace] value 0x"); printhexln(desc.value);
		break;

	case COE_SDOI_INFO_ERR_REQ: /* FIXME check abort code and take action */
		abortcode = ((unsigned)buffer[6]&0xff) |
			(((unsigned)buffer[7]<<8)&0xff) |
			(((unsigned)buffer[8]<<16)&0xff) |
			(((unsigned)buffer[9]<<24)&0xff);
		printstr("[SDO INFO] Error request receiveied 0x");
		printhexln(abortcode);
		/* FIXME do something appropriate  */
		break;

	default:
		printstr("[SDO INFO] Error unknown opcode 0x");
		printhexln(infoheader.opcode);
		return -1;
	}
#if 0
	for (i=0; i<size; i++) {
		data[i] = buffer[i+6];
	}
#endif

	return 0;
}

/* coe api */

int coe_init(void)
{
	int i;

	for (i=0; i<COE_MAX_DATA_SIZE; i++)
		reply[i] = 0;

	replyPending = 0;
	replyDataSize = 0;

	return 0;
}

int coe_rx_handler(chanend coe, char buffer[], unsigned size)
{
	struct _coe_header coe_header;
	unsigned canmsgsize = size - COE_MAX_HEADER_SIZE; /* FIXME unused */
	unsigned reply_pending = 0;

	parse_coe_header(buffer, /*size,*/ coe_header);

	switch (coe_header.sdoservice) {
	case COE_SERVICE_EMERGENCY:
		/* emergency request */
		break;

	case COE_SERVICE_SDO_REQ:
		/* download expedited, download normal, SDO segment, upload expedited, upload normal, upload SDO segment, abort SDO transfer */
		sdo_request(buffer, size);
		break;

	case COE_SERVICE_SDO_RSP: /* only needed if SDO requests are sent */
		break;

	case COE_SERVICE_SDO_INFO:
		/* SDO information service, get OD list, get object dictionary, get entry description SDO information error */
		sdoinfo_request(buffer, size); /* can generate reply */
		break;

	case COE_SERVICE_TXPDO:
		break;

	case COE_SERVICE_RXPDO:
		break;

	case COE_SERVICE_TXPDO_REMOTE:
		break;

	case COE_SERVICE_RXPDO_REMOTE:
		break;

	default:
		break;
	}

	if (replyPending)
		return 1;

	return 0/*reply_pending*/;
}

int coe_get_reply(char buffer[])
{
	int i, size;

	if (replyPending == 0)
		return 0;

	for (i=0; i<COE_MAX_DATA_SIZE; i++) {
		buffer[i] = reply[i];
	}

	size = replyDataSize;

	reply[0] = 0;
	replyPending = 0;
	replyDataSize = 0;

	return size;
}

int coe_reply_ready()
{
	return replyPending;
}
