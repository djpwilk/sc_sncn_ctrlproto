/* eoe.xc
 *
 * Copyright 2012, Synapticon GmbH. All rights reserved.
 * Author: Frank Jeschke <jeschke@fjes.de>
 */

#include "eoe.h"

#include <print.h>

struct {
	int state;
	int rx_buffer;  ///< index of used rx buffer
	int tx_buffer;  ///< index of used tx buffer
} eoe_state;

static int sendstate;
static int used_rx_buffer;
static int used_tx_buffer;
static int tx_packet_ready;
static const unsigned current = 0; /* FIXME to make use of input buffer rxCurrent and txCurrent should be globals */

struct _ethernet_packet {
	unsigned char frame[MAX_ETHERNET_FRAME]; ///< package payload
	int ready;                               ///< this packet is ready for transmission
	unsigned size;                           ///< payload size of this package
	unsigned currentpos;                     ///< current position of read
	unsigned nextFragment;                   ///< fragment counter (starts with 0)
};

/* FIXME should I really butther the rx/tx ethernet packets??? */
static struct _ethernet_packet ethernet_packet_rx[MAX_ETHERNET_BUFFER];
static struct _ethernet_packet ethernet_packet_tx[MAX_ETHERNET_BUFFER];

static void reset_ethernet_packet(struct _ethernet_packet ep)
{
	int i;

	for (i=0; i<MAX_ETHERNET_FRAME; i++) {
		ep.frame[i]=0;
	}

	ep.ready=0;
	ep.size=0;
	ep.currentpos=0;
	ep.nextFragment=0;
}

static int parse_packet(uint16_t msg[], unsigned size, struct _eoe_packet ep)
{
	int i;
	unsigned dataLength = size;

	ep.type = (msg[0]>>4)&0x0f;
	ep.eport = msg[0]&0x0f;
	ep.lastFragment = (msg[1]>>8)&0x01;
	ep.timeAppended = (msg[1]>>7)&0x01;
	ep.timeRequest = (msg[1]>>6)&0x01;
	ep.fragmentNumber = msg[2]&0x2f;
	ep.a.offset = /* upper two bits of msg[2] and some bits of msg[3] */
	ep.frameNumber = /* 4 bits of msg[3] */

	dataLength = size-4;
	if (ep.timeAppended == 1) {
		dataLength -= 4;
	}

	for (i=4; i<dataLength && i<size; i++) {
		ep.b.data[i-4] = msg[i];
	}

	if (ep.timeAppended == 1) {
		ep.timestamp = ((uint32_t)msg[i]<<24) && ((uint32_t)msg[i+1]<<16) && ((uint32_t)msg[i+2]<<8) && msg[i+3]; /* FIXME check for quirks */
	}

	return 0;
}


void eoe_init(void)
{
	int i;

	sendstate = EOE_STATE_IDLE;
	tx_packet_ready = 0;
	used_rx_buffer = 0;
	used_tx_buffer = 0;

	eoe_state.state = EOE_STATE_IDLE;

	for (i=0; i<MAX_ETHERNET_BUFFER; i++) {
		reset_ethernet_packet(ethernet_packet_rx[i]);
		reset_ethernet_packet(ethernet_packet_tx[i]);
	}
}

int eoe_tx_handler(chanend eoe, unsigned size)
{
	/* read eoe channel
	 * until complete package is finished
	 * if no RX is in progress split package and successivly send the chunks
	 */
	unsigned otmp;
	unsigned pos = 0;

	while (pos<MAX_ETHERNET_FRAME && pos<size) {
		select  {
		case eoe :> otmp :
			eoe :> otmp;
			ethernet_packet_tx[current].frame[pos++] = otmp&0xff;
			ethernet_packet_tx[current].frame[pos++] = (otmp>>8)&0xff;
			break;

		default:
			/* no packet waiting in transfere channel */
			return 0;
			break;
		}
	}

	/* construct ethercat packet */
	ethernet_packet_tx[current].ready = 1;
	ethernet_packet_tx[current].size = size;
	ethernet_packet_tx[current].currentpos = 0;
	ethernet_packet_tx[current].nextFragment = 0;

	return 1;
}

