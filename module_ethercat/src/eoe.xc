/* eoe.xc
 *
 * Copyright 2012, Synapticon GmbH. All rights reserved.
 * Author: Frank Jeschke <jeschke@fjes.de>
 */

//#include <xtcp.h>
//#include <uip.h>
#include "eoe.h"

#define MAX_ETHERNET_FRAME   1522   /* Max. number of bytes within a ethernet frame. FIXME couldn't it be less? */
#define MAX_ETHERNET_BUFFER  2

struct {
	int state;
	int rx_buffer;  ///< index of used rx buffer
	int tx_buffer;  ///< index of used tx buffer
} eoe_state;

static int sendstate;
static int used_rx_buffer;
static int used_tx_buffer;

struct _ethernet_packet {
	unsigned char frame[MAX_ETHERNET_FRAME];
	int readytosend;
	unsigned usedsize;
	unsigned currentpos;
};

/* FIXME should I really store a complete of 2 ethernet packets??? */
static struct _ethernet_packet ethernet_packet_rx[MAX_ETHERNET_BUFFER];
static struct _ethernet_packet ethernet_packet_tx[MAX_ETHERNET_BUFFER];

static void reset_ethernet_packet(struct _ethernet_packet ep)
{
	int i;

	for (i=0; i<MAX_ETHERNET_FRAME; i++) {
		ep.frame[i]=0;
	}

	ep.readytosend=0;
	ep.usedsize=0;
	ep.currentpos=0;
}

static int check_received(chanend eoe_rx) /* !!! */
{
	unsigned tmp;
	unsigned expected;
	unsigned received = 0;
	int rbytes = 0;

	unsigned buffer[256];

	select {
	case eoe_rx :> tmp :
		/* handle input */
		expected = tmp;
		/* FIXME avoid stall if expected words aren't reached. */
		while (received < expected) {
			eoe_rx :> tmp;
			buffer[received] = tmp;
		}
		break;

	default:
		break;
	}

	return rbytes;
}

static int check_send(chanend eoe_tx) /* !!! */
{
	int err = -1;

	switch (sendstate) {
	case EOE_STATE_IDLE:
		err = 0; /* nothing to do */
		break;

	case EOE_STATE_RX_FRAGMENT:
		break;

	case EOE_STATE_RX_LAST_FRAGMENT:
		break;

	case EOE_STATE_TX_FRAGMENT:
		break;

	default:
		err = -1;
		break;
	}

	return err;
}

void eoe_init(void)
{
	int i;

	sendstate = EOE_STATE_IDLE;
	used_rx_buffer = 0;
	used_tx_buffer = 0;

	eoe_state.state = EOE_STATE_IDLE;

	for (i=0; i<MAX_ETHERNET_BUFFER; i++) {
		reset_ethernet_packet(ethernet_packet_rx[i]);
		reset_ethernet_packet(ethernet_packet_tx[i]);
	}
}

int eoe_tx_handler(uint16_t msg[], unsigned size)
{
	switch (eoe_state.state) {
	case EOE_STATE_IDLE:
		break;
	case EOE_STATE_RX_FRAGMENT:
		break;
	case EOE_STATE_RX_LAST_FRAGMENT:
		break;
	case EOE_STATE_TX_FRAGMENT:
		break;
	}

	return 0;
}

int eoe_rx_handler(chanend eoe, uint16_t msg[], unsigned size)
{
	int ret = 0;
	int type;
	int eport;


	switch (eoe_state.state) {
	case EOE_STATE_IDLE:
		//parse_packet(msg, size);
		type = (msg[0]>>4)&0x0f;
		eport = msg[0]&0x0f;

		break;

	case EOE_STATE_RX_FRAGMENT:
		break;

	case EOE_STATE_RX_LAST_FRAGMENT:
		break;

	case EOE_STATE_TX_FRAGMENT:
		ret = 1; /* currently in rx state so no TX allowed */
		break;

	default: /* Error if unknown state is set */
		break;
	}

	return ret;
}

