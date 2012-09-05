// Copyright (c) 2011, XMOS Ltd, All rights reserved
// This software is freely distributable under a derivative of the
// University of Illinois/NCSA Open Source License posted in
// LICENSE.txt and at <http://github.xcore.com/>

#include <platform.h>
#include <print.h>
#include "uip_server.h"
#include "xhttpd.h"
#include "getmac.h"
#include "ethernet_server.h"
#include "ethercat.h"


// IP Config - change this to suit your network.  Leave with all
// 0 values to use DHCP
xtcp_ipconfig_t ipconfig = {
		{ 192,168,0,2 }, // ip address (eg 192,168,0,2)
		{ 255,255,255,0 }, // netmask (eg 255,255,255,0)
		{ 192,168,0,1 } // gateway (eg 192,168,0,1)
};

void ethernet_getmac_dummy(char mac[])
{
	mac[0] = 0xde;
	mac[1] = 0xaf;
	mac[2] = 0xbe;
	mac[3] = 0xef;
	mac[4] = 0xde;
	mac[5] = 0xaf;
	mac[6] = 0xbe;
	mac[7] = 0xef;
}

// Program entry point
int main(void) {
	chan mac_rx[1], mac_tx[1], xtcp[1], connect_status;

	/* ethercat channels */
        chan coe_in;   ///< CAN from module_ethercat to consumer
        chan coe_out;  ///< CAN from consumer to module_ethercat
        chan eoe_in;   ///< Ethernet from module_ethercat to consumer
        chan eoe_out;  ///< Ethernet from consumer to module_ethercat
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
                        ecat_handler(coe_out, coe_in, eoe_out, eoe_in, foe_out, foe_in, pdo_out, pdo_in);
		}

		// The ethernet server
		on stdcore[1]:
		{
			int mac_address[2];

#if 0 /* FIXME otp data should be set somewhere else */
			ethernet_getmac_otp(otp_data, otp_addr, otp_ctrl,
					(mac_address, char[]));
#else
			ethernet_getmac_dummy((mac_address, char[]));
#endif


			ethernet_server(/*mii,*/ eoe_in, eoe_out,
					mac_address,
					mac_rx, 1, mac_tx, 1, null,
					connect_status);
		}

		// The TCP/IP server thread
		on stdcore[1]: uip_server(mac_rx[0], mac_tx[0],
				xtcp, 1, ipconfig,
				connect_status);

		// The webserver thread
		on stdcore[0]: xhttpd(xtcp[0]);

	}

	return 0;
}
