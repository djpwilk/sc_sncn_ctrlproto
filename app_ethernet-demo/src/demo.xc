// Copyright (c) 2011, XMOS Ltd, All rights reserved
// This software is freely distributable under a derivative of the
// University of Illinois/NCSA Open Source License posted in
// LICENSE.txt and at <http://github.xcore.com/>

/*************************************************************************
 *
 * Ethernet MAC Layer Client Test Code
 * IEEE 802.3 MAC Client
 *
 *
 *************************************************************************
 *
 * ARP/ICMP demo
 * Note: Only supports unfragmented IP packets
 *
 *************************************************************************/

#include <xs1.h>
#include <xclib.h>
#include <print.h>
#include <platform.h>
#include <stdlib.h>
#include "otp_board_info.h"
#include "ethernet.h"
//#include "ethernet_board_support.h"
#include "checksum.h"
#include "xscope.h"
#include "ethercat.h"

// If you have a board with the xscope xlink enabled (e.g. the XC-2) then
// change this define to 0, make sure you also remove the -lxscope from
// the build flags in the Makefile
#define USE_XSCOPE 0


#if USE_XSCOPE
void xscope_user_init(void) {
  xscope_register(0);
  xscope_config_io(XSCOPE_IO_BASIC);
}
#endif

// Port Definitions

// These ports are for accessing the OTP memory - FIXME OTP isn't used currently (fj)
//on ETHERNET_DEFAULT_TILE: otp_ports_t otp_ports = OTP_PORTS_INITIALIZER;

// Here are the port definitions required by ethernet
// The intializers are taken from the ethernet_board_support.h header for
// XMOS dev boards. If you are using a different board you will need to
// supply explicit port structure intializers for these values
//smi_interface_t smi = ETHERNET_DEFAULT_SMI_INIT;
//mii_interface_t mii = ETHERNET_DEFAULT_MII_INIT;
//ethernet_reset_interface_t eth_rst = ETHERNET_DEFAULT_RESET_INTERFACE_INIT;

//::ip_address_define
// NOTE: YOU MAY NEED TO REDEFINE THIS TO AN IP ADDRESS THAT WORKS
// FOR YOUR NETWORK
#define OWN_IP_ADDRESS {192, 168, 0, 23}
//::


unsigned char ethertype_ip[] = {0x08, 0x00};
unsigned char ethertype_arp[] = {0x08, 0x06};

unsigned char own_mac_addr[6];

#define ARP_RESPONSE 1
#define ICMP_RESPONSE 2
#define UDP_RESPONSE 3

void demo(chanend tx, chanend rx);

static void ethernet_getmac_dummy(char mac[])
{
	mac[0] = 0x00;
	mac[1] = 0x50;
	mac[2] = 0xC2;
	mac[3] = 0xDE;
	mac[4] = 0xDE;
	mac[5] = 0xDE;
}

#pragma unsafe arrays
int is_ethertype(unsigned char data[], unsigned char type[]){
	int i = 12;
	return data[i] == type[0] && data[i + 1] == type[1];
}

#pragma unsafe arrays
int is_mac_addr(unsigned char data[], unsigned char addr[]){
	for (int i=0;i<6;i++){
          if (data[i] != addr[i]){
			return 0;
		}
	}

	return 1;
}

#pragma unsafe arrays
int is_broadcast(unsigned char data[]){
	for (int i=0;i<6;i++){
          if (data[i] != 0xFF){
			return 0;
		}
	}

	return 1;
}

//::custom-filter
int mac_custom_filter(unsigned int data[]){
	if (is_ethertype((data,char[]), ethertype_arp)){
		return 1;
	}else if (is_ethertype((data,char[]), ethertype_ip)){
		return 1;
	}

	return 0;
}
//::


