/** \file
 * \brief Example code for Simple Open EtherCAT master
 *
 * Usage : voe [ifname1]
 * ifname is NIC interface, f.e. eth0
 *
 * This is a minimal test to send VoE mailbox packages.
 *
 * (c)Arthur Ketels 2010 - 2011 (original simpletest)
 * (c) 2011 Frank Jeschke <jeschke@fjes.de>
 */

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdint.h>

#include "ethercattype.h"
#include "nicdrv.h"
#include "ethercatbase.h"
#include "ethercatmain.h"
#include "ethercatcoe.h"
#include "ethercatfoe.h"
#include "ethercatconfig.h"
#include "ethercatprint.h"

#define MBX_VOE_TYPE  0xF

#define MAX_MBXDATALENGTH  0x200 /* this size is copied from ethercatcoe.c */

typedef struct __attribute__((__packed__)) {
	ec_mbxheadert header;
	uint8_t data[MAX_MBXDATALENGTH]; /* max length of data foo */
} ec_VOEmbx_t;

char IOmap[4096];

int send_mbx(uint16_t slave, uint8_t *data, size_t length)
{
	//ec_mbxbuft mbxIn;
	ec_mbxbuft mbxOut;

	//ec_clearmbx(&mbxIn);
	ec_clearmbx(&mbxOut);

	//ec_VOEmbx_t *voeMbxIn = (ec_VOEmbx_t *)mbxIn;
	ec_VOEmbx_t *voeMbxOut = (ec_VOEmbx_t *)mbxOut;

	voeMbxOut->header.length = htoes(0x000a);
	voeMbxOut->header.address = 0x0000;
	voeMbxOut->header.priority = 0x00;
	voeMbxOut->header.mbxtype = ECT_MBXT_VOE; /* + (cnt<<4) */

	memset(voeMbxOut->data, 0, MAX_MBXDATALENGTH);
	memmove(voeMbxOut->data, data, length);

	int ret = ec_mbxsend(slave, &mbxOut, EC_TIMEOUTTXM);

	return ret;
}

