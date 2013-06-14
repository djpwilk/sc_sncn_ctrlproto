/* app_ethercat_test
 *
 * Test appliction for module_ethercat.
 *
 * Copyright 2011-2013, Synapticon GmbH. All rights reserved.
 * Author: Frank Jeschke <jeschke@fjes.de>
 */

#include <platform.h>
#include <print.h>
#include <xs1.h>

#include <ethercat.h>
#include <foefs.h>
#include <coecmd.h>

//#include <uip.h>
//#include <xtcp.h>

#define CIA402_APP    1

#define MAX_BUFFER_SIZE   1024

on stdcore[1] : out port ledBlue = LED_BLUE;
on stdcore[1] : out port ledGreen = LED_GREEN;
on stdcore[1] : out port ledRed = LED_RED;

/* example consumer */
static void consumer(
#ifndef CIA402_APP
	chanend coe_in, chanend coe_out,
#endif
	chanend eoe_in, chanend eoe_out)
{
	timer t;
	const unsigned int delay = 10;
	unsigned int time = 0;

	unsigned int inBuffer[MAX_BUFFER_SIZE];
	unsigned int outBuffer[MAX_BUFFER_SIZE];
	unsigned int tmp = 0;
	unsigned int size = 0;
	unsigned count = 0;
	unsigned int outType = -1; /* FIXME set with define */
	unsigned outSize;

	unsigned int foePacketNbr = 0;
	int i;

	for (i=0; i<MAX_BUFFER_SIZE; i++) {
		inBuffer[i] = 0;
		outBuffer[i] = 0;
	}

	while (1) {
		/* Receive data */
		select {
#ifndef CIA402_APP
		case coe_in :> tmp :
			/* the CoE packets are handled within module_ethercat, currently
			 * no data is provided for the application layer.
			 */
			inBuffer[0] = tmp&0xffff;
			printstr("[APP] Received COE packet\n");
			count=0;

			while (count < inBuffer[0]) {
				coe_in :> tmp;
				inBuffer[count+1] = tmp&0xffff;
				count++;
			}

			break;
#endif
		case eoe_in :> tmp :
			inBuffer[0] = tmp&0xffff;
			printstr("[APP] Received EOE packet\n");
			count=0;
			while (count < inBuffer[0]) {
				eoe_in :> tmp;
				inBuffer[count+1] = tmp&0xffff;
				count++;
			}
			break;

		}

		/* send data */
		switch (outType /*outBuffer[0]*/) {
#ifndef CIA402_APP
		case COE_PACKET:
			/* Sending of CoE packets isn't provided, the low level CoE handling
			 * is performed by module_ethercat.
			 */
			count=0;
			//printstr("[APP DEBUG] send CoE packet\n");
			outSize = outBuffer[0]+1;
			while (count<outSize) {
				coe_out <: outBuffer[count];
				count++;
			}
			outBuffer[0] = 0;
			outType = -1;
			break;
#endif
		case EOE_PACKET:
			count=0;
			//printstr("DEBUG send EoE packet\n");
			outSize = outBuffer[0]+1;
			while (count<outSize) {
				eoe_out <: outBuffer[count];
				count++;
			}
			outBuffer[0] = 0;
			outType = -1;
			break;

		default:
			break;
		}

		t :> time;
		t when timerafter(time+delay) :> void;
	}
}

/*
 * FoE Example handling
 */

/* request a file from the master */
static void get_file(chanend foe_out, char filename[])
{
	unsigned i, pos=0;
	unsigned outBuffer[20];
	outBuffer[1] = REQUEST_FILE;

	for (i=0, pos=2; filename[i] != '\n'; i++, pos++) {
		outBuffer[pos++] = filename[i];
	}

	outBuffer[0] = pos;

	for (i=0; i<pos; i++) {
		foe_out <: outBuffer[i];
	}
}

/* The following functions are a simple example on how to use the channel
 * abstraction to the file access system.
 * If a file is available it is read by check_file_access() and the filesystem
 * becomes formated for the next file.
 *
 * No storage is done here and only the functionality is tested.
 */
#define BUFFER_SIZE   1024