int build_arp_response(unsigned char rxbuf[], unsigned int txbuf[], const unsigned char own_mac_addr[6])
{
  unsigned word;
  unsigned char byte;
  const unsigned char own_ip_addr[4] = OWN_IP_ADDRESS;

  for (int i = 0; i < 6; i++)
    {
      byte = rxbuf[22+i];
      (txbuf, unsigned char[])[i] = byte;
      (txbuf, unsigned char[])[32 + i] = byte;
    }
  word = (rxbuf, const unsigned[])[7];
  for (int i = 0; i < 4; i++)
    {
      (txbuf, unsigned char[])[38 + i] = word & 0xFF;
      word >>= 8;
    }

  (txbuf, unsigned char[])[28] = own_ip_addr[0];
  (txbuf, unsigned char[])[29] = own_ip_addr[1];
  (txbuf, unsigned char[])[30] = own_ip_addr[2];
  (txbuf, unsigned char[])[31] = own_ip_addr[3];

  for (int i = 0; i < 6; i++)
  {
    (txbuf, unsigned char[])[22 + i] = own_mac_addr[i];
    (txbuf, unsigned char[])[6 + i] = own_mac_addr[i];
  }
  txbuf[3] = 0x01000608;
  txbuf[4] = 0x04060008;
  (txbuf, unsigned char[])[20] = 0x00;
  (txbuf, unsigned char[])[21] = 0x02;

  // Typically 48 bytes (94 for IPv6)
  for (int i = 42; i < 64; i++)
  {
    (txbuf, unsigned char[])[i] = 0x00;
  }

  return 64;
}


int is_valid_arp_packet(const unsigned char rxbuf[], int nbytes)
{
  static const unsigned char own_ip_addr[4] = OWN_IP_ADDRESS;

  if (rxbuf[12] != 0x08 || rxbuf[13] != 0x06)
    return 0;

  //printstr("ARP packet received\n");

  if ((rxbuf, const unsigned[])[3] != 0x01000608)
  {
    printstr("Invalid et_htype\n");
    return 0;
  }
  if ((rxbuf, const unsigned[])[4] != 0x04060008)
  {
    printstr("Invalid ptype_hlen\n");
    return 0;
  }
  if (((rxbuf, const unsigned[])[5] & 0xFFFF) != 0x0100)
  {
    printstr("Not a request\n");
    return 0;
  }
  /* DEBUG * /
  printstr("received IP: ");
  printint(rxbuf[38+0]); printstr(".");
  printint(rxbuf[38+1]); printstr(".");
  printint(rxbuf[38+2]); printstr(".");
  printint(rxbuf[38+3]); printstr("\n");
  printstr("local IP: ");
  printint(own_ip_addr[0]); printstr(".");
  printint(own_ip_addr[1]); printstr(".");
  printint(own_ip_addr[2]); printstr(".");
  printint(own_ip_addr[3]); printstr("\n");
  // */
  for (int i = 0; i < 4; i++)
  {
    if (rxbuf[38 + i] != own_ip_addr[i])
    {
      //printstr("Not for us\n");
      return 0;
    }
  }

  return 1;
}


