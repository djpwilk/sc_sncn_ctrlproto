/* eoe.xc
 *
 * Copyright 2012-2013, Synapticon GmbH. All rights reserved.
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

#define MAX_ETHERNET_FRAME_BYTES   (MAX_ETHERNET_FRAME+3)
#define MAX_ETHERNET_FRAME_UINT    (MAX_ETHERNET_FRAME_BYTES/4)

struct _ethernet_packet {
	union {
		unsigned char frameb[MAX_ETHERNET_FRAME_BYTES]; ///< package payload byte representation
		unsigned framei[MAX_ETHERNET_FRAME_UINT];      ///< package payload int32 representation
	} f;
	int ready;                               ///< this packet is ready for transmission
	unsigned size;                           ///< payload size of this package
	unsigned currentpos;                     ///< current position of read
	unsigned nextFragment;                   ///< fragment counter (starts with 0)
};

/* FIXME should I really butther the rx/tx ethernet packets??? */
static struct _ethernet_packet ethernet_packet_rx[MAX_ETHERNET_BUFFER];
static struct _ethernet_packet ethernet_packet_tx[MAX_ETHERNET_BUFFER];

static void reset_ethernet_packet(struct _ethernet_packet &ep)
{
	int i;

	for (i=0; i<MAX_ETHERNET_FRAME_BYTES; i++) {
		ep.f.frameb[i]=0;
	}

	ep.ready=0;
	ep.size=0;
	ep.currentpos=0;
	ep.nextFragment=0;
}

