/* ethercat.xc
 *
 * Copyright 2011, Synapticon GmbH. All rights reserved.
 * Author: Frank Jeschke <jeschke@fjes.de>
 */

#include "ethercat.h"
#include "alstate.h"
#include "foefs.h"
#include "foe.h"

#include <platform.h>
#include <xs1.h>
#include <print.h> /* FIXME remove after debug */

#define ET_USER_RAM       0x0f80
#define ET_HEADER_SZ      6
#define MAX_BUFFER_SIZE    128

#define EC_CS_SET()       ecatCS <: 0
#define EC_CS_UNSET()     ecatCS <: 1
#define EC_RD_SET()       ecatRD <: 0
#define EC_RD_UNSET()     ecatRD <: 1
#define EC_WR_SET()       ecatWR <: 0
#define EC_WR_UNSET()     ecatWR <: 1

#define EC_DATA_READ(x)   ecatData <: x
#define EC_DATA_WRITE(x)  ecatData :> x

#define EC_BUSY(x)        ecatBUSY :> x
#define EC_IRQ(x)         ecatIRQ :> x

#define EC_ADDRESS(x)     ecatAddress <: x

#define BYTESWAP_16(x)   (((x>>8)&0x00ff) | ((x<<8)&0xff00))

/* --- sync manager defines and global variables --- */

#define EC_SYNCM_COUNT  8 /* # of syncmanager channels for et1100 */
#define EC_SYNCM_BASE   0x0800
#define EC_SYNCM_GET_CONTROL_STATUS(manid)     ((EC_SYNCM_BASE+manid*8)+0x4)

#define SYNCM_BUFFER_MODE         0x00
#define SYNCM_MAILBOX_MODE        0x02
#define SYNCM_BUFFER_MODE_READ    (SYNCM_BUFFER_MODE | 0x04)
#define SYNCM_MAILBOX_MODE_READ   (SYNCM_MAILBOX_MODE | 0x04)
#define SYNCM_BUFFER_MODE_WRITE   SYNCM_BUFFER_MODE
#define SYNCM_MAILBOX_MODE_WRITE  SYNCM_MAILBOX_MODE


struct _syncm {
	uint16_t address;
	uint16_t size;
	uint8_t control;
		/* uint8_t type; / * buffer | mailbox mode */
		/* uint8_t direction / * ECAT:r PDI:w  OR ECAT:w PDI:r */
	uint8_t status;
	uint8_t activate;
	uint8_t pdi_ctrl;
};

struct _syncm manager[EC_SYNCM_COUNT];

#if 0
/* FIXME use this instead of the SyncManager struct above! I.e. one for coe, eoe, foe each. */
struct _mbox {
	uint16_t rd_address;
	uint16_t rd_size;
	uint16_t wr_address;
	uint16_t wr_size;
	char rd_full;
	char wr_full;
};
#endif

/* --- fmmu defines and global variables --- */

#define EC_FMMU_BASE    0x0600
#define EC_FMMU_COUNT   8

struct _fmmu {
	uint32_t logical_start;
	uint16_t offset;
	uint8_t reg_start_bit;
	uint8_t reg_stop_bit;
	uint16_t physical_start_address;
	uint8_t phy_start_bit;
	uint8_t reg_type;
	uint8_t reg_activate;
};

struct _fmmu fmmu[EC_FMMU_COUNT];


/* --- in/output ports --- */

/* FIXME add defines to *.xn file */
on stdcore[0]: out port ecatCS = XS1_PORT_1M;
on stdcore[0]: out port ecatRD = XS1_PORT_1L;
on stdcore[0]: out port ecatWR = XS1_PORT_1K;
on stdcore[0]: in  port ecatBUSY = XS1_PORT_1J;
on stdcore[0]: in  port ecatIRQ = XS1_PORT_1I;
//on stdcore[0]: in port ecatEEPROM = XS1_PORT_???;
/* reset ??? */