int build_icmp_response(unsigned char rxbuf[], unsigned char txbuf[], const unsigned char own_mac_addr[6])
{
  static const unsigned char own_ip_addr[4] = OWN_IP_ADDRESS;
  unsigned icmp_checksum;
  int datalen;
  int totallen;
  const int ttl = 0x40;
  int pad;

  // Precomputed empty IP header checksum (inverted, bytereversed and shifted right)
  unsigned ip_checksum = 0x0185;

  for (int i = 0; i < 6; i++)
    {
      txbuf[i] = rxbuf[6 + i];
    }
  for (int i = 0; i < 4; i++)
    {
      txbuf[30 + i] = rxbuf[26 + i];
    }

  // As far as I understood, the icmp_checksum has nothing to do with the received checksum (jes)
  //icmp_checksum = byterev((rxbuf, const unsigned[])[9]) >> 16;
  icmp_checksum = 0x0; /* start with all 0 */
  for (int i = 0; i < 4; i++)
    {
      txbuf[38 + i] = rxbuf[38 + i];
    }

    // FIXME datalen is too large by 2 bytes
  totallen = byterev((rxbuf, const unsigned[])[4]) >> 16;
  datalen = totallen - 28;
  for (int i = 0; i < datalen; i++)
    {
      txbuf[42 + i] = rxbuf[42+i];
    }

  for (int i = 0; i < 6; i++)
  {
    txbuf[6 + i] = own_mac_addr[i];
  }
  (txbuf, unsigned[])[3] = 0x00450008;
  totallen = byterev(28 + datalen) >> 16;
  (txbuf, unsigned[])[4] = totallen;
  ip_checksum += totallen;
  (txbuf, unsigned[])[5] = 0x01000000 | (ttl << 16);
  (txbuf, unsigned[])[6] = 0;
  for (int i = 0; i < 4; i++)
  {
    txbuf[26 + i] = own_ip_addr[i];
  }
  ip_checksum += (own_ip_addr[0] | own_ip_addr[1] << 8);
  ip_checksum += (own_ip_addr[2] | own_ip_addr[3] << 8);
  ip_checksum += txbuf[30] | (txbuf[31] << 8);
  ip_checksum += txbuf[32] | (txbuf[33] << 8);

  txbuf[34] = 0x00;
  txbuf[35] = 0x00;

// jes: calculate the checksum:
  // jes: first set the checksum to 0 in the header:
  icmp_checksum = 0;
  txbuf[36] = icmp_checksum >> 8;
  txbuf[37] = icmp_checksum & 0xFF;

  // sum up the ICMP header
  icmp_checksum += (unsigned)txbuf[34] | ((unsigned)txbuf[35]<<8); // should make 16-bit calculation
  icmp_checksum += (unsigned)txbuf[36] | ((unsigned)txbuf[37]<<8); // should make 16-bit calculation
  icmp_checksum += (unsigned)txbuf[38] | ((unsigned)txbuf[39]<<8); // should make 16-bit calculation
  icmp_checksum += (unsigned)txbuf[40] | ((unsigned)txbuf[41]<<8); // should make 16-bit calculation

// FIXME add checkk for odd number of bytes
  // sum up the package content
  for (int j=0; j<(datalen-2); j+=2) {
  	icmp_checksum += (unsigned)txbuf[42+j] | ((unsigned)txbuf[42+j+1]<<8);
  }

  icmp_checksum = (icmp_checksum&0xffff) + (icmp_checksum>>16); // add carry bits
  icmp_checksum = ~icmp_checksum; // one's complement

  // set the freshly calculated checksum in the header (byte swap):
  txbuf[37] = icmp_checksum >> 8;
  txbuf[36] = icmp_checksum & 0xFF;

  while (ip_checksum >> 16)
  {
    ip_checksum = (ip_checksum & 0xFFFF) + (ip_checksum >> 16);
  }
  ip_checksum = byterev(~ip_checksum) >> 16;
  txbuf[24] = ip_checksum >> 8;
  txbuf[25] = ip_checksum & 0xFF;

  for (pad = 42 + datalen; pad < 64; pad++)
  {
    txbuf[pad] = 0x00;
  }
  return pad;
}


int is_valid_icmp_packet(const unsigned char rxbuf[], int nbytes)
{
  static const unsigned char own_ip_addr[4] = OWN_IP_ADDRESS;
  unsigned totallen;

  if (rxbuf[12] != 0x08 || rxbuf[13] != 0x00) /* ICMP is a IP packet, so first check this */
    return 0;

  if (rxbuf[23] != 0x01)
    return 0;

  printstr("ICMP packet received\n");

  if ((rxbuf, const unsigned[])[3] != 0x00450008)
  {
    printstr("Invalid et_ver_hdrl_tos\n");
    return 0;
  }
  if (((rxbuf, const unsigned[])[8] >> 16) != 0x0008)
  {
    printstr("Invalid type_code\n");
    return 0;
  }
  for (int i = 0; i < 4; i++)
  {
    if (rxbuf[30 + i] != own_ip_addr[i])
    {
      printstr("Not for us\n");
      return 0;
    }
  }

  totallen = byterev((rxbuf, const unsigned[])[4]) >> 16;
  if (nbytes > 60 && nbytes != totallen + 12/*14*/)
  {
    printstr("Invalid size\n");
    printintln(nbytes);
    printintln(totallen+12/*14*/);
    return 0;
  }
  if (checksum_ip(rxbuf) != 0)
  {
    printstr("Bad checksum\n");
    return 0;
  }

  return 1;
}


