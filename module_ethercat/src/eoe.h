/* eoe.h
 *
 * Copyright 2012, Synapticon GmbH. All rights reserved.
 * Author: Frank Jeschke <jeschke@fjes.de>
 */

/* What to do here:
 * - get EoE header and check package
 * - forward ethernet resp. tcp/ip packet to application resp. module to handle
 *   these.
 */

/* FIXME check ETG for packet type numbers */
#define EOE_FRAME_TYPE_INIT_REQ      0x02
#define EOE_FRAME_TYPE_INIT_RESP     0x03
#define EOE_FRAME_TYPE_FRAGMENT_REQ  0x00

#define EOE_FRAME_TYPE_IP_PARAM      0x02
#define EOE_FRAME_TYPE_ADDR_FILTER   0x03


typedef struct
{
	unsigned FrameType:       4;
	unsigned Port:            4;
	unsigned LastFragment:    1;
	unsigned TimeAppended:    1;
	unsigned TimeRequested:   1;
	unsigned Reserved:        5;
	unsigned FragmentNumber:  6;
	unsigned CompleteSize:    6;
	unsigned FrameNumber:     4;
} TEOEHEADER;

void eoe_init(void);
int eoe_handler(chanend eoe_rx, chanend eoe_tx);