on stdcore[0]: out port ecatAddress = XS1_PORT_16B;
on stdcore[0]:     port ecatData = XS1_PORT_16A;

static uint16_t escStationAddress;
static uint16_t escStationAddressAlias;
static uint16_t escDlStatus;
static uint16_t alControl;
static uint16_t alStatus;
static uint16_t alStatusCode;
static uint8_t escConfiguration;
static uint16_t alEventMasq;
static uint8_t pdiControl;

static uint16_t rxErrorCounter;
static uint8_t ecatProcError;
static uint8_t pdiError;

static int packet_pending; /* indicate packet ready for sending to master */
static int foeReplyPending;

/**
 * @brief send packages to the connected channel endpoint.
 */
static void ecat_send_handler(chanend c_handler, uint16_t packet[], uint16_t size)
{
	unsigned int i;

	c_handler <: (unsigned int)size; /* transmit size first so the handler knows the number of bytes that follow */
	for (i=0; i<size; i++) {
		c_handler <: (unsigned int)packet[i];
	}
}

/* low level read of ET1100 register */
static uint16_t ecat_read(uint16_t address)
{
	timer t;
	unsigned int time;
	uint8_t busy;
	uint16_t adr;
	uint16_t data = 0x0000;

	adr = BYTESWAP_16(address);

	ecatAddress <: adr; /* set address */
	ecatRD <: 0; /* set read active */

	/* wait until busy is released */
	ecatBUSY :> busy;

	/* FIXME should wait until busy becomes busy (1->0) ? */
	while (busy == 1)
		ecatBUSY :> busy;
	/* /
	while (busy == 0)
		ecatBUSY :> busy;
	 */

	/* after max 5ns the data are valid */
	t :> time;
	t when timerafter(time+1) :> void; /* this is about 10ns */

	ecatData :> data; /* read data word */

	ecatRD <: 1; /* read sequence finished */

	/* satisfy t_read_delay */
	t :> time;
	t when timerafter(time+1) :> void; /* this is about 10ns */

#if 0
	if (address>=0x1000) {
		printstr("Access to process ram\n");
	}
	if ((address>=0x1000) && ((data&0x2000) == 0)) {
		printstr("Address/Data: ");
		printhex(address);
		printstr("/");
		printhexln(data);
	}
#endif

	return data;
}

static unsigned int ecat_read_block(uint16_t addr, uint16_t len, uint16_t buf[])
{
	unsigned int wordcount = 0;
	uint16_t address = addr;

	while (wordcount<len) {
		buf[wordcount] = ecat_read(address);
		wordcount++;
		address+=2;
	}

	return wordcount;
}

/* low level write to ET1100 register */
static int ecat_write(uint16_t address, uint16_t word)
{
	timer t;
	unsigned int time;
	uint16_t adr;

	address = address&0xffff;
	adr = BYTESWAP_16(address);

	ecatAddress <: adr;
	ecatData <: word;
	ecatWR <: 0;

	t :> time;
	t when timerafter(time+2) :> void; /* wait 20ns  to satisfy t_WR_active */

	ecatWR <: 1;

	t:>time;
	t when timerafter(time+2) :> void; /* wait t_wr_delay >= 10ns to make sure the next WR access is in time. */

	return 0;
}

static unsigned int ecat_write_block(uint16_t addr, uint16_t len, uint16_t buf[])
{
	unsigned int wordcount = 0;
	uint16_t address = addr;

	while (wordcount<len) {
		ecat_write(address, buf[wordcount]);
		wordcount++;
		address+=2;
	}

	return wordcount;
}

