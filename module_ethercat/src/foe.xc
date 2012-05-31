/* foe.xc
 *
 * 2012-03-29 Frank Jeschke <jeschke@fjes.de>
 */

#include <xs1.h>
#include <print.h>

#include "foe_chan.h"
#include "foefs.h"
#include "foe.h"

static int state;
static int file;
static foemsg_t reply;
static unsigned replySize;
static unsigned reply_raw[FOE_MAX_MSGSIZE];
static int current_fp; /* current file pointer */

static foemsg_t parse(uint16_t msg[], unsigned size)
{
	foemsg_t m;
	uint32_t tmp=0;
	int i=0, k=0;

	m.opcode = msg[k++]&0xff;
	tmp = (uint32_t)msg[k+1];
	m.a.packetnumber = ((tmp&0xffff)<<16)|(msg[k]&0xff);
	k+=2;

	for (i=0; i<FOE_DATA_SIZE && k<size; i+=2, k++) {
		m.b.data[i] = (unsigned char)(msg[k]&0xff);
		m.b.data[i+1] = (unsigned char)((msg[k]>>8)&0xff);
	}

	/* pad the data buffer */
	for (;i<FOE_DATA_SIZE; i++) {
		m.b.data[i] = 0x00;
	}

	return m;
}

static unsigned int make_reply(unsigned type, uint32_t a, char ?data[], unsigned data_size)
{
	int16_t tmp;
	unsigned i, k;
	unsigned resize = FOE_HEADER_SIZE/2;

	reply.opcode = type&0xff;
	reply.a.packetnumber = a; /* here it's irrelevant which field of the union is used. */

	if (data_size <= 0) {
		return resize;
	}

	/* FIXME another option is to transfer only FOE_DATA_SIZE */
	if (data_size > FOE_DATA_SIZE) {
		return -1;
	}

	if (!isnull(data)) {
		for (k=0; k<data_size; k++) {
			reply.b.data[k] = data[k];
		}
	} else {
		for (i=0; i<FOE_DATA_SIZE; i++) {
			reply.b.data[i] = 0;
		}
	}

	resize += data_size;

	return resize;
}

/* draft for handle idle_state */
static int handle_idle_state()
{
	int nextState = FOE_STATE_IDLE;
	return nextState;
}

/* draft for handle read_state */
static int handle_read_state()
{
	int nextState = FOE_STATE_IDLE;
	return nextState;
}

/* draft for handle write_state */
static int handle_write_state()
{
	int nextState = FOE_STATE_IDLE;
	return nextState;
}

/* public functions */

int foe_init(void)
{
	state = FOE_STATE_IDLE;
	current_fp = 0;
	replySize = 0;

	return 0;
}

/* unused */
int foe_close(void)
{
	return foe_init();
}

