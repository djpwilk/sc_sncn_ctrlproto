/**
 * Implementation of the CoE handling of the EtherCAT module.
 *
 * @author  Frank Jeschke <jeschke@fjes.de>
 * 2012-2013 Synapticon GmbH
 */

#ifndef ECAT_COE_H
#define ECAT_COE_H

#include <stdint.h>

#define COE_MAX_HEADER_SIZE  2
#define COE_MAX_MSG_SIZE     124
#define COE_MAX_DATA_SIZE    (COE_MAX_MSG_SIZE-COE_MAX_HEADER_SIZE)

/* can service (4 bit wide) */
#define COE_SERVICE_EMERGENCY     0x01  /* Emergency */
#define COE_SERVICE_SDO_REQ       0x02  /* SDO Request */
#define COE_SERVICE_SDO_RSP       0x03  /* SDO Response */
#define COE_SERVICE_TXPDO         0x04  /* TxPDO */
#define COE_SERVICE_RXPDO         0x05  /* RxPDO */
#define COE_SERVICE_TXPDO_REMOTE  0x06  /* TxPDO remote request */
#define COE_SERVICE_RXPDO_REMOTE  0x07  /* RxPDO remote request */
#define COE_SERVICE_SDO_INFO      0x08  /* SDO Information */

/* SDO information request types (7 bit wide) */
#define COE_SDOI_GET_ODLIST_REQ        0x01 /* Get OD List Request */
#define COE_SDOI_GET_ODLIST_RSP        0x02 /* Get OD List Response */
#define COE_SDOI_OBJDICT_REQ           0x03 /* Get Object Description Request */
#define COE_SDOI_OBJDICT_RSP           0x04 /* Get Object Description Response */
#define COE_SDOI_ENTRY_DESCRIPTION_REQ 0x05 /* Get Entry Description Request */
#define COE_SDOI_ENTRY_DESCRIPTION_RSP 0x06 /* Get Entry Description Response */
#define COE_SDOI_INFO_ERR_REQ          0x07 /* SDO Info Error Request */

/* fragment indicator (1 bit wide) */
#define COE_SDO_LAST_FRAGMENT     0x00  /* last SDO Information fragment */
#define COE_SDO_MORE_FRAGMENTS    0x01  /* SDO Information fragments will follow */

/* data set size is 2 bit wide */
#define COE_DATA_SET_SIZE_4       0x00  /* 4 octet data */
#define COE_DATA_SET_SIZE_3       0x01  /* 3 octet data */
#define COE_DATA_SET_SIZE_2       0x02  /* 2 octet data */
#define COE_DATA_SET_SIZE_1       0x03  /* 1 octet data */

/* command specifier (3 bit wide) */
#define COE_CMD_DOWNLOAD_SEG_REQ  0x00  /* download segment request */
#define COE_CMD_DOWNLOAD_SEG_RSP  0x01  /* download segment response */
#define COE_CMD_DOWNLOAD_REQ      0x01  /* download request */
#define COE_CMD_DOWNLOAD_RSP      0x03  /* download response */
#define COE_CMD_UPLOAD_REQ        0x02  /* upload request */
#define COE_CMD_UPLOAD_RSP        0x02  /* upload response */
#define COE_CMD_UPLOAD_SEG_REQ    0x03  /* upload segment request */
#define COE_CMD_UPLOAD_SEG_RSP    0x00  /* upload segment response */


#define COE_CMD_ABRTT_REQ         0x04  /* abort transfer request */