/* FIXME check if split up in ecat_read_buffer() and ecat_read_mailbox() makes sense. */
static int ecat_process_packet(uint16_t start, uint16_t size, uint8_t type,
				chanend c_coe, chanend c_eoe, chanend c_foe, chanend c_pdo)
{
	const uint8_t mailboxHeaderLength = 3; /* words */
	uint16_t buffer[MAX_BUFFER_SIZE];
	uint16_t address = start;
	int error = AL_NO_ERROR;
	int data_start;
	int i, wc;
	uint16_t offset=0;

	uint16_t wordCount;
	uint16_t packetWords;

	struct _ec_mailbox_header h;

	if ((size&0x0001) == 0) { /* size is even */
		packetWords = size/2;
	} else {
		packetWords = (size+1)/2;
	}

	wordCount = (MAX_BUFFER_SIZE<packetWords) ? MAX_BUFFER_SIZE : packetWords;

	switch (type) {
	case SYNCM_BUFFER_MODE:
		//printstr("DEBUG ecat_process_packet(): processing Buffer Mode\n");
		ecat_read_block(start, wordCount, buffer);
		ecat_send_handler(c_pdo, buffer, wordCount);

		/* FIXME: makeshift echo the buffer packet */
		/* FIXME is it really necessary to echo the packet? */
		for (i=0; i<8; i++) {
			if ((manager[i].activate&0x01) != 1) {
				continue;
			}

			if ((manager[i].control&0x0f) == SYNCM_BUFFER_MODE_WRITE) {
				address = manager[i].address;
				for (wc=0; wc<wordCount; wc++) {
					ecat_write(address, buffer[wc]);
					address+=2;
				}

				/* padding to make buffer active */
				for (wc; wc<manager[i].size; wc++) {
					ecat_write(address, buffer[wc]);
					address+=2;
				}
			}
		}
		/* end buffer echo test */

		/* DEBUG print * /
		printstr("DEBUG ecat_handler: ");
		for (wc=0; wc<wordCount; wc++) {
			printstr(" ");
			printhex(buffer[wc]);
		}
		printstr("\n");
		// */
		break;

	case SYNCM_MAILBOX_MODE:
		ecat_read_block(start, mailboxHeaderLength, buffer);          // read mailbox header
		h.length = buffer[0];
		h.address = buffer[1];
		h.channel = buffer[2]&0x003f;
		h.priority = (buffer[2]>>6)&0x0003;
		h.type = (buffer[2]>>8)&0x000f;
		h.control = (buffer[2]>>12)&0x0007;

#if 0 /* DEBUG */
		printstr("[DEBUG ecat_process_packet()] Received mailbox packet of type: 0x");
		printhexln(h.type);
#endif

		if (wordCount < (h.length/2)) {
			wordCount = wordCount;
			printstr("Error length in mailbox header is too large: 0x");
			printhexln(h.length);
			error = AL_ERROR;
		} else {
			wordCount = h.length/2;
		}

		offset = start+(mailboxHeaderLength*2);
		ecat_read_block(offset, packetWords-mailboxHeaderLength/*wordCount*/, buffer);  // read mailbox data

		switch (h.type) {
		case EOE_PACKET:
			//printstr("DEBUG ethercat: received EOE packet.\n");
			ecat_send_handler(c_eoe, buffer, wordCount);
			break;

		case COE_PACKET:
			//printstr("DEBUG ethercat: received COE packet, but this is not processed.\n");
			ecat_send_handler(c_coe, buffer, wordCount);
			//error = AL_MBX_COE;
			break;

		case FOE_PACKET:
			//printstr("DEBUG ethercat: received FOE packet, start processing.\n");
			//ecat_send_handler(c_foe, buffer, wordCount);
			foeReplyPending = foe_parse_packet(buffer, wordCount);
			break;

		case SOE_PACKET: /* ignored unsupported */
			error = AL_MBX_SOE;
			break;

		case VENDOR_BECKHOFF_PACKET: /* ignored unsupported */
			error = AL_MBX_VOE;
			break;

		case VOE_PACKET: /* ignored unsupported */ 
			/* FIXME: [FOE as VOE] due to the need for a quick solution the FOE channel is used as VOE channel */
			//ecat_send_handler(c_foe, buffer, wordCount);
			error = AL_MBX_VOE;
			break;

		case ERROR_PACKET:
			printstr("Error packet received: ");
			printhexln(h.type);
			error = AL_ERROR;
			break;

		default:
			printstr("found unknown package: ");
			printhexln(h.type);
			error = AL_ERROR;
			break;
		}
		break;
	}

	return error;
}