static int parse_packet(uint16_t msg[], unsigned size, struct _eoe_packet &ep)
{
	int i=0, k=0;
	unsigned dataLength = 2*(size-2); /* size is word count, dataLength is byte count, remove the 2 header words */

	ep.type = (char)(msg[0]&0x000f);
	ep.eport = (char)((msg[0]&0x00f0)>>4);

	ep.lastFragment = (char)((msg[0]>>8)&0x01);
	ep.timeAppended = (char)((msg[0]>>9)&0x01);
	ep.timeRequest = (char)((msg[0]>>10)&0x01);
	ep.fragmentNumber = (char)(msg[1]&0x2f);
	ep.a.offset = (char)((msg[1]>>6)&0x2f);
	ep.frameNumber = (char)((msg[1]>>12)&0x0f);

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

/* FIXME at least for transmission, build a 3 stage buffer (size: 3*1518 + overhead) */
int eoe_tx_handler(chanend eoe, unsigned size)
{
	/* read eoe channel
	 * until complete package is finished
	 * if no RX is in progress split package and successivly send the chunks
	 *
	 * size is given as number of 32-bit words
	 */
	unsigned otmp;
	unsigned pos = 0;

	//printstr("[DEBUG EOE TX] working eoe_tx_handler() rec. size: "); printintln(size);
	while (pos<((MAX_ETHERNET_FRAME+3)/4) && pos<size) {
#if 0
		select  {
		case eoe :> otmp :
#endif
			eoe :> otmp;
			ethernet_packet_tx[current].f.framei[pos++] = otmp;
			/*
			ethernet_packet_tx[current].f.frameb[pos++] = otmp&0xff;
			ethernet_packet_tx[current].f.frameb[pos++] = (otmp>>8)&0xff;
			 */
#if 0
			break;

		default:
			/* no packet waiting in transfere channel */
			return 0;
			break;
		}
#endif
	}

	/* construct ethercat packet */
	ethernet_packet_tx[current].ready = 1;
	ethernet_packet_tx[current].size = size*4; /* set size as number of octets */
	ethernet_packet_tx[current].currentpos = 0;
	ethernet_packet_tx[current].nextFragment = 0; /* FIXME */

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
	static int lastRecFragment = -1; /* last received fragment number - resets to 0 if new ethernet frame starts */
	static int currentFrameNumber = -1; /* current number of the ethernet frame. */
	/* static char receiveFrame = 0; / * we receive frame */

	struct _eoe_packet inpacket;

	packetSize = parse_packet(msg, size, inpacket);

	/* FIXME when is a duplicate fragment send??? This is not necessary a
	 * error, it could result in a not acknoledged packet, but there is no
	 * such ack I could use! */
	/* ignore duplicate packets - no error, just wait for the next valid packet */
	if (inpacket.fragmentNumber == lastRecFragment) {
		printstr("[DEBUG ERR eoe.xc] Received duplicate Fragment\n");
		return -1;
	}

	/* a out of order fragment is received, something went wrong during transmission */
	if (inpacket.fragmentNumber < lastRecFragment) {
		printstr("[DEBUG ERR eoe.xc] Malicious Fragment (fragmentNr/ lasRecFragment) ");
		printint(inpacket.fragmentNumber); printstr(" "); printintln(lastRecFragment);
		lastRecFragment = -1;
		currentFrameNumber = -1;
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
		lastRecFragment = -1;
		currentFrameNumber = -1;
		reset_ethernet_packet(ethernet_packet_rx[0]); /* FIXME make a reset function */
		return -1;
	}

	/* FIXME check packet time, if too large abort transmission? * /
	if (inpacket.timeAppended == 1) {
	}
	// */

	/* FIXME handle optional SET_IP_PARAMETER (0x02) and SET_ADDRESS_FILTER (0x04) ??? */
	switch (inpacket.type) {
	/*case EOE_INIT_REQ: / * remove this, because this is not handled correctly -> EOE_IP_PARAM_REQ is equal */
	case EOE_FRAGMENT_REQ:
		rxoffset = ethernet_packet_rx[0].currentpos;
		if (rxoffset+packetSize >= MAX_ETHERNET_FRAME) {
			printstr("[DEBUG EOE_XC] Error, ethernet frame too big.\nThe first 10 bytes: ");
			for (i=0; i<10; i++) {
				printhex(ethernet_packet_rx[0].f.frameb[i]);
				printstr(" ");
			}
			printstr("\n");

			lastRecFragment = -1;
			currentFrameNumber = -1;
			reset_ethernet_packet(ethernet_packet_rx[0]);

			return -1;
		}

		for (i=0; i<packetSize && i<MAX_EOE_DATA; i++) {
			ethernet_packet_rx[0].f.frameb[rxoffset+i] = inpacket.b.data[i];
		}

		ethernet_packet_rx[0].currentpos = rxoffset+packetSize;
		ethernet_packet_rx[0].size += packetSize;

		if (inpacket.lastFragment == 1) {
			unsigned datalen = ethernet_packet_rx[0].size;

			if (datalen<64) /* FIXME should be 60, but then 4 bytes are missing - guess: it's the missing FCS */
				datalen=64;

			//for (i=0; i<(ethernet_packet_rx[0].size+3)/4; i++) {
			for (i=0; i<(datalen+3)/4; i++) {
				eoe <: ethernet_packet_rx[0].f.framei[i];
			}

			/* Extended debug * /
			printstr("DEBUG Packet received: ");
			for (i=0; i<ethernet_packet_rx[0].size; i++) {
				printhex(ethernet_packet_rx[0].f.frameb[i]);
				printstr(" ");
			}
			printstr("\n");
			/ * end debug */
#if 0
			for (i=0; i<ethernet_packet_rx[0].size; i++) {
				tmp = ethernet_packet_rx[0].f.frameb[i];
				i++;
				tmp |= ((unsigned)ethernet_packet_rx[0].frame[i]<<8)&0xff00;
				eoe <: tmp;
				//eoe <: (int)ethernet_packet_rx[0].frame[i];
			}
#endif

			sig <: 1;
			eoe_state.state = EOE_STATE_IDLE/*RX_LAST_FRAGMENT*/;
			reset_ethernet_packet(ethernet_packet_rx[0]);
			currentFrameNumber = -1;
			lastRecFragment = -1;
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
/**
 * @brief Get eoe reply
 *
 * @param msg[] reference to outgoing msg array
 * @return Number of 16-bit words to send
 */
unsigned eoe_get_reply(uint16_t msg[])
{
	int i;
	int k=0;
	unsigned tmpl, tmph;
	unsigned length=0;
	struct _eoe_packet ep;
	static unsigned ethCurrentFrame = 0;

	//printstr("[DEBUG eoe_get_reply()] Building EoE reply\n");
	if (ethernet_packet_tx[0].ready == 0) {
		return 0;
	}

	/* construct header */
	ep.type = EOE_FRAGMENT_REQ;
	ep.eport = 0; /* FIXME check port usage */
	ep.timeAppended = 0; /* depends on mii TX_TIMESTAMP_END_OF_PACKET */
	ep.timeRequest = 0;
	ep.reserved = 0x00;
	ep.a.offset = 0; /* is set below if current size is known! */
	//ep.fragmentNumber = 0;
	ep.fragmentNumber = ethernet_packet_tx[0].nextFragment & 0x2f;
	ethernet_packet_tx[0].nextFragment += 1; /* FIXME if last fragment the nextFragment field should be 0 and ethernet_packet_tx should be cleared */
	ep.frameNumber = ethCurrentFrame;
	ethCurrentFrame = (ethCurrentFrame<0xf) ? ethCurrentFrame+1 : 0; /* CurrentFrame is a 4 bit sequence counter */

	if (((ethernet_packet_tx[0].currentpos + EOE_MAX_DATA_SIZE) >= ethernet_packet_tx[0].size) ||
		(ethernet_packet_tx[0].size < EOE_MAX_DATA_SIZE)) {
		//printstr("[DEBUG eoe.xc] +++ set last fragment\n");
		ep.lastFragment = 1;
	} else {
		ep.lastFragment = 0;
		ethernet_packet_tx[0].nextFragment++;
	}

	//unsigned startidx = ethernet_packet_tx[0].currentpos;

	/* build send packet */
	/* FIXME fix header construction */
	//msg[k] = (ep.type & (ep.eport<<4)) & (ep.lastFragment & (ep.timeAppended<<1) & (ep.timeRequest<<2))<<8;
	msg[0] = (ep.type&0x0f);
	msg[0] |= (ep.eport<<4)&0xf0;
	msg[0] |= (ep.lastFragment<<8)&0x100;
	msg[0] |= (ep.timeAppended<<9)&0x200;
	msg[0] |= (ep.timeRequest<<10)&0x400;
	/* remaining 5 bit are reserved so 0, no need to set explicitly */
	//printstr("DEBUG first header word: "); printhexln(msg[0]);

	msg[1] = ep.fragmentNumber&0x3f;
	msg[1] |= (ep.a.offset<<6)&0xfc0;
	msg[1] |= (ep.frameNumber<<12)&0xf000;

	k=2;

	for (i=ethernet_packet_tx[0].currentpos; i<EOE_MAX_DATA_SIZE && i<(ethernet_packet_tx[0].size-ethernet_packet_tx[0].currentpos); i+=2, k++) {
		tmpl = ethernet_packet_tx[0].f.frameb[i];
		tmph = ethernet_packet_tx[0].f.frameb[i+1];
		msg[k] = ((tmph<<8)&0xff00) | (tmpl&0xff);
	}

	length += k; /* number of 16-bit words */
	ethernet_packet_tx[0].currentpos += i;

	/* fix offset field for real value */
	ep.a.offset = (i+31)/32; /* number of bytes is used here */
	msg[1] |= (ep.a.offset<<6)&0xfc0;

	if (ep.lastFragment)
		reset_ethernet_packet(ethernet_packet_tx[0]);

	return length;
}

int eoe_check_chunks(void)
{
	if (ethernet_packet_tx[0].currentpos < ethernet_packet_tx[0].size) {
		return 1;
	}

	return 0;
}