/* abort codes */
#define COE_ABORT_TOGGLE_BIT          0x05030000 /* Toggle bit not changed */
#define COE_ABORT_PROTO_TIMEOUT       0x05040000 /* SDO protocol timeout */
#define COE_ABORT_CLIENTSERVER        0x05040001 /* Client/Server command specifier not valid or unknown */
#define COE_ABORT_MEMORY              0x05040005 /* Out of memory */
#define COE_ABORT_UNSUPPORTED         0x06010000 /* Unsupported access to an object */
#define COE_ABORT_READ_WO             0x06010001 /* Attempt to read to a write only object */
#define COE_ABORT_WRITE_RO            0x06010002 /* Attempt to write to a read only object */
#define COE_ABORT_OBJNOTEXIST         0x06020000 /* The object does not exist in the object directory */
#define COE_ABORT_NOPDOMAPPING        0x06040041 /* The object can not be mapped into the PDO */
#define COE_ABORT_EXCEED_PDO          0x06040042 /* The number and length of the objects to be mapped would exceed the PDO length */
#define COE_ABORT_PARAM_INCOMPATIBLE  0x06040043 /* General parameter incompatibility reason */
#define COE_ABORT_INTERNAL_INCOMP     0x06040047 /* General internal incompatibility in the device */
#define COE_ABORT_HW_ERROR            0x06060000 /* Access failed due to a hardware error */
#define COE_ABORT_TYPE_MISMATCH       0x06070010 /* Data type does not match, length of service parameter does not match */
#define COE_ABORT_TYPE_MISMATCH_HI    0x06070012 /* Data type does not match, length of service parameter too high */
#define COE_ABORT_TYPE_MISMATCH_LO    0x06070013 /* Data type does not match, length of service parameter too low */
#define COE_ABORT_NO_SUBINDEX         0x06090011 /* Subindex does not exist */
#define COE_ABORT_RANGE_EXCEED        0x06090030 /* Value range of parameter exceeded (only for write access) */
#define COE_ABORT_VALUE_WRHIGH        0x06090031 /* Value of parameter written too high */
#define COE_ABORT_VALUE_WRLOW         0x06090032 /* Value of parameter written too low */
#define COE_ABORT_MINMAX              0x06090036 /* Maximum value is less than minimum value */
#define COE_ABORT_ERROR               0x08000000 /* General error */
#define COE_ABORT_NOTRANS             0x08000020 /* Data cannot be transferred or stored to the application */
#define COE_ABORT_NOTRANS_LOCAL       0x08000021 /* Data cannot be transferred or stored to the application because of local control */
#define COE_ABORT_NOTRANS_STATE       0x08000022 /* Data cannot be transferred or stored to the application because of the present device state */
#define COE_ABORT_NO_OBJECT_DICT      0x08000023 /* Object dictionary dynamic generation fails or no object dictionary is present */


/*
 * Static object dictionary entries
 *
 * Attention: keep in sync with global syncmanger config of ET1100!
 */

/* sync manager communication type values */
#define COE_SMCT_UNUSED          0
#define COE_SMCT_MAILBOX_RX      1
#define COE_SMCT_MAILBOX_TX      2   /* slave to master */
#define COE_SMCT_PDO_OUT         3
#define COE_SMCT_PDO_IN          4   /* slave to master */


/* ODE index 0x1C00, subindex is the position within the array */
#if 0
struct _sm_comm_type_ode {
	uint8_t sm_channels = 4;
	uint8_t sm0 = COE_SMCT_MAILBOX_RX;
	uint8_t sm1 = COE_SMCT_MAILBOX_TX;
	uint8_t sm2 = COE_SMCT_PDO_IN;
	uint8_t sm3 = COE_SMCT_PDO_OUT;
	uint8_t sm4 = COE_SMCT_UNUSED;
} SM_CommType_ODE;
#endif

uint8_t SM_CommType_ODE[] = {
	4,
	COE_SMCT_MAILBOX_RX,
	COE_SMCT_MAILBOX_TX,
	COE_SMCT_PDO_IN,
	COE_SMCT_PDO_OUT,
	COE_SMCT_UNUSED
};

/* sync manager channel object dictionary entry - index 0x1C10 - 0x1C2F */
struct _sm_channel {
	uint8_t sm_pdo_channels = 4; /* or 4? */
	uint16_t pdo1 = 0x1600; /* RxPDO 1 */
	uint16_t pdo2 = 0x1601; /* RxPDO 2 */
	uint16_t pdo3 = 0x1A00; /* TxPDO 1 */
	uint16_t pdo4 = 0x1A01; /* TxPDO 2 */
} SM_PDO_Channels;

/* sync manager synchronization object dictionary entry - index 0x1C30 - 0x1C4F */
/* FIXME: this is optional, but makes sense, don't get it if (and how) this
 * coudl be assigned to specific SMs */
struct {
	uint8_t params = 1;
	uint16_t synctype = 1; /* syncronized with AL Event */
	/*uint32_t cycletime =  / * optional */
} SM_PDO_Synchronization;
/* data structures */

struct _coe_header {
	unsigned int number;      /* 9 bit - depending on COE service */
	/* unsigned char reserved;   3 bit */
	unsigned char service;    /* 4 bit - used with COE_SERVICE_* */
};


/**
 * @brief
 *
 * @param
 * @return
 */
int coe_init(void);

/**
 * @brief
 *
 * @param
 * @return
 */
int coe_rx_handler(chanend coe, char buffer[], unsigned size);

/**
 * @brief
 *
 * @param
 * @return
 */
int coe_tx_handler();

/**
 * @brief
 *
 * @param
 * @return
 */
int coe_get_reply();

/**
 * @brief
 *
 * @param
 * @return
 */
int coe_reply_ready();

#endif /* ECAT_COE_H */