/* Send mailbox packet
 *
 * start_address  start address of memory area of mailbox
 * max_size       maximum size of mailbox memory area
 * type           type of mailbox message
 * buffer[]       buffer to copy to mailbox memory area
 * sendsize       size of mailbox data in words
 * return AL_NO_ERROR or AL_ERROR
 */
static int ecat_mbox_packet_send(uint16_t start_address, uint16_t max_size, int type,
					uint16_t buffer[], uint16_t sendsize)
{
	uint16_t sendbuffer[256];
	uint16_t temp = 0;
	unsigned int pos = 0;
	unsigned int i = 0;
	uint16_t size = max_size/2; /* max_size in bytes, size in words */
	uint16_t sent = 0;

	struct _ec_mailbox_header h;

	h.length = sendsize*2; /* FIXME add handling of uneven sized payload */
	h.address = escStationAddress;
	h.channel = 0;
	h. priority = 1;
	h.type = type;
	h.control = 1; /* start value 1, 0 is reserved */

	sendbuffer[pos++] = h.length;
	sendbuffer[pos++] = h.address;
	sendbuffer[pos] = h.channel&0x003f;
	temp = h.priority&0x3;
	sendbuffer[pos] |= (temp<<6);
	temp = h.type&0xf;
	sendbuffer[pos] |= (temp<<8);
	temp = h.control&0x7;
	sendbuffer[pos] |= (temp<<12);
	pos++;

	for (i=0; i<sendsize; i++, pos++) {
		sendbuffer[pos] = buffer[i];
	}

	/* Padding: The last byte in SyncM mailbox buffer must be written to trigger send signal */
	for (i=pos; i<size; i++) {
		sendbuffer[i] = 0x00;
	}

	sent = ecat_write_block(start_address, size, sendbuffer);
	if (sent != size) {
		printstr("Error wrong return size\n");
		return AL_ERROR;
	}

	return AL_NO_ERROR;
}

static void ecat_update_error_counter(void)
{
	uint16_t data = 0;

	rxErrorCounter = ecat_read(0x0300);

	data = ecat_read(0x030c);
	ecatProcError = data & 0xff;
	pdiError = (data>>8) & 0xff;
}

/* ----- sync manager ----- */

static void ecat_read_syncm(void)
{
	int i;
	uint16_t address = EC_SYNCM_BASE;
	uint16_t data;

	for (i=0; i<EC_SYNCM_COUNT; i++) {
		address = EC_SYNCM_BASE+i*8;

		data = ecat_read(address);
		manager[i].address = data&0xffff;
		address+=2;
		data = ecat_read(address);
		manager[i].size = data&0xffff;

		address+=2;
		data = ecat_read(address);
		manager[i].control = data&0xff;
		manager[i].status = (data>>8)&0xff;

		address+=2;
		data = ecat_read(address);
		manager[i].activate = data&0xff;
		manager[i].pdi_ctrl = (data>>8)&0xff;
	}
}

static void ecat_clear_syncm(void)
{
	int i=0;

	for (i=0; i<EC_SYNCM_COUNT; i++) {
		manager[i].address = 0;
		manager[i].size = 0;
		manager[i].control = 0;
		manager[i].status = 0;
		manager[i].activate = 0;
		manager[i].pdi_ctrl = 0;
	}
}

/* ----- fmmu ----- */