/* FIXME add handling of FOE_BUSY packages */
int foe_parse_packet(uint16_t msg[], unsigned size)
{
	int ret = -1;
	unsigned char data[FOE_MAX_MSGSIZE];
	int dataSize = 0;
	static uint32_t packetNumber = 0;
	int bytecount = 0;

	foemsg_t rec = parse(msg, size);

	switch (state) {
	case FOE_STATE_IDLE:
		/* expected write.req and read.req */
		switch (rec.opcode) {
		case FOE_READ:
			if (current_fp <= 0) {
				state = FOE_STATE_READ;
				current_fp = foefs_open(rec.b.filename, MODE_RO);
				if (current_fp <= 0) {
					state = FOE_STATE_IDLE;
					replySize = make_reply(FOE_ERROR, FOE_ERR_NOTFOUND, "File not found", 14);
				} else {
					/* prepare first data package */
					dataSize = foefs_read(current_fp, 116/*FOE_DATA_SIZE*/, data);

					if (dataSize < 0) { /* read returns error */
						state = FOE_STATE_IDLE;
						replySize = make_reply(FOE_ERROR, -1*dataSize, "File not found", 14);
					} else {
						packetNumber = 1;
						replySize = make_reply(FOE_DATA, packetNumber, data, dataSize);
						packetNumber++;
						if (/*data*/replySize < 116/*FOE_DATA_SIZE*/) { /* this is infact the 'mailbox full' size. */
							state = FOE_STATE_IDLE;
							packetNumber = 0;
						}
					}
				}
				ret = 1;
			} else {
				state = FOE_STATE_IDLE;
				replySize = make_reply(FOE_ERROR, FOE_ERR_ILLEGAL, null, 0);
				ret = 1;
			}
			break;

		case FOE_WRITE:
			if (current_fp <= 0 && foefs_free() > 0) {
				current_fp = foefs_open(rec.b.filename, MODE_RW);
				state = FOE_STATE_WRITE;
				replySize = make_reply(FOE_ACK, 0, null, 0);
			} else {
				state = FOE_STATE_IDLE;
				replySize = make_reply(FOE_ERROR, FOE_ERR_DISKFULL, null, 0);
			}
			ret = 1;
			break;

		default:
			state = FOE_STATE_IDLE;
			replySize = make_reply(FOE_ERROR, FOE_ERR_UNDEF, "Illegal Request", 15);
			ret = 1;
			break;
		}
		break;

	case FOE_STATE_READ:
		/* expected ack.req, error.req */
		switch (rec.opcode) {
		case FOE_ACK:
			/* prepare next pacakge or end */
			dataSize = foefs_read(current_fp, FOE_DATA_SIZE, data);
			replySize = make_reply(FOE_DATA, packetNumber, data, dataSize);
			packetNumber++;
			if (dataSize < FOE_DATA_SIZE) {
				state = FOE_STATE_IDLE;
				packetNumber = 0;
			}
			ret = 1;
			break;

		case FOE_ERROR:
			/* abort transmission */
			state = FOE_STATE_IDLE;
			current_fp = foefs_close(current_fp);
			replySize = make_reply(FOE_UNUSED, 0, null, 0); /* clear reply package */ /* FIXME shouldn't a error message constructed? */
			ret = 0;
			printstr("Errornous reply while in FOE_STATE_READ\n");
			break;

		default:
			state = FOE_STATE_IDLE;
			current_fp = foefs_close(current_fp);
			replySize = make_reply(FOE_ERROR, FOE_ERR_UNDEF, "Missing Reply", 13);
			ret = 1;
			break;
		}
		break;

	case FOE_STATE_WRITE:
		/* expected data.req, error.req */
		switch (rec.opcode) {
		case FOE_DATA: /* FIXME if !"mailbox full" then the last package is received */
			/* handle data */
			bytecount = foefs_write(current_fp, (size*2)-FOE_HEADER_SIZE, rec.b.data); /* FIXME add error check */
			if (bytecount < 0) {
				state = FOE_STATE_IDLE;
				replySize = make_reply(FOE_ERROR, FOE_ERR_UNDEF, "Missing Reply", 13);
				current_fp = foefs_close(current_fp);
				ret = 1;
			} else {

				/* The last package is recognized by not fully filled data field. */
				/* FIXME sadly the ACK response isn't recognized by SOEM at the moment */
				if (bytecount < FOE_DATA_SIZE) { /* FIXME here lies the rabbit !!! */
					state = FOE_STATE_IDLE;
					current_fp = foefs_close(current_fp);
					ret = 0; /* FIXME the last packet ACK isn't recognized by SOEM */
				} else {
					replySize = make_reply(FOE_ACK, rec.a.packetnumber, null, 0);
					ret = 1;
				}
			}
			break;

		case FOE_ERROR:
			/* abort transmission */
			state = FOE_STATE_IDLE;
			replySize = make_reply(FOE_UNUSED, 0, null, 0); /* clear reply package */
			current_fp = foefs_close(current_fp);
			ret = 1;
			break;

		default:
			state = FOE_STATE_IDLE;
			replySize = make_reply(FOE_ERROR, FOE_ERR_UNDEF, "Missing Reply", 13);
			current_fp = foefs_close(current_fp);
			ret = 1;
			break;
		}
		break;

	/* FIXME check this state machine and bring it to work */
	case FOE_STATE_REQUEST:
		switch (rec.opcode) {
		case FOE_DATA:
			foefs_write(file, FOE_DATA_SIZE, rec.b.data); /* FIXME add error check */
			break;

		case FOE_ERROR:
			state = FOE_STATE_IDLE;
			replySize = make_reply(FOE_UNUSED, 0, null, 0); /* clear reply package */
			current_fp = foefs_close(current_fp);
			ret = 1;
			break;

		default:
			state = FOE_STATE_IDLE;
			current_fp = foefs_close(file);
			break;
		}
		break;

	/* FIXME check this state machine and bring it to work */
	case FOE_STATE_COMMIT:
		switch (rec.opcode) {
		case FOE_ACK:
			foefs_read(file, FOE_DATA_SIZE, rec.b.data); /* FIXME add error check */
			//replySize = make_reply(FOE_DATA, packetNumber, data, dataSize);
			break;

		case FOE_ERROR:
			state = FOE_STATE_IDLE;
			replySize = make_reply(FOE_UNUSED, 0, null, 0); /* clear reply package */
			current_fp = foefs_close(current_fp);
			ret = 1;
			break;

		default:
			state = FOE_STATE_IDLE;
			current_fp = foefs_close(file);
			break;
		}
		break;

	}

	return ret;
}

