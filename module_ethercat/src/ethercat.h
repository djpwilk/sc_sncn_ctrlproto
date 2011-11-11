/* ethercat.h
 *
 * Copyright 2011, Synapticon GmbH. All rights reserved.
 * Author: Frank Jeschke <jeschke@fjes.de>
 */

#ifndef ETHERCAT_H
#define ETHERCAT_H

#include <stdint.h>

enum EC_MailboxProtocolTypes {
	ERROR_PACKET=0,
	VENDOR_BECKHOFF_PACKET,
	EOE_PACKET,
	COE_PACKET,
	FOE_PACKET,
	SOE_PACKET,
	VOE_PACKET=0xf
};

struct _ec_mailbox_header {
	uint16_t length;   /* length of data area */
	uint16_t address;  /* originator address */
	uint8_t  channel;  /* =0 reserved for future use */
	uint8_t  priority; /* 0 (lowest) to 3 (highest) */
	uint8_t  type;     /* Protocol types -> enum EC_MailboxProtocolTypes */
	uint8_t  control;  /* sequence number to detect duplicates */
};

/**
 * @brief Main ethercat handler function.
 *
 * This function should run in a separate thread on the XMOS core controlling the I/O pins for
 * EtherCAT communication.
 * 
 * For every packet send or received from or to this EtherCAT handler, the
 * first word transmitted indicates the number of words to follow (the packet
 * itself).
 *
 * @param c_coe_r push received CAN packets
 * @param c_coe_s read packets to send as CAN
 * @param c_eoe_r push received Ethernet packets
 * @param c_eoe_s read packets to send as Ethernt
 * @param c_foe_r push received File packets
 * @param c_foe_s read packets to send as File
 * @param c_pdo_r push received File packets
 * @param c_pdo_s read packets to send as File
 */
void ecat_handler(chanend c_coe_r, chanend c_coe_s,
			chanend c_eoe_r, chanend c_eoe_s,
			chanend c_foe_r, chanend c_foe_s,
			chanend c_pdo_r, chanend c_pdo_s);

int ecat_init(void);

int ecat_reset(void);

#endif /* ETHERCAT_H */