static void ecat_read_fmmu_config(void)
{
	int i;
	uint16_t address = EC_FMMU_BASE;
	uint16_t data;
	uint32_t tmp;

	for (i=0; i<EC_FMMU_COUNT; i++) {
		address = EC_FMMU_BASE+i*8;

		tmp = ecat_read(address);
		fmmu[i].logical_start = tmp;
		tmp = ecat_read(address+2);
		fmmu[i].logical_start = ((tmp<<16)&0xffff0000) | fmmu[i].logical_start;

		fmmu[i].offset = ecat_read(address+4);

		data = ecat_read(address+6);
		fmmu[i].reg_start_bit = data&0xff;
		fmmu[i].reg_stop_bit = (data>>8)&0xff;

		fmmu[i].physical_start_address = ecat_read(address+8);

		data = ecat_read(address+10);
		fmmu[i].phy_start_bit = data&0xff;
		fmmu[i].reg_type = (data>>8)&0xff;

		data = ecat_read(address+12);
		fmmu[i].reg_activate = data&0xff;
	}
}

static void ecat_clear_fmmu(void)
{
	int i;

	for (i=0; i<EC_FMMU_COUNT; i++) {
		fmmu[i].logical_start = 0;
		fmmu[i].logical_start = 0;
		fmmu[i].offset = 0;
		fmmu[i].reg_start_bit = 0;
		fmmu[i].reg_stop_bit = 0;
		fmmu[i].physical_start_address = 0;
		fmmu[i].phy_start_bit = 0;
		fmmu[i].reg_type = 0;
		fmmu[i].reg_activate = 0;
	}
}

/* ----- al state machine ----- */

/**
 * @brief State machine for application layer.
 *
 * FIXME handle BOOTSTRAP separately
 * 
 * @param reqState   master requested state
 * @return  new state, either reqstate is confirmed or fallback.
 * @return  0 if no error occures, error word (@see AL error codes)
 */
{uint16_t, uint16_t} al_state_machine(uint16_t reqState)
{
	uint16_t newstate = 0;
	uint16_t error = AL_NO_ERROR;

	/* FIXME better handling of error state changes */
	if ((reqState & AL_STATE_ERRORBIT) > 0) {
		/* error confirmed by master */
		return {AL_STATE_INIT, AL_ERROR};
	}

	switch (reqState) {
	case AL_STATE_INIT:
		ecat_clear_syncm();
		ecat_clear_fmmu();
		newstate = AL_STATE_INIT;
		error = AL_NO_ERROR;
		break;

	case AL_STATE_BOOTSTRAP: /* possible use for configuration (FoE) */
		newstate = AL_STATE_INIT|AL_STATE_ERRORBIT;
		error = AL_BOOTSTRAP_NOT_SUPPORTED;
		break;

	case AL_STATE_PREOP:
		/* master configured DL-Address registers and SyncM registers */
		ecat_read_syncm();
		ecat_clear_fmmu();
		newstate = AL_STATE_PREOP;
		error = AL_NO_ERROR;
		break;

	case AL_STATE_SAFEOP:
		/* master conf. parameters using mailbox (process data);
		 * master configures SyncM channels for process data and fmmu channels
		 */
		ecat_read_syncm(); /* reread for process data */
		ecat_read_fmmu_config();
		newstate = AL_STATE_SAFEOP;
		error = AL_NO_ERROR;
		break;

	case AL_STATE_OP:
		/* process input (master->client) mailbox communication outputs in safestate */
		newstate = AL_STATE_OP;
		error = AL_NO_ERROR;
		break;

	default:
		/* errornous state: set AL_REG_STATUS_CODE (error) and stop */
		newstate = AL_STATE_INIT|AL_STATE_ERRORBIT;
		error = AL_ERROR;
		break;
	}

	if (error != AL_NO_ERROR)
		newstate |= 0x0010; /* set error indicator */

	return {newstate, error};
}