unsigned foe_get_reply(uint16_t data[])
{
	unsigned k = 0;
	unsigned i;
	uint16_t tmp;

	data[k++] = 0x0000 | (reply.opcode&0xff);
	data[k++] = (reply.a.packetnumber & 0xffff);
	data[k++] = (reply.a.packetnumber>>16)&0xffff;

	for (i=0; (i<FOE_DATA_SIZE) && (i<(replySize-3)); i+=2) {
		tmp = reply.b.data[i+1];
		data[k++] = (reply.b.data[i]&0xff) | ((tmp<<8)&0xff00);
	}

	return k;
}

int foe_request(uint16_t data[])
{
	unsigned pos = 0;
	unsigned i;
	uint16_t request = data[pos++];

	if (state != FOE_STATE_IDLE) {
		return 1;
	}

	for (i=0; i<FOE_DATA_SIZE; i++, pos++) {
		reply.b.filename[i] = data[pos];
	}

	reply.a.password = 0x0000;

	/* FIXME put correct state for statemachine */
	switch (request) {
	case REQUEST_FILE:
		reply.opcode = FOE_READ;
		state = FOE_STATE_REQUEST;
		file = foefs_open(reply.b.filename, MODE_RW);
		break;
	case COMMIT_FILE:
		reply.opcode = FOE_WRITE;
		state = FOE_STATE_COMMIT;
		file = foefs_open(reply.b.filename, MODE_RO);
		break;
	default:
		printstr("Error invalid request\n");
		break;
	}

	return 0;
}

/* foe channel communication to application */

/* This is a simple adaption of the default file operations above in channel communications */
int foe_app_request(int command, chanend comm)
{
	int fh=0;
	int i=0;
	int otmp;
	int size;
	char filename[MAX_FNAME];
	union {
		int inbuffer[BLKSZ/2]; /* FIXME try to reduce amount of stored memory */
		char inbufc[BLKSZ];
	} inp;

	switch (command) {
	/* a file name is terminated with '\0' */
	case FOE_FILE_OPEN:
		while (otmp != '\0') {
			comm :> inp.inbuffer[i];
			i++;
		}
		inp.inbuffer[i] = '\0';
		for (i=0; i<MAX_FNAME && inp.inbuffer[i]!='\0'; i++) {
			filename[i] = (char)inp.inbuffer[i];
		}
		filename[i] = '\0';

		fh = foefs_open(filename, MODE_RW); /* there is currently no distinction between read-only and read-write mode */
		if (fh>0) {
			comm <: FOE_FILE_ACK;
		} else {
			comm <: FOE_FILE_ERROR;
		}
		break;

	case FOE_FILE_READ:
		comm :> otmp;
		size = foefs_read(fh, otmp, inp.inbufc);

		comm <: FOE_FILE_DATA;
		for (i=0; i<size; i++) {
			comm <: inp.inbufc[i];
		}

		break;

	case FOE_FILE_WRITE: /* FIXME writing to a file is currently unsuported */
		comm :> otmp;
		i=0;

		while (i<otmp) {
			comm :> otmp;
			i++;
		}

		comm <: FOE_FILE_ERROR;
		break;

	case FOE_FILE_CLOSE:
		foefs_close(fh);
		comm <: FOE_FILE_ACK;
		break;

	case FOE_FILE_SEEK:
		comm :> otmp;
		foefs_seek(fh, otmp, SEEK_SET);
		comm <: FOE_FILE_ACK;
		break;

	case FOE_FILE_FREE: /* removes file and resets filesystem */
		foefs_format();
		comm <: FOE_FILE_ACK;
		break;
	}

	return 0;
}

