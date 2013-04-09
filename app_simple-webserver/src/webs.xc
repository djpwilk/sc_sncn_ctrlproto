// Copyright (c) 2011, XMOS Ltd, All rights reserved
// This software is freely distributable under a derivative of the
// University of Illinois/NCSA Open Source License posted in
// LICENSE.txt and at <http://github.xcore.com/>

#include <xs1.h>
#include <xclib.h>
#include <platform.h>
#include <print.h>
#include "xtcp.h"
#include "uip_server.h"
#include "xhttpd.h"
#include <otp_board_info.h>
#include "ethercat.h"


// IP Config - change this to suit your network.  Leave with all
// 0 values to use DHCP
xtcp_ipconfig_t ipconfig = {
		{ 192,168,0,23 }, // ip address (eg 192,168,0,2)
		{ 255,255,255,0 }, // netmask (eg 255,255,255,0)
		{ 192,168,0,1 } // gateway (eg 192,168,0,1)
};

static void ethernet_getmac_dummy(char mac[])
{
	mac[0] = 0x00;
	mac[1] = 0xaf;
	mac[2] = 0xbe;
	mac[3] = 0xef;
	mac[4] = 0xde;
	mac[5] = 0xaf;
}

#define MAX_BUFFER_SIZE   1024

/* example consumer */
static void consumer(chanend coe_in, chanend coe_out)
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
		case coe_in :> tmp :
			inBuffer[0] = tmp&0xffff;
			printstr("[APP] Received COE packet\n");
			count=0;

			while (count < inBuffer[0]) {
				coe_in :> tmp;
				inBuffer[count+1] = tmp&0xffff;
				count++;
			}

#if 0
			/* Reply with abort initiate download sequence */
			outType = COE_PACKET;
			outBuffer[0] = 5;
			outBuffer[1] = 0x2000;
			outBuffer[2] = 0x0080;
			outBuffer[3] = 0x001c;
			outBuffer[4] = 0x0000;
			outBuffer[5] = 0x0601;
#endif
			break;

		}

		/* send data */
		switch (outType /*outBuffer[0]*/) {
		case COE_PACKET:
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

		default:
			break;
		}

		t :> time;
		t when timerafter(time+delay) :> void;
	}
}

// Program entry point
int main(void) {
	chan mac_rx[1], mac_tx[1], xtcp[1], connect_status;

	/* ethercat channels */
        chan coe_in;   ///< CAN from module_ethercat to consumer
        chan coe_out;  ///< CAN from consumer to module_ethercat
        chan eoe_in;   ///< Ethernet from module_ethercat to consumer
        chan eoe_out;  ///< Ethernet from consumer to module_ethercat
	chan eoe_sig;  ///< Signals from EtherCAT to Ethernet MII
        chan foe_in;   ///< File from module_ethercat to consumer
        chan foe_out;  ///< File from consumer to module_ethercat
        chan pdo_in;
        chan pdo_out;

	par
	{
		/* the ethercat thread */
		on stdcore[0]:
		{
                        ecat_init();
                        ecat_handler(coe_out, coe_in, eoe_out, eoe_in, eoe_sig, foe_out, foe_in, pdo_out, pdo_in);
		}


		// The TCP/IP server thread
		on stdcore[1]:
		{
			char mac_address[6];
			ethernet_getmac_dummy(mac_address);
#if XTCP_SEPARATE_MAC
			ethernet_server(eoe_sig, eoe_in, eoe_out,
					mac_address, mac_rx, 1, mac_tx, 1);
			xtcp_server(mac_rx[0], mac_tx[0], xtcp, 1, ipconfig);
#else
#warning "using module_ethernet/lite"
			ethernet_xtcp_server(mac_address,
						eoe_sig, eoe_in, eoe_out,
						ipconfig,
						xtcp,
						1);
#endif
		}

		// The webserver thread
		on stdcore[1]: xhttpd(xtcp[0]);

		on stdcore[2]: consumer(coe_in, coe_out); /* Dummy consumer to catch up CoE init package */
	}

	return 0;
}
