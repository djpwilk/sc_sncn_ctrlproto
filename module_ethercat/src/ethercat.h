/**
 * @file ethercat.h
 *
 * @author Frank Jeschke <jeschke@fjes.de>
 * @note Copyright 2011,2012,2013 Synapticon GmbH. All rights reserved.
 */

#ifndef ETHERCAT_H
#define ETHERCAT_H

#include "foe_chan.h"

#include <stdint.h>

#define DATA_REQUEST     1

enum EC_MailboxProtocolTypes {
	ERROR_PACKET=0,            ///< Error Packet
	VENDOR_BECKHOFF_PACKET,    ///< Beckhoff vendor specific packet
	EOE_PACKET,                ///< Ethernet-over-EtherCAT packet
	COE_PACKET,                ///< CAN over EtherCAT packet
	FOE_PACKET,                ///< File over EtherCAT packet
	SOE_PACKET,                ///< SoE
	VOE_PACKET=0xf             ///< Vendor specific mailbox packet
};

struct _ec_mailbox_header {
	uint16_t length;           ///< length of data area
	uint16_t address;          ///< originator address
	uint8_t  channel;          ///< =0 reserved for future use
	uint8_t  priority;         ///< 0 (lowest) to 3 (highest)
	uint8_t  type;             ///< Protocol types -> enum EC_MailboxProtocolTypes
	uint8_t  control;          ///< sequence number to detect duplicates
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
 * @param c_eoe_sig signals for ethernet handling
 * @param c_foe_r push received File packets
 * @param c_foe_s read packets to send as File
 * @param c_pdo_r push received File packets
 * @param c_pdo_s read packets to send as File
 */
void ecat_handler(chanend c_coe_r, chanend c_coe_s,
			chanend c_eoe_r, chanend c_eoe_s, chanend c_eoe_sig,
			chanend c_foe_r, chanend c_foe_s,
			chanend c_pdo_r, chanend c_pdo_s);

/**
 * @brief Init function for the EtcherCAT module
 *
 * This function must be called in the first place to enable ethercat service.
 *
 * @return  0 on success
 */
int ecat_init(void);

/**
 * @brief Reset ethercat handler and servicces
 *
 * @warning Currently this function is only a stub and doesn't perform any
 * functionality.
 */
int ecat_reset(void);

#endif /* ETHERCAT_H */
