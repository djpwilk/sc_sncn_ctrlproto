/* foe.xc
 *
 * 2012-03-29 Frank Jeschke <jeschke@fjes.de>
 */

#include <xs1.h>
#include <print.h>

#include "foefs.h"
#include "foe.h"

#define FOE_HEADER_SIZE     6
#define FOE_MAX_MSGSIZE     256
#define FOE_DATA_SIZE       (FOE_MAX_MSGSIZE-FOE_HEADER_SIZE)

static int state;
static foemsg_t reply;
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
	k++;

	for (i=0; i<(FOE_DATA_SIZE/2) && i<size; i+=2) {
		m.b.data[i] = (unsigned char)(msg[k]&0xff);
		m.b.data[i+1] = (unsigned char)((msg[k]>>8)&0xff);
	}

	return m;
}

static int make_reply(unsigned type, uint32_t a, char ?data[], unsigned data_size)
{
	int16_t tmp;
	unsigned i, k;

	reply.opcode = type&0xff;
	reply.a.packetnumber = a; /* here it's irrelevant which field of the union is used. */

	if (data_size <= 0) {
		return 0;
	}

	/* FIXME another option is to transfer only FOE_DATA_SIZE */
	if (data_size > FOE_DATA_SIZE) {
		return -1;
	}

	if (!isnull(data)) {
		for (i=0, k=0; i<data_size; i+=2, k++) {
			tmp = data[i+1]&0xff;
			reply.b.data[i+FOE_HEADER_SIZE] = ((tmp<<8)&0xff00) | (data[i]&0xff);
		}
	} else {
		for (i=0; i<FOE_DATA_SIZE; i++) {
			reply.b.data[i+FOE_HEADER_SIZE] = 0;
		}
	}

	return (i+FOE_HEADER_SIZE);
}

/* draft for handle idle_state */
static handle_idle_state()
{
	int nextState = FOE_STATE_IDLE;
	return nextState;
}

/* draft for handle read_state */
static handle_read_state()
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

/* public function */
int foe_init(void)
{
	state = FOE_STATE_IDLE;
	current_fp = 0;

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
	unsigned int dataSize = 0;
	uint32_t packetNumber = 0;

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
					make_reply(FOE_ERROR, FOE_ERR_NOTFOUND, null, 0);
				}
				/* prepare first data package */
				dataSize = foefs_read(current_fp, FOE_DATA_SIZE, data); /* FIXME should work with reference here */
				packetNumber++;
				make_reply(FOE_DATA, packetNumber, data, dataSize);
				ret = 1;
			} else {
				state = FOE_STATE_IDLE;
				make_reply(FOE_ERROR, FOE_ERR_ILLEGAL, null, 0);
				ret = 1;
			}
			break;

		case FOE_WRITE:
			if (current_fp <= 0 && foefs_free() > 0) {
				state = FOE_STATE_WRITE;
				make_reply(FOE_ACK, 0, null, 0);
			} else {
				state = FOE_STATE_IDLE;
				make_reply(FOE_ERROR, FOE_ERR_DISKFULL, null, 0);
			}
			ret = 1;
			break;

		default:
			state = FOE_STATE_IDLE;
			make_reply(FOE_ERROR, FOE_ERR_UNDEF, null, 0);
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
			packetNumber++;
			make_reply(FOE_DATA, packetNumber, data, dataSize);
			break;

		case FOE_ERROR:
			/* abort transmission */
			state = FOE_STATE_IDLE;
			foefs_close(current_fp);
			make_reply(FOE_UNUSED, 0, null, 0); /* clear reply package */
			ret = 1;
			break;

		default:
			state = FOE_STATE_IDLE;
			foefs_close(current_fp);
			make_reply(FOE_ERROR, FOE_ERR_UNDEF, null, 0);
			ret = 1;
			break;
		}
		break;

	case FOE_STATE_WRITE:
		/* expected data.req, error.req */
		switch (rec.opcode) {
		case FOE_DATA:
			/* handle data */
			foefs_write(current_fp, FOE_DATA_SIZE, rec.b.data); /* FIXME add error check */

			/* FIXME sadly the ACK response isn't recognized by SOEM at the moment * /
			make_reply(FOE_ACK, rec.a.packetnumber, null, 0);
			ret = 1;
			// */
			break;

		case FOE_ERROR:
			/* abort transmission */
			state = FOE_STATE_IDLE;
			make_reply(FOE_UNUSED, 0, null, 0); /* clear reply package */
			foefs_close(current_fp);
			ret = 1;
			break;

		default:
			state = FOE_STATE_IDLE;
			make_reply(FOE_ERROR, FOE_ERR_UNDEF, null, 0);
			foefs_close(current_fp);
			ret = 1;
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

	for (i=0; i<FOE_DATA_SIZE; i+=2) {
		tmp = reply.b.data[i+1];
		data[k++] = (reply.b.data[i]&0xff) | ((tmp<<8)&0xff00);
	}

	return k;
}