/* FIXME check if it is possible to push packet segments directly to mii */
int eoe_rx_handler(chanend eoe, uint16_t msg[], unsigned size)
{
	int ret = 0;
	int type;
	int eport;
	unsigned i;

	struct _eoe_packet inpacket;

	parse_packet(msg, size, inpacket);

	/* FIXME check packet time, if too large abort transmission? * /
	if (inpacket.timeAppended == 1) {
	}
	 */

	switch (eoe_state.state) {
	case EOE_STATE_IDLE:
		if (inpacket.type == EOE_INIT_REQ) {
			/* do something with the request! */
			eoe_state.state = EOE_STATE_RX_FRAGMENT;

			for (i=0; i<size && i<MAX_EOE_DATA; i++) {
				ethernet_packet_rx[0].frame[ethernet_packet_rx[0].currentpos] = inpacket.b.data[i];
			}
		}
		break;

	case EOE_STATE_RX_FRAGMENT:
		if (inpacket.type == EOE_FRAGMENT_REQ) {
			for (i=0; i<size && i<MAX_EOE_DATA; i++) {
				ethernet_packet_rx[0].frame[ethernet_packet_rx[0].currentpos] = inpacket.b.data[i];
			}

			if (inpacket.lastFragment == 1) {
				for (i=0; i<ethernet_packet_rx[0].size; i++) {
					eoe <: (int)ethernet_packet_rx[0].frame[i];
				}

				eoe_state.state = EOE_STATE_IDLE/*RX_LAST_FRAGMENT*/;
				reset_ethernet_packet(ethernet_packet_rx[0]);
			}
		}
		break;

#if 0
	case EOE_STATE_RX_LAST_FRAGMENT:
		/* last fragment is received, push ethernet frame to ethernet mii */
		for (i=0; i<ethernet_packet_rx[0].size; i++) { /* */
			eoe <: (int)ethernet_packet_rx[0].frame[i];
		}
		break;
#endif

#if 0/* since there are two separate buffer for rx and tx this state isn't necessary */
	case EOE_STATE_TX_FRAGMENT:
		ret = 1; /* currently in tx state so no RX allowed */
		printstr("[WARN eoe_rx_handler()] Concurrency race occured\n");
		break;
#endif

	default: /* Error if unknown state is set */
		printstr("[ERROR eoe_rx_handler()] Reached unknown state, try to recover\n");
		eoe_state.state = EOE_STATE_IDLE;
		reset_ethernet_packet(ethernet_packet_rx[0]);
		break;
	}

	return ret;
}

int eoe_tx_ready(void)
{
	return ethernet_packet_tx[current].ready;
}

/* FIXME rename eoe_get_reply() -> eoe_get_tx_packet() */
/* FIXME add last segment check and clear buffer after last segment */
unsigned eoe_get_reply(uint16_t msg[])
{
	int i;
	int k=0;
	unsigned tmpl, tmph;
	unsigned length=0;
	struct _eoe_packet ep;

	if (ethernet_packet_tx[0].ready == 0) {
		return length;
	}

	/* construct header */
	ep.type = EOE_FRAGMENT_REQ;
	ep.eport = 0; /* FIXME check port usage */
	ep.timeAppended = 0; /* depends on mii TX_TIMESTAMP_END_OF_PACKET */
	ep.timeRequest = 0;
	ep.reserved = 0x00;
	//ep.fragmentNumber = 0;
	ep.fragmentNumber = ethernet_packet_tx[0].nextFragment & 0x2f;
	ethernet_packet_tx[0].nextFragment += 1; /* FIXME if last fragment the nextFragment field should be 0 and ethernet_packet_tx should be cleared */

	if ((ethernet_packet_tx[0].currentpos + EOE_MAX_DATA_SIZE) >= ethernet_packet_tx[0].size) {
		ep.lastFragment = 1;
	} else {
		ep.lastFragment = 0;
	}

	//unsigned startidx = ethernet_packet_tx[0].currentpos;

	/* build send packet */
	/* FIXME fix header construction */
	msg[k] = (ep.type & (ep.eport<<4)) & (ep.lastFragment & (ep.timeAppended<<1) & (ep.timeRequest<<2))<<8;
	length = 2*k;

	for (i=ethernet_packet_tx[0].currentpos; i<EOE_MAX_DATA_SIZE && i<(ethernet_packet_tx[0].size-ethernet_packet_tx[0].currentpos); i+=2, k++) {
		tmpl = ethernet_packet_tx[0].frame[i];
		tmph = ethernet_packet_tx[0].frame[i+1];
		msg[k] = (tmph<<8)&0xff00 | tmpl&0xff;
	}

	length += 2*k;
	ethernet_packet_tx[0].currentpos += i;

	return length;
}

int eoe_check_chunks(void)
{
	if (ethernet_packet_tx[0].currentpos < ethernet_packet_tx[0].size) {
		return 1;
	}

	return 0;
}