void demo(chanend tx, chanend rx)
{
	unsigned int rxbuf[1600/4];
	unsigned int txbuf[1600/4];
	
	//::get-macaddr
	//mac_get_macaddr(tx, own_mac_addr);
	ethernet_getmac_dummy(own_mac_addr);
	//::

	//::setup-filter
#ifdef CONFIG_FULL
	mac_set_custom_filter(rx, 0x1);
#endif
	//::
	printstr("Test started\n");

	//::mainloop
	while (1)
	{
		unsigned int src_port;
		unsigned int nbytes;
		mac_rx(rx, (rxbuf,char[]), nbytes, src_port);
		//printstr("[DEBUG demo()] mac_rx() returned.\n");
#ifdef CFIG_LITE
		if (!is_broadcast((rxbuf,char[])) && !is_mac_addr((rxbuf,char[]), own_mac_addr))
			continue;
		if (mac_custom_filter(rxbuf) != 0x1)
			continue;
#endif


		//::arp_packet_check
		if (is_valid_arp_packet((rxbuf,char[]), nbytes)) {
			build_arp_response((rxbuf,char[]), txbuf, own_mac_addr);
			mac_tx(tx, txbuf, nbytes, ETH_BROADCAST);
			printstr("ARP response sent\n");
		}
		//::icmp_packet_check
		else if (is_valid_icmp_packet((rxbuf,char[]), nbytes))
		{
			build_icmp_response((rxbuf,char[]), (txbuf, unsigned char[]), own_mac_addr);
			mac_tx(tx, txbuf, nbytes, ETH_BROADCAST);
			printstr("ICMP response sent\n");
		}
		//::
	}
}

#define MAX_BUFFER_SIZE   1024

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

int main()
{
	chan rx[1], tx[1];

	chan coe_in;   ///< CAN from module_ethercat to consumer
	chan coe_out;  ///< CAN from consumer to module_ethercat
	chan eoe_in;   ///< Ethernet from module_ethercat to consumer
	chan eoe_out;  ///< Ethernet from consumer to module_ethercat
	chan eoe_sig;  ///< Signals from EtherCAT to Ethernet MII
	chan foe_in;   ///< File from module_ethercat to consumer
	chan foe_out;  ///< File from consumer to module_ethercat
	chan pdo_in;
	chan pdo_out;

	/* FIXME this function is pretty obfuscatet atm. Until I understand the function the dummy here is used. * /
	own_mac_addr[0] = 0x00;
	own_mac_addr[1] = 0x50;
	own_mac_addr[2] = 0xC2;
	own_mac_addr[3] = 0xDE;
	own_mac_addr[4] = 0xDE;
	own_mac_addr[5] = 0xDE;
	// */

	par
	{
		//::ethercat
		on stdcore[0]:
		{
			ecat_init();
			ecat_handler(coe_out, coe_in, eoe_out, eoe_in, eoe_sig, foe_out, foe_in, pdo_out, pdo_in);
		}
		//::

		//::ethernet
		on stdcore[1]:
		{
			char mac_address[6];
			//otp_board_info_get_mac(otp_ports, 0, mac_address);
			ethernet_getmac_dummy(mac_address);
			//eth_phy_reset(eth_rst);
			//smi_init(smi);
			//eth_phy_config(1, smi);
			ethernet_server(eoe_sig, eoe_in, eoe_out,
					mac_address,
					rx, 1,
					tx, 1);
		}
		//::

		//::demo
		on stdcore[1] : demo(tx[0], rx[0]);
		//::

		//::dummyconsumer
		on stdcore[2]: consumer(coe_in, coe_out); /* Dummy consumer to catch up CoE init package */
		//::
	}

	return 0;
}
