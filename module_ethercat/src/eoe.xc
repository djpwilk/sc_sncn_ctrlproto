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

static int parse_packet(uint16_t msg[], unsigned size, struct _eoe_packet &ep)
{
	int i=0, k=0;
	unsigned dataLength = 2*size; /* size is word count, dataLength is byte count! */

	ep.type = (char)(msg[0]&0x000f);
	ep.eport = (char)((msg[0]&0x00f0)>>4);

	ep.lastFragment = (char)((msg[0]>>8)&0x01);
	ep.timeAppended = (char)((msg[0]>>9)&0x01);
	ep.timeRequest = (char)((msg[0]>>10)&0x01);
	ep.fragmentNumber = (char)(msg[1]&0x2f);
	ep.a.offset = (char)((msg[1]>>6)&0x2f);
	ep.frameNumber = (char)((msg[1]>>12)&0x0f);

	dataLength -= 4;
	if (ep.timeAppended == 1) {
		dataLength -= 4;
	}

	for (i=2, k=0; k<dataLength && i<size; i++, k+=2) {
		ep.b.data[k] = (char)msg[i]&0xff;
		ep.b.data[k+1] = (char)((msg[i]>>8)&0xff);
	}

	if (ep.timeAppended == 1) {
		ep.timestamp = ((uint32_t)msg[i+3]<<24) && ((uint32_t)msg[i+2]<<16) && ((uint32_t)msg[i+1]<<8) && (uint32_t)msg[i]; /* FIXME check for quirks */
	}

	return k;
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

	printstr("[DEBUG EOE TX] working eoe_tx_handler()\n");
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

/* FIXME refactor to push packet segments directly to mii */
int eoe_rx_handler(chanend eoe, chanend sig, uint16_t msg[], unsigned size)
{
	int ret = 0;
	int type;
	int eport;
	unsigned i, k;
	unsigned rxoffset;
	unsigned packetSize;
	unsigned tmp;
	static char lastRecFragment = 0; /* last received fragment number - resets to 0 if new ethernet frame starts */
	static char currentFrameNumber = -1; /* current number of the eternet frame. */
	/* static char receiveFrame = 0; / * we receive frame */

	struct _eoe_packet inpacket;

	packetSize = parse_packet(msg, size, inpacket);

	/* FIXME when is a duplicate fragment send??? This is not necessary a
	 * error, it could result in a not acknoledged packet, but there is no
	 * such ack! */
	if (inpacket.fragmentNumber == lastRecFragment) {
		//printstr("[DEBUG ERR eoe.xc] Received duplicate Fragment\n");
		/*
		lastRecFragment = 0;
		currentFrameNumber = 0;
		reset_ethernet_packet(ethernet_packet_rx[0]);
		 */
		return -1;
	}

	if (inpacket.fragmentNumber < lastRecFragment) {
		//printstr("[DEBUG ERR eoe.xc] Malicious Fragment\n");
		lastRecFragment = 0;
		currentFrameNumber = 0;
		reset_ethernet_packet(ethernet_packet_rx[0]); /* FIXME make a reset function */
		return -1;
	}

	lastRecFragment = inpacket.fragmentNumber;

	if (currentFrameNumber == -1) { /* a new ethernet frame has started */
		currentFrameNumber = inpacket.frameNumber;
	}

	/* check if ethernet frames are in order */
	if (inpacket.frameNumber != currentFrameNumber) {
		//printstr("[DEBUG ERR eoe.xc] Errornous packet received\n");
		lastRecFragment = 0;
		currentFrameNumber = 0;
		reset_ethernet_packet(ethernet_packet_rx[0]); /* FIXME make a reset function */
		return -1;
	}

	/* FIXME check packet time, if too large abort transmission? * /
	if (inpacket.timeAppended == 1) {
	}
	 */

	/* FIXME handle optional SET_IP_PARAMETER (0x02) and SET_ADDRESS_FILTER (0x04) ??? */
	switch (inpacket.type) {
	//case EOE_INIT_REQ: /* remove this, because maybe this is not handled correctly by IgH Master! -> EOE_IP_PARAM_REQ */
	case EOE_FRAGMENT_REQ:
	//if (inpacket.type == EOE_INIT_REQ || inpacket.type == EOE_FRAGMENT_REQ) 
		rxoffset = ethernet_packet_rx[0].currentpos;
		if (rxoffset+packetSize >= MAX_ETHERNET_FRAME) {
			printstr("[DEBUG EOE_XC] Error, ethernet frame too big.\nThe first 10 bytes: ");
			for (i=0; i<10; i++) {
				printhex(ethernet_packet_rx[0].frame[i]);
				printstr(" ");
			}
			printstr("\n");

			lastRecFragment = 0;
			currentFrameNumber = 0;
			reset_ethernet_packet(ethernet_packet_rx[0]);

			return -1;
		}

		for (i=0; i<packetSize && i<MAX_EOE_DATA; i++) {
			ethernet_packet_rx[0].frame[rxoffset+i] = inpacket.b.data[i];
		}

		ethernet_packet_rx[0].currentpos = rxoffset+packetSize;
		ethernet_packet_rx[0].size += packetSize;

		/** /
		printstr("[DEBUG EOE RX - frag] working eoe_rx_handler(");
		printint(eoe_state.state);
		printstr(")\n");
		printstr("[DEBUG EOE RX - frag] Working Frame No. ");
		printintln(inpacket.frameNumber);
		printstr("[DEBUG EOE RX - frag] Working Fragment No. ");
		printintln(inpacket.fragmentNumber);
		printstr("[DEBUG EOE RX - frag] Frame finished? ");
		printintln(inpacket.lastFragment);
		// */

		if (inpacket.lastFragment == 1) {
			for (i=0; i<ethernet_packet_rx[0].size; i++) {
				tmp = ethernet_packet_rx[0].frame[i];
				i++;
				tmp |= ((unsigned)ethernet_packet_rx[0].frame[i]<<8)&0xff00;
				eoe <: tmp;
				//eoe <: (int)ethernet_packet_rx[0].frame[i];
			}

			sig <: 1;
			eoe_state.state = EOE_STATE_IDLE/*RX_LAST_FRAGMENT*/;
			reset_ethernet_packet(ethernet_packet_rx[0]);
			currentFrameNumber = -1;
			lastRecFragment = 0;
			printstr("[DEBUG EOE RX] last fragment received\n");
		}
		break;
	case EOE_IP_PARAM_REQ:
	default:
		printstr("[DEBUG ERROR] Packet of type 0x");
		printhex(inpacket.type);
		printstr(" not supported\n");
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
		msg[k] = ((tmph<<8)&0xff00) | (tmpl&0xff);
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