int slave_operation(int oloop, int iloop, boolean needlf)
{
	int i;
	int j, wkc;
	int ret = 0;
	int slaveid, value, ctr, idx;

	//uint8_t db1[] = { 0xde, 0xad, 0xbe, 0xef }; // test data
	//uint8_t db2[] = { 0xAA, 0xAA, 0x66, 0x66 }; // test data

	int cycleCount = 0;

	/* load testdata to outputs
	for (i=0; i<oloop && i<4; i++) {
		*(ec_slave[1].outputs + i) = db[i];
	}
	// */

	printf("Operational state reached for all slaves.\n");

	/* check mailbox settings of device: */
	for (i=1; i<=ec_slavecount; i++) {
		if (ec_slave[i].state == EC_STATE_OPERATIONAL) {
			printf("[%s] ec_slave[%d].mbx_l = %d\n", __func__, i, ec_slave[i].mbx_l);
		}
	}
	/* end check */


	for (i=0, cycleCount=1; i<10000; i++, cycleCount++) {
		printf("Processdata cycle (%d):", cycleCount);
		printf("> ");
		scanf("%d:%d", &slaveid, &value);
		//printf("\t%d -> %x\n", slaveid, value);

		for (ctr=0; ctr<ec_slavecount; ctr++) {
		    idx = (slaveid >> ctr) & 0x1;
		    if (idx != 0) {
			idx = ctr + 1;
			printf("\t%d -> 0x%x\n", idx, value);
			*(ec_slave[idx].outputs + 0) = (value >> 16) & 0xff;
			*(ec_slave[idx].outputs + 1) = (value >> 24) & 0xff;
			*(ec_slave[idx].outputs + 2) = (value >> 0) & 0xff;
			*(ec_slave[idx].outputs + 3) = (value >> 8) & 0xff;
		    }
		}
/*
		for (j=0; j<oloop && j<4; j++) {
			if (cycleCount%2 == 0) {
				*(ec_slave[1].outputs + j) = db1[j];
			} else {
				*(ec_slave[1].outputs + j) = db2[j];
			}
		}
*/

//		printf("O:");
		for (j = 0; j < oloop; j++) {
//			printf(" %2.2x", *(ec_slave[1].outputs + j));
		}

//		printf("\nI:");
		for (j = 0; j < iloop; j++) {
//			printf(" %2.2x", *(ec_slave[1].inputs + j));
		}
//		printf("\n\r");
		needlf = TRUE;

//		usleep(10000);

		ret = ec_send_processdata();
		wkc = ec_receive_processdata(EC_TIMEOUTRET*4);
		if ((wkc < ec_group[0].expectedWKC) || ec_group[0].docheckstate) {
			printf("wkc not reached\n");
		}

//		printf("O:");
		for (j = 0; j < oloop; j++) {
//			printf(" %2.2x", *(ec_slave[0].outputs + j));
		}

//		printf("\nI:");
		for (j = 0; j < iloop; j++) {
//			printf(" %2.2x", *(ec_slave[0].inputs + j));
		}
//		printf("\n\r");

		if (ret == 0) {
			fprintf(stderr, " error sending processdata\n");
			return -1;
		}
		//usleep(300000);
	}

	return 0;

#if 0
	for (i = 1; i <= 10000; i++) {
		ec_send_processdata();
		wkc = ec_receive_processdata(EC_TIMEOUTRET);

		if ((wkc < ec_group[0].expectedWKC) || ec_group[0].docheckstate) {
			if (needlf) {
				needlf = FALSE;
				printf("\n");
			}

			/* one ore more slaves are not responding */
			ec_group[0].docheckstate = FALSE;
			ec_readstate();

			for (slave = 1; slave <= ec_slavecount; slave++) {
				if (ec_slave[slave].state != EC_STATE_OPERATIONAL) {
					ec_group[0].docheckstate = TRUE;

					if (ec_slave[slave].state == (EC_STATE_SAFE_OP + EC_STATE_ERROR)) {
						printf("ERROR : slave %d is in SAFE_OP + ERROR, attempting ack.\n", slave);
						ec_slave[slave].state = (EC_STATE_SAFE_OP + EC_STATE_ACK);
						ec_writestate(slave);
					} else if (ec_slave[slave].state == EC_STATE_SAFE_OP) {
						printf ("WARNING : slave %d is in SAFE_OP, change to OPERATIONAL.\n", slave);
						ec_slave[slave].state = EC_STATE_OPERATIONAL;
						ec_writestate(slave);
					} else if (ec_slave[slave].state > 0) {
						if (ec_reconfig_slave(slave)) {
							ec_slave[slave].islost = FALSE;
							printf ("MESSAGE : slave %d reconfigured\n", slave);
						}
					} else if (!ec_slave[slave].islost) {
						ec_slave[slave].islost = TRUE;
						printf("ERROR : slave %d lost\n", slave);
					}
				}

				if (ec_slave[slave].islost) {
					if (!ec_slave[slave].state) {
						if (ec_recover_slave(slave)) {
							ec_slave[slave].islost = FALSE;
							printf("MESSAGE : slave %d recovered\n", slave);
						}
					} else {
						ec_slave[slave].islost = FALSE;
						printf("MESSAGE : slave %d found\n", slave);
					}
				}
			}

			if (!ec_group[0].docheckstate)
				printf("OK : all slaves resumed OPERATIONAL.\n");
		} else {
			printf("Processdata cycle %4d, WKC %d , O:", i, wkc);

			for (j = 0; j < oloop; j++) {
				printf(" %2.2x", *(ec_slave[0].outputs + j));
			}

			printf(" I:");

			for (j = 0; j < iloop; j++) {
				printf(" %2.2x", *(ec_slave[0].inputs + j));
			}
			printf("\r");
			needlf = TRUE;
		}
		usleep(10000);
	}
#endif

	return ret;
}

