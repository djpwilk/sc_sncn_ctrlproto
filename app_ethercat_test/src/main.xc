/* app_ethercat_test
 *
 * Test appliction for module_ethercat.
 *
 * Copyright 2011, Synapticon GmbH. All rights reserved.
 * Author: Frank Jeschke <jeschke@fjes.de>
 */

#include <platform.h>
#include <print.h>
#include <xs1.h>

#include <ethercat.h>

#define MAX_BUFFER_SIZE   1024

on stdcore[1] : out port ledBlue = LED_BLUE;
on stdcore[1] : out port ledGreen = LED_GREEN;
on stdcore[1] : out port ledRed = LED_RED;

/* example consumer */
static void consumer(chanend coe_in, chanend coe_out, chanend eoe_in, chanend eoe_out,
			chanend foe_in, chanend foe_out/*, chanend pdo_in, chanend pdo_out*/)
{
	timer t;
	const unsigned int delay = 100000;
	unsigned int time = 0;

	unsigned int inBuffer[MAX_BUFFER_SIZE];
	unsigned int outBuffer[MAX_BUFFER_SIZE];
	unsigned int tmp = 0;
	unsigned int size = 0;
	unsigned count = 0;
	unsigned outSize;

	int i;

	for (i=0; i<MAX_BUFFER_SIZE; i++) {
		inBuffer[i] = 0;
		outBuffer[i] = 0;
	}

	while (1) {
		/* Receive data */
		select {
		case coe_out :> tmp :
			inBuffer[0] = tmp&0xffff;
			printstr("[APP] Received COE packet\n");
			count=0;
			while (count < inBuffer[0]) {
				coe_in :> tmp;
				inBuffer[count+1] = tmp&0xffff;
				count++;
			}
			break;

		case eoe_out :> tmp :
			inBuffer[0] = tmp&0xffff;
			printstr("[APP] Received EOE packet\n");
			count=0;
			while (count < inBuffer[0]) {
				eoe_in :> tmp;
				inBuffer[count+1] = tmp&0xffff;
				count++;
			}
			break;

		case foe_out :> tmp :
			size = (uint16_t)(tmp&0xffff);
			count=0;

			while (count < size) {
				tmp = 0;
				foe_out :> tmp;
				inBuffer[count] = (tmp&0xffff);
				count++;
			}

			/* Here comes the code to handle the package content! */
			printstr("[APP] Received FOE packet\n");
			printhexln(size);
			for (i=0;i<size;i++) {
				printstr("Received: ");
				printhexln(inBuffer[i]);
			}
			/* end example */
			break;

#if 0
		case pdo_out :> tmp :
			inBuffer[0] = tmp&0xffff;
			printstr("[APP] Received PDO packet: \n");

			count = 0;
			while (count<inBuffer[0]) {
				tmp = 0;
				pdo_out :> tmp;
				inBuffer[count+1] = (tmp&0xffff);
				count++;
			}

			if (inBuffer[1] == 0xde || inBuffer[1] == 0xad) {
				ledGreen <: 0;
			} else {
				ledGreen <: 1;
			}

			if (inBuffer[1] == 0xaa || inBuffer[1] == 0x66) {
				ledGreen <: 0;
			} else {
				ledRed <: 1;
			}

			/* DEBUG output of received packet: */
			for (i=1;i<count+1;i++) {
				printhex(inBuffer[i]);
				printstr(";");
			}
			printstr(".\n");
			// */

			break;
#endif
#if 0
		case coe_out <: outBuffer[0] :
			count=1;
			break;

		case eoe_out <: outBuffer[0] :
			count=1;
			break;

		case foe_out <: outBuffer[0] :
			count=1;
			break;
#endif
		}

		/* send data */
		switch (outBuffer[0]) {
		case COE_PACKET:
			count=1;
			while (count<outSize) {
				coe_out <: outBuffer[count];
				count++;
			}
			break;

		case EOE_PACKET:
			count=1;
			while (count<outSize) {
				eoe_out <: outBuffer[count];
				count++;
			}
			break;

		case FOE_PACKET:
			count=1;
			while (count<outSize) {
				foe_out <: outBuffer[count];
				count++;
			}
			break;
		}

		t :> time;
		t when timerafter(time+delay) :> void;
	}
}

static void pdo_handler(chanend pdo_in, chanend pdo_out)
{
	unsigned int inBuffer[64];
	unsigned int count;
	unsigned int tmp;

	timer t;
	const unsigned int delay = 100000;
	unsigned int time = 0;

	while (1){
		select {
		case pdo_out :> tmp :
			inBuffer[0] = tmp&0xffff;
			//printstr("[APP] Received PDO packet: \n");

			count = 0;
			while (count<inBuffer[0]) {
				tmp = 0;
				pdo_out :> tmp;
				inBuffer[count+1] = (tmp&0xffff);
				count++;
			}

			if (inBuffer[1] == 0xdead || inBuffer[1] == 0xadde || inBuffer[1] == 0xbeef || inBuffer[1] == 0xefbe) {
				ledGreen <: 0;
			} else {
				ledGreen <: 1;
			}

			if (inBuffer[1] == 0xaaaa || inBuffer[1] == 0x6666) {
				ledRed <: 0;
			} else {
				ledRed <: 1;
			}
			break;
		}

		//t :> time;
		//t when timerafter(time+delay) :> void;
	}
}

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
	chan foe_in;   ///< File from module_ethercat to consumer
	chan foe_out;  ///< File from consumer to module_ethercat
	chan pdo_in;
	chan pdo_out;

	par {
		on stdcore[0] : {
			ecat_init();
			ecat_handler(coe_in, coe_out, eoe_in, eoe_out, foe_in, foe_out, pdo_in, pdo_out);
		}
	
		on stdcore[0] : {
			consumer(coe_in, coe_out, eoe_in, eoe_out, foe_in, foe_out/*, pdo_in, pdo_out*/);
		}

		/*
		on stdcore[1] : {
			led_handler();
		}
		*/

		on stdcore[1] : {
			pdo_handler(pdo_in, pdo_out);
		}
	}
	
	return 0;
}