static void check_file_access(chanend foe_comm)
{
	char buffer[BUFFER_SIZE];
	unsigned i=0;
	int ctmp;
	unsigned size;

	foe_comm <: FOE_FILE_READ;
	foe_comm <: BUFFER_SIZE;

	foe_comm :> ctmp;
	switch (ctmp) {
	case FOE_FILE_DATA:
		foe_comm :> ctmp;
		printstr("[DEBUG FOE] file transfered:\n"); /* DEBUG */
		size = (unsigned int)ctmp;
		for (i=0; i<size; i++) {
			foe_comm :> ctmp;
			buffer[i] = (char)ctmp;
			printhex(buffer[i]); /* DEBUG */
		}
		printstr("\n");
		break;

	case FOE_FILE_ERROR:
		printstr("[check_file_access()] error is returned\n");
		break;

	default:
		printstr("[check_file_access()] Unexpected reply\n");
		break;
	}

	/* clean up file system to permit next foe transfere */
	foe_comm <: FOE_FILE_FREE;
	foe_comm :> ctmp;
	switch (ctmp) {
	case FOE_FILE_ACK:
		printstr("[check_file_access()] filesystem is clear again\n");
		break;
	case FOE_FILE_ERROR:
		printstr("[check_file_access()] error during filesystem clean up\n");
		break;
	default:
		printstr("[check_file_access()] unknon return value\n");
		break;
	}
}

static void check_file(chanend foe_comm, chanend foe_signal)
{
	timer t;
	unsigned time = 0;
	unsigned delay = 100000;
	char name[] = "test";
	unsigned i=0;
	int ctmp=0;
	int notification=0;

	/* wait some time until ethercat handler is ready */
	t :> time;
	t when timerafter(time+delay) :> void;

	while (1) {
		foe_signal :> notification;

		if (notification != FOE_FILE_READY) {
			t :> time;
			t when timerafter(time+delay) :> void;
			continue;
		}

		/* check if a file is present, FIXME: this could be realized by the signaling channel! */
		foe_comm <: FOE_FILE_OPEN;
		i=-1;
		do {
			i++;
			foe_comm <: (int)name[i];
		} while (name[i] != '\0');

		foe_comm :> ctmp;
		switch (ctmp) {
		case FOE_FILE_ERROR:
			printstr("Error file is not ready\n");
			break;

		case FOE_FILE_ACK:
			/* File is ready read it and print to std. out */
			check_file_access(foe_comm);
			break;

		default:
			printstr("Unknown state returned\n");
			break;
		}

		t :> time;
		t when timerafter(time+delay) :> void;
	}
}


#ifdef CIA402_APP
/* example implementation of CIA402 facility
 *
 * read PDOs and read/update OD entries
 */

/* TX Objects */
#define CANOD_STATUS            0x6041
#define CANOD_VELOCITY          0x606c
#define CANOD_POSITION          0x6064
#define CANOD_TORQUE            0x6077
#define CANOD_OP_MODE_DISP      0x6061

/* RX Objects */
#define CANOD_CONTROL           0x6040
#define CANOD_TARGET_VELOCITY   0x60ff
#define CANOD_TARGET_POSITION   0x607a
#define CANOD_TARGET_TORQUE     0x6071
#define CANOD_OP_MODE           0x6060

struct _cia402_values {
	unsigned char status;
	unsigned char modes;
	unsigned torque; /* 16 bit value !!! */
	unsigned position;
	unsigned velocity;
};

