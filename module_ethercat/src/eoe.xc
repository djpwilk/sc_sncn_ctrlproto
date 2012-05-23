/* eoe.xc
 *
 * Copyright 2012, Synapticon GmbH. All rights reserved.
 * Author: Frank Jeschke <jeschke@fjes.de>
 */

//#include <xtcp.h>
#include <uip.h>

#define MAX_ETHERNET_FRAME   1024   /* FIXME What is the max number of bytes in a ehternet frame? */

enum eSendState {
	IDLE,
	READY,
	SEND,
	STOP
};

static enum eSendState sendstate;

struct _ethernet_packet {
	unsigned char ethernet_fame[MAX_ETHERNET_FRAME];
	int readytosend;
	unsigned usedsize;
	unsigned currentpos;
};

/* FIXME should I really store a complete of 3 ethernet packets??? */
static struct _ethernet_packet ethernet_packet_rx[3];
static struct _ethernet_packet ethernet_packet_tx[3];

static int check_received(chanend eoe_rx)
{
	unsigned tmp;
	unsigned expect;
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

static int check_send(eoe_tx)
{
	int err = -1;
	switch (sendstate) {
	case IDLE:
		err = 0; /* nothing to do */
		break;

	case READY:
		break;

	case SEND:
		break;

	case STOP:
		break;

	default:
		err = -1;
		break;
	}

	return err;
}

void eoe_init(void)
{
	sendstate = IDLE;
	return 0;
}

int eoe_handler(chanend eoe_rx, chanend eoe_tx)
{
	int ret = 0;

	while (1) {
		/* make the work */
		ret = check_received(eoe_rx);
		ret = check_send(eoe_tx);
	}

	return 0;
}

