#include <xs1.h>
#include <print.h>

#include "coe.h"
#include "canod/canod.h"

static unsigned char reply[COE_MAX_DATA_SIZE];
static int replyPending;


struct _sdo_info_header {
	unsigned char opcode;
	unsigned char incomplete;
	unsigned fragmentsleft; /* number of fragments which will follow - Q: in this or the next packet? */
};

static void build_reply(struct _sdo_info_header sdo_header, unsigned char data[], unsigned datasize)
{
	int i,j;

	reply[0] = (sdo_header.opcode&0x7f) | ((sdo_header.incomplete&0x01)<<8);
	reply[1] = 0;
	reply[2] = sdo_header.fragmentsleft&0xff;
	reply[3] = (sdo_header.fragmentsleft>>8)&0xff;

	for (i=0,j=4; i<datasize; i++) {
		reply[j] = data[i];
	}

	replyPending = 1;
}

#if 0
static void parse_packet(unsigned char buffer[], ...)
{
}
#endif

static inline void parse_coe_header(unsigned char buffer[], struct _coe_header head)
{
	unsigned tmp = buffer[1];
	tmp = (tmp<<8) | buffer[0];

	head.sdonumber = tmp&0x09;
	head.sdoservice = (tmp>>12)&0x04;
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
		for (i=0, k=0; i<size; i++, k++) {
			data[k] = olists[i]&0xff;
			k++;
			data[k] = (olists[i]>>8)&0xff;
		}
	}

	build_reply(sdo_header, data, k);

	return 0;
}

static int sdoinfo_request(unsigned char buffer[], unsigned size)
{
	struct _sdo_info_header infoheader;
	unsigned char data[COE_MAX_DATA_SIZE-6]; /* quark */
	unsigned datasize = COE_MAX_DATA_SIZE-6;
	unsigned abortcode = 0;
	unsigned servicedata = 0;

	unsigned index, subindex, valueinfo;
	struct _sdoinfo_entry_description desc;

	infoheader.opcode = buffer[2]&0x07;
	infoheader.incomplete = (buffer[2]>>7)&0x01;
	infoheader.fragmentsleft = buffer[4] | ((unsigned)buffer[5]>>8);

	if (size>(COE_MAX_DATA_SIZE-6)) {
		printstrln("[sdoinfo_request()] error size is much larger than expected\n");
		return 0;
	}

	switch (infoheader.opcode) {
	case COE_SDOI_GET_ODLIST_REQ: /* answer with COE_SDOI_GET_ODLIST_RSP */
		/* DEBUG output: */
		servicedata = (unsigned)buffer[6]&0xff | ((unsigned)buffer[7])>>8&0xff;
		printstr("[DEBUG SDO INFO] get OD list: 0x"); printhexln(servicedata);
		getODListRequest(servicedata);
		break;

	case COE_SDOI_OBJDICT_REQ: /* answer with COE_SDOI_OBJDICT_RSP */
		servicedata = (unsigned)buffer[6]&0xff | ((unsigned)buffer[7])>>8&0xff;
		/* here servicedata  holds the index of the requested object description */
		break;

	case COE_SDOI_ENTRY_DESCRIPTION_REQ: /* answer with COE_SDOI_ENTRY_DESCRIPTION_RSP */
		index = (unsigned)buffer[6]&0xff | ((unsigned)buffer[7])>>8&0xff;
		subindex = buffer[8];
		valueinfo = buffer[9]; /* bitmask which elements should be in the response - bit 1,2 and 3 = 0 (reserved) */
		canod_get_entry_description(index, subindex, valueinfo, desc);
		break;

	case COE_SDOI_INFO_ERR_REQ: /* FIXME check abort code and take action */
		abortcode = (unsigned)buffer[6]&0xff |
			((unsigned)buffer[7]>>8)&0xff |
			((unsigned)buffer[8]>>16)&0xff |
			((unsigned)buffer[9]>>24)&0xff;
		printstr("[SDO INFO] Error request receiveied 0x");
		printhexln(abortcode);
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

	return reply_pending;
}