void simpletest(char *ifname)
{
	int i, oloop, iloop, err; // j, wkc, wkc_count, slave,
	boolean needlf;

	needlf = FALSE;
	printf("Starting process data  test\n");

	/* initialise SOEM, bind socket to ifname */
	if (ec_init(ifname)) {
		printf("ec_init on %s succeeded.\n", ifname);

		/* find and auto-config slaves */
		if (ec_config_init(FALSE) > 0) {
			printf("%d slaves found and configured.\n", ec_slavecount);

			printf("Checking device configuration from EEPROM:\n");
			printf(" Man = %x, ID = %x, Name %s, dtype=%d, Ibits= %d, Obits= %d\n",
					ec_slave[0].eep_man,
					ec_slave[0].eep_id,
					ec_slave[0].name,
					ec_slave[0].Dtype,
					ec_slave[0].Ibits,
					ec_slave[0].Obits);

			ec_config_map(&IOmap);

			printf("Slaves mapped, state to SAFE_OP.\n");
			/* wait for all slaves to reach SAFE_OP state */
			ec_statecheck(0, EC_STATE_SAFE_OP, EC_TIMEOUTSTATE * 4);

			oloop = ec_slave[0].Obytes;
			if ((oloop == 0) && (ec_slave[0].Obits > 0))
				oloop = 1;
			if (oloop > 8)
				oloop = 8;

			iloop = ec_slave[0].Ibytes;
			if ((iloop == 0) && (ec_slave[0].Ibits > 0))
				iloop = 1;
			if (iloop > 8)
				iloop = 8;

			printf("segments : %d : %d %d %d %d\n",
			       ec_group[0].nsegments, ec_group[0].IOsegment[0],
			       ec_group[0].IOsegment[1],
			       ec_group[0].IOsegment[2],
			       ec_group[0].IOsegment[3]);

			printf("Request operational state for all slaves\n");
			printf("Calculated workcounter %d\n",
					ec_group[0].expectedWKC);

			/* Preload test
			uint8_t db[] = { 0xde, 0xad, 0xbe, 0xef }; // test data

			// load testdata to outputs
			for (i=0; i<oloop && i<4; i++) {
				*(ec_slave[1].outputs + i) = db[i];
			}
			*/

			ec_slave[0].state = EC_STATE_OPERATIONAL;
			/* send one valid process data to make outputs in slaves happy */
			ec_send_processdata();
			ec_receive_processdata(EC_TIMEOUTRET*4);
			/* request OP state for all slaves */
			ec_writestate(0);
			/* wait for all slaves to reach OP state */
			ec_statecheck(0, EC_STATE_OPERATIONAL, EC_TIMEOUTSTATE * 4);

			if (ec_slave[0].state == EC_STATE_OPERATIONAL) {
				err = slave_operation(oloop, iloop, needlf);
				if (err) {
					printf("slave_operation() returned error: %d\n", err);
				}
			} else {
				printf("Not all slaves reached operational state.\n");
				ec_readstate();
				for (i = 1; i <= ec_slavecount; i++) {
					if (ec_slave[i].state != EC_STATE_OPERATIONAL) {
						printf ("Slave %d State=0x%2.2x StatusCode=0x%4.4x : %s\n",
								i,
								ec_slave[i].state,
								ec_slave[i].ALstatuscode,
								ec_ALstatuscode2string(ec_slave[i].ALstatuscode));
					}
				}
			}

			printf("\nRequest init state for all slaves\n");
			/* request INIT state for all slaves */
			ec_slave[0].state = EC_STATE_INIT;
			ec_writestate(0);
			ec_statecheck(0, EC_STATE_INIT, EC_TIMEOUTSTATE*4);
			if (ec_slave[0].state == EC_STATE_INIT) {
				printf("slave reached init state\n");
			} else {
				printf("slave doesn't respond to init state, current state: %d\n",
						ec_slave[0].state);
			}
		} else {
			printf("No slaves found!\n");
		}

		/* stop SOEM, close socket */
		printf("End simple test, close socket\n");
		ec_close();
	} else {
		printf("No socket connection on %s\nExcecute as root\n", ifname);
	}
}

int main(int argc, char *argv[])
{
	printf("SOEM (Simple Open EtherCAT Master)\nSimple test\n");

	if (argc > 1) {
		/* start cyclic part */
		simpletest(argv[1]);
	} else {
		printf
		    ("Usage: simple_test ifname1\nifname = eth0 for example\n");
	}

	printf("End program\n");
	return (0);
}