static int ecat_read_fmmu(uint16_t data[])
{
	/* Assumption: there is only one FMMU read channel */
	int i,j;
	unsigned int wordCount=0;
	uint16_t address;

	for (i=0; i<EC_FMMU_COUNT; i++) {
		if (fmmu[i].reg_type == 0x01 && fmmu[i].reg_activate == 1) {
			if ((fmmu[i].offset&0x0001) == 1) /* offset is odd */
				wordCount = (fmmu[i].offset+1) / 2;
			else
				wordCount = fmmu[i].offset /2;

			address = fmmu[i].physical_start_address;
			for (j=0; j<wordCount; j++) {
				data[j] = ecat_read(address);
				address+=2;
				/* DEBUG * /
				printstr("FMMU content ("); printint(j); printstr(") = ");
				printhexln(data[j]);
				// */
			}
		}
	}

	return 0;
}

static int ecat_write_fmmu(uint16_t data[])
{
	int i, j;
	uint16_t wordCount=0;
	uint16_t address;

	for (i=0; i<EC_FMMU_COUNT; i++) {
		if (fmmu[i].reg_type == 0x02 && fmmu[i].reg_activate == 1) {
			if ((fmmu[i].offset&0x00001) == 1) /* offset is odd */
				wordCount = (fmmu[i].offset+1) /2;
			else
				wordCount = fmmu[i].offset /2;

			address = fmmu[i].physical_start_address;
			for (j=0; j<wordCount; j++) {
				ecat_write(address, data[j]);
				address+=2;
			}

		}
	}

	return 0;
}

int ecat_get_fmmu(uint16_t data[])
{
	return 0;
}

int ecat_put_fmmu(uint16_t data[])
{
	return 0;
}

int ecat_init(void)
{
	uint16_t address = 0x0000;
	uint16_t data = 0x0;
	uint8_t busy = 1;
	char loaded = 0;
	int i;

	uint8_t newState;
	uint16_t stateError;
	uint8_t alState;
	uint8_t alError;
	uint16_t alEvent;

	uint32_t eeprom_config;

	timer t;
	unsigned time;

	ecatCS <: 1;
	ecatWR <: 1;
	ecatRD <: 1;
	ecatData <: 0x0000;

	for (i=0; i<EC_FMMU_COUNT; i++) {
		fmmu[i].logical_start = 0;
		fmmu[i].offset = 0;
		fmmu[i].reg_start_bit = 0;
		fmmu[i].reg_stop_bit = 0;
		fmmu[i].physical_start_address = 0;
		fmmu[i].phy_start_bit = 0;
		fmmu[i].reg_type = 0;
		fmmu[i].reg_activate = 0;
	}

	for (i=0; i<EC_SYNCM_COUNT; i++) {
		manager[i].address = 0;
		manager[i].size = 0;
		manager[i].control = 0;
		manager[i].status = 0;
		manager[i].activate = 0;
		manager[i].pdi_ctrl = 0;
	}

	t :> time;
	t when timerafter(time+10000) :> void; /* after 100ms the EEPROM should be loaded */

#if 0
	/* currently not available */
	while (!loaded) {
		ecatEEPROM :> loaded;
		t :> time;
		t when timerafter(time+1) :> void;
	}
#endif

	/* Force init state on startup */
	EC_CS_SET();
	ecat_write(AL_REG_STATUS, (uint16_t)AL_STATE_INIT);
	ecat_write(AL_REG_STATUS_CODE, (uint16_t)AL_NO_ERROR);

#if 0
	while (1) {
		data = ecat_read(0x0110 /* ESC DL Status */);
		printstr("ESC DL Status: ");
		printhexln(data);

		if ((data&0x0001) > 0) {
			printstr("EEPPROM loaded, PDI operational\n");
			break; /* finish, continue with operational */
		}
	}

	/* read PDI configuration * /
	data = ecat_read(0x150);
	printstr("Register 0x150: ");
	printhexln(data);
	// */
	
	data = ecat_read(0x0152);
	printstr("Register 0x152: ");
	printhexln(data);
#endif

	EC_CS_UNSET();

	/* init foefs and foe */
	foefs_init();
	foe_init();
	foeReplyPending=0;

	return 0;
}