static void cia402_example(chanend coe_od, chanend coe_in, chanend pdo_in, chanend pdo_out)
{
	unsigned char status = 0;
	unsigned torque = 0;
	unsigned velocity = 0;
	unsigned position = 0;
	unsigned opmodes = 0;
	unsigned coein;

	unsigned int inBuffer[64];
	unsigned int outBuffer[64];
	unsigned int count=0;
	unsigned int outCount=0;
	unsigned int tmp;
	unsigned ready = 0;
	int i;

	struct _cia402_values values;

	timer t;
	const unsigned int delay = 100;
	unsigned int time = 0;

	/* read initial states */
	coe_od <: CAN_GET_OBJECT;
	coe_od <: CAN_OBJ_ADR(CANOD_STATUS, 0);
	coe_od :> tmp;
	status = (unsigned char)(tmp&0xff);
	if (status == 0) {
		coe_od <: CAN_SET_OBJECT;
		coe_od <: CAN_OBJ_ADR(CANOD_STATUS, 0);
		status = 0xaf;
		coe_od <: (unsigned)status;
		coe_od :> tmp;
		if (tmp == status) {
			printstr("successfully set status\n");
		}
	}

	coe_od <: CAN_GET_OBJECT;
	coe_od <: CAN_OBJ_ADR(CANOD_TORQUE, 0);
	coe_od :> torque;

	coe_od <: CAN_GET_OBJECT;
	coe_od <: CAN_OBJ_ADR(CANOD_VELOCITY, 0);
	coe_od :> velocity;

	coe_od <: CAN_GET_OBJECT;
	coe_od <: CAN_OBJ_ADR(CANOD_POSITION, 0);
	coe_od :> position;

	coe_od <: CAN_GET_OBJECT;
	coe_od <: CAN_OBJ_ADR(CANOD_OP_MODE, 0);
	coe_od :> position;

	while (1) {
		count = 0;

		pdo_in <: DATA_REQUEST;
		pdo_in :> count;
		for (i=0; i<count; i++) {
			pdo_in :> inBuffer[i];
			/*
			printstr("data "); printint(i);
			printstr(": "); printhexln(inBuffer[i]);
			 */
		}

		if (count>0) {
			values.status = inBuffer[0]&0xff;
			values.modes = (inBuffer[0]>>8)&0xff;
			values.torque = inBuffer[1]&0xffff;
			values.position = (inBuffer[2]<<16) | inBuffer[3];
			values.velocity = (inBuffer[4]<<16) | inBuffer[5];

			/* set the objects in the object dictionary */
			coe_od <: CAN_SET_OBJECT;
			coe_od <: CAN_OBJ_ADR(CANOD_STATUS, 0);
			coe_od <: (unsigned)values.status;
			coe_od :> tmp;

			coe_od <: CAN_SET_OBJECT;
			coe_od <: CAN_OBJ_ADR(CANOD_OP_MODE, 0);
			coe_od <: (unsigned)values.modes;
			coe_od :> tmp;

			coe_od <: CAN_SET_OBJECT;
			coe_od <: CAN_OBJ_ADR(CANOD_TORQUE, 0);
			coe_od <: (unsigned)values.torque;
			coe_od :> tmp;

			coe_od <: CAN_SET_OBJECT;
			coe_od <: CAN_OBJ_ADR(CANOD_POSITION, 0);
			coe_od <: (unsigned)values.position;
			coe_od :> tmp;

			coe_od <: CAN_SET_OBJECT;
			coe_od <: CAN_OBJ_ADR(CANOD_VELOCITY, 0);
			coe_od <: (unsigned)values.velocity;
			coe_od :> tmp;


			/* build reply - attention the parameter order is different than the receive side */
			pdo_out <: 6;
			pdo_out <: ((unsigned)values.modes<<8) | values.status;
			pdo_out <: values.position&0xffff;
			pdo_out <: (values.position>>16)&0xffff;
			pdo_out <: values.velocity&0xffff;
			pdo_out <: (values.velocity>>16)&0xffff;
			pdo_out <: values.torque;
		}

		t :> time;
		t when timerafter(time+delay) :> void;
	}
}
#else

/*
 * example PDO handling - receive and reply values
 */

static void pdo_handler(chanend pdo_out, chanend pdo_in)
{
	unsigned int inBuffer[64];
	unsigned int outBuffer[64];
	unsigned int count=0;
	unsigned int outCount=0;
	unsigned int tmp;
	unsigned ready = 0;
	int i;

	timer t;
	const unsigned int delay = 100;
	unsigned int time = 0;

	while (1){
		count = 0;

		pdo_in <: DATA_REQUEST;
		pdo_in :> count;
		for (i=0; i<count; i++) {
			pdo_in :> inBuffer[i];
			printstr("data "); printint(i);
			printstr(": "); printhexln(inBuffer[i]);
		}

		if (count>0) {
			pdo_out <: count;
			for (i=0; i<count; i++) {
				pdo_out <: inBuffer[i];
			}
		}

		t :> time;
		t when timerafter(time+delay) :> void;
	}
}
#endif

static void led_handler(void)
{
	timer t;
	const unsigned int delay = 50000000;
	unsigned int time = 0;
	int blueOn = 0;

	while (1) {
		t :> time;
		t when timerafter(time+delay) :> void;

		ledBlue <: blueOn;
		blueOn = ~blueOn & 0x1;
	}
}

int main(void) {
	chan coe_in;   ///< CAN from module_ethercat to consumer
	chan coe_out;  ///< CAN from consumer to module_ethercat
	chan eoe_in;   ///< Ethernet from module_ethercat to consumer
	chan eoe_out;  ///< Ethernet from consumer to module_ethercat
	chan eoe_sig;
	chan foe_in;   ///< File from module_ethercat to consumer
	chan foe_out;  ///< File from consumer to module_ethercat
	chan pdo_in;   ///< input to pdo_hanlder()
	chan pdo_out;  ///< output from pdo_handler()

	par {
		on stdcore[0] : {
			ecat_init();
			ecat_handler(coe_out, coe_in, eoe_out, eoe_in, eoe_sig, foe_out, foe_in, pdo_out, pdo_in);
		}

		on stdcore[0] : {
			consumer(
#ifndef CIA402_APP
				coe_in, coe_out,
#endif
				eoe_in, eoe_out  /*, foe_in, foe_out, pdo_in, pdo_out*/);
		}

		on stdcore[1] : {
			check_file(foe_out, foe_in);
		}

		/*
		on stdcore[1] : {
			led_handler();
		}
		*/

		on stdcore[1] : {
#ifdef CIA402_APP
			cia402_example(coe_out, coe_in, pdo_in, pdo_out);
#else
			pdo_handler(pdo_out, pdo_in);
#endif
		}
	}

	return 0;
}