int ecat_reset(void)
{
	return -1;
}

void ecat_handler(chanend c_coe_r, chanend c_coe_s,
			chanend c_eoe_r, chanend c_eoe_s,
			chanend c_foe_r, chanend c_foe_s,
			chanend c_pdo_r, chanend c_pdo_s)
{
	timer t;
	unsigned int time;
	unsigned int i;
	uint16_t data = 0;
	uint16_t syncm_event = 0;

	uint16_t buffer[256];
	uint16_t fmmu_inbuf[64];
	uint16_t fmmu_outbuf[64];

	uint16_t al_state;
	uint16_t al_error = AL_NO_ERROR;
	uint16_t packet_error = AL_NO_ERROR;

	uint16_t out_buffer[256];
	uint16_t out_size = 0;
	uint16_t out_type = ERROR_PACKET;
	unsigned int otmp = 0;
	int pending_buffer = 0; /* no buffer to send */
	int pending_mailbox = 0; /* no mailbox to send */

	foemsg_t foeMessage;

	EC_CS_SET();
	while (1) {
		ecat_update_error_counter();

		data = ecat_read(AL_REG_EVENT_REQUEST_LOW);
		if (data & AL_CONTROL_EVENT) {
			data = ecat_read(AL_REG_CONTROL);
			{al_state, al_error} = al_state_machine(data&0x001f); /* bits 15:5 are reserved */
			ecat_write(AL_REG_STATUS, al_state);
			ecat_write(AL_REG_STATUS_CODE, al_error);
			//if (al_state==AL_STATE_OP)
				//printstr("Device OPERATIONAL\n");
			//printstr("new state: "); printhexln(al_state);
		}

		if ((al_state&0x10) > 0 || al_error != AL_NO_ERROR) {
			continue;
		}

		/* If preop state isn't reached there is no need to process mailbox/buffer communication. */
		if ((al_state&0xf) < AL_STATE_PREOP) {
			escStationAddress = ecat_read(0x0010);
			escStationAddressAlias = ecat_read(0x0012);
			continue;
		}

		for (i=0; i<8; i++) {
			packet_error = AL_NO_ERROR;

			if ((manager[i].activate&0x01) == 1) { /* sync manager is active */
				data = ecat_read(EC_SYNCM_GET_CONTROL_STATUS(i));
				manager[i].control = data&0xff;
				manager[i].status = (data>>8)&0xff;

				switch (manager[i].control&0x0f) {
				case SYNCM_BUFFER_MODE_READ:
					if ((manager[i].status & 0x01) == 1) { /* read buffer is accessible, buffer was successfully written */
						packet_error = ecat_process_packet(manager[i].address, manager[i].size, SYNCM_BUFFER_MODE,
									c_coe_s, c_eoe_s, c_foe_s, c_pdo_s);
					}
					break;

				case SYNCM_BUFFER_MODE_WRITE:
					/* send packets pending? */
					if (pending_buffer) {
						printstr("Write Buffer SyncM: ");
						printintln(i);
						/* FIXME check return value */
/* FIXME: disabled
						ecat_write_block(manager[i].address, out_size, out_buffer);
*/
						pending_buffer=0;
					}
					break;

				case SYNCM_MAILBOX_MODE_READ:
					//printstr("[DEBUG:] Mailbox ready to read.\n");
					if ((manager[i].status & 0x08) != 0) { /* mailbox full */
						//printstr("Read Mailbox SyncM: ");
						//printintln(i);
						//printstr("Mailbox address and size:\n");
						//printhexln(manager[i].address);
						//printhexln(manager[i].size);
						packet_error = ecat_process_packet(manager[i].address, manager[i].size, SYNCM_MAILBOX_MODE,
									c_coe_s, c_eoe_s, c_foe_s, c_pdo_s);
					}
					break;

				case SYNCM_MAILBOX_MODE_WRITE:
					/* send packets pending? */
					if (pending_mailbox == 1 /*&& manager[i].status == 0*/) {
						packet_error = ecat_mbox_packet_send(manager[i].address, manager[i].size,
									 out_type, out_buffer, out_size);
						pending_mailbox=0;
					}

					break;

				}
			}

			ecat_write(AL_REG_STATUS, al_state);
			ecat_write(AL_REG_STATUS_CODE, packet_error);

			if (packet_error != AL_NO_ERROR) { /* FIXME implement valid error handling */
				printstr("Packet error: 0x"); printhexln(packet_error);
			}
		}

		/* send pending mailbox data * /
		if (pending_mailbox) {
			printstr("DEBUG: Pending mailbox, calling ecat_mbox_packet_send()\n");
			packet_error = ecat_mbox_packet_send(manager[i].address, manager[i].size,
						 out_type, out_buffer, out_size);
		}
		// */

		/* check FMMU after AL_STATE_SAFEOP is reached * /
		if ((al_state&0xf) < AL_STATE_OP) {
			continue;
		}
		// */

		/* FIXME: check FMMU data */
		//ecat_read_fmmu(fmmu_inbuf);
		//ecat_write_fmmu(fmmu_outbuf); /* echo the current values */

		/* read incoming filehandles, if no mailbox is pending! */
		if (pending_mailbox != 1) {
			select {
			case c_coe_r :> otmp :
				printstr("DEBUG: processing outgoing CoE packets\n");
				out_type = COE_PACKET;
				out_size = otmp&0xffff;
				printhexln(out_size);
				for (i=0; i<out_size; i++) {
					c_coe_r :> otmp;
					out_buffer[i] = otmp&0xffff;
				}
				pending_mailbox=1;
				break;

			case c_eoe_r :> otmp :
				printstr("DEBUG: processing outgoing EoE packets\n");
				out_size = otmp&0xffff;
				out_type = EOE_PACKET;
				for (i=0; i<out_size; i++) {
					c_eoe_r :> otmp;
					out_buffer[i] = otmp&0xffff;
				}
				pending_mailbox=1;
				break;

			case c_foe_r :> otmp :
				printstr("DEBUG: receive FoE command (e.g. file access)\n");
				foe_file_access(otmp, c_foe_r);
				#if 0
				out_size = otmp&0xffff;
				//printstr("DEBUG: read: "); printhexln(out_size);
				//printstr("> ");
				out_type = FOE_PACKET;
				for (i=0; i<out_size; i++) {
					c_foe_r :> otmp;
					out_buffer[i] = otmp&0xffff;
					//printhex(out_buffer[i]);
				}
				foe_command = otmp;

				if (foe_request(out_buffer) == 1) {
					out_size = foe_get_reply(out_buffer);
					pending_mailbox=1;
				}
				#endif
				break;

			default:
				break;
			}

			/* FIXME Check for potential race conditions between this internal package and channel based communication! */
			if (pending_mailbox != 1 && foeReplyPending == 1) {
				out_size = foe_get_reply(out_buffer);
				out_type = FOE_PACKET;
				pending_mailbox = 1;
				foeReplyPending = 0;
			}
		}

		/* buffered things can be send anytime */
		select {
			case c_pdo_r :> otmp :
				printstr("DEBUG: processing outgoing PDO packets\n");
				out_size = otmp&0xffff;
				out_type = ERROR_PACKET; // no mailbox packet, unused there!
				for (i=0; i<out_size; i++) {
					c_pdo_r :> otmp;
					out_buffer[i] = otmp&0xffff;
				}
				pending_mailbox=1;
				break;

			default:
				break;
		}
	}
	EC_CS_UNSET();
}
