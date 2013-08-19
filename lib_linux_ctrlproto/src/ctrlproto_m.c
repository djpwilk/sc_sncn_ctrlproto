/*****************************************************************************
 *
 *  $Id$
 *
 *  Copyright (C) 2007-2009  Florian Pose, Ingenieurgemeinschaft IgH
 *
 *  This file is part of the IgH EtherCAT Master.
 *
 *  The IgH EtherCAT Master is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License version 2, as
 *  published by the Free Software Foundation.
 *
 *  The IgH EtherCAT Master is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 *  Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with the IgH EtherCAT Master; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *  ---
 *
 *  The license mentioned above concerns the source code only. Using the
 *  EtherCAT technology and brand is only permitted in compliance with the
 *  industrial property and similar rights of Beckhoff Automation GmbH.
 *
 ****************************************************************************/
 /*
  * Adaption to somanet by Frank Jeschke <jeschke@fjes.de>
  *
  * for Synapticon GmbH
  */

/*
 *       Copyright (c) 2013, Synapticon GmbH
 *       All rights reserved.
 *
 *       Redistribution and use in source and binary forms, with or without
 *       modification, are permitted provided that the following conditions are met:
 *
 *       1. Redistributions of source code must retain the above copyright notice, this
 *          list of conditions and the following disclaimer.
 *       2. Redistributions in binary form must reproduce the above copyright notice,
 *          this list of conditions and the following disclaimer in the documentation
 *          and/or other materials provided with the distribution.
 *       3. Execution of this software or parts of it exclusively takes place on hardware
 *          produced by Synapticon GmbH.
 *
 *       THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *       ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *       WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *       DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 *       ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *       (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *       LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *       ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *       (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *       SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *       The views and conclusions contained in the software and documentation are those
 *       of the authors and should not be interpreted as representing official policies,
 *       either expressed or implied, of the Synapticon GmbH.
 */

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdarg.h>

/****************************************************************************/

#include "ecrt.h"

/****************************************************************************/

#define MAJOR 1
#define MINOR 0

#define MAXDBGLVL  3

// Application parameters
#define FREQUENCY 1000
#define PRIORITY 1

// Optional features
#define CONFIGURE_PDOS  1
#define SDO_ACCESS      1
#define CIA402          1

/****************************************************************************/

/* application global definitions */
static int g_dbglvl = 1;

// EtherCAT
static ec_master_t *master = NULL;
static ec_master_state_t master_state/* = {}*/;

static ec_domain_t *domain1 = NULL;
static ec_domain_state_t domain1_state/* = {}*/;

static ec_slave_config_t *sc_data_in = NULL;
static ec_slave_config_state_t sc_data_in_state;
#if 0
static ec_slave_config_t *sc_ana_in = NULL;
static ec_slave_config_state_t sc_ana_in_state/* = {}*/;
#endif

// Timer
static unsigned int sig_alarms = 0;
static unsigned int user_alarms = 0;

/****************************************************************************/

// process data pointer
static uint8_t *domain1_pd = NULL;

#define SomanetPos     0, 0
#define SOMANET_ID     0x000022d2, 0x00000201



// offsets for PDO entries
#ifdef CIA402
static unsigned int off_pdo1_in; /* status word 8 bit */
static unsigned int off_pdo2_in; /* op modes display 8 bit */
static unsigned int off_pdo3_in; /* position value 32 bit */
static unsigned int off_pdo4_in; /* velocity value 32 bit */
static unsigned int off_pdo5_in; /* torque value 16 bit */

static unsigned int off_pdo1_out; /* control word 8 bit */
static unsigned int off_pdo2_out; /* op modes 8 bit */
static unsigned int off_pdo3_out; /* target torque 16 bit */
static unsigned int off_pdo4_out; /* target position 32 bit */
static unsigned int off_pdo5_out; /* target velocity 32 bit */
#endif

/*
static unsigned int off_ana_in_status;
static unsigned int off_ana_in_value;
static unsigned int off_ana_out;
static unsigned int off_dig_out;
 */

#ifdef CIA402
#define CAN_OD_CONTROL_WORD       0x6040 /* RX; 8 bit */
#define CAN_OD_STATUS_WORD        0x6041 /* TX; 8 bit */
#define CAN_OD_MODES              0x6060 /* RX; 8 bit */
#define CAN_OD_MODES_DISP         0x6061 /* TX; 8 bit */

#define CAN_OD_POS_VALUE          0x6064 /* TX; 32 bit */
#define CAN_OD_POS_TARGET         0x607A /* RX; 32 bit */
#define CAN_OD_VEL_VALUE          0x606C /* TX; 32 bit */
#define CAN_OD_VEL_TARGET         0x60ff /* RX; 32 bit */
#define CAN_OD_TOR_VALUE          0x6077 /* TX; 16 bit */
#define CAN_OD_TOR_TARGET         0x6071 /* RX; 16 bit */

const static ec_pdo_entry_reg_t domain1_regs[] = {
	/* RX */
	{SomanetPos, SOMANET_ID, CAN_OD_CONTROL_WORD, 0, &off_pdo1_out},
	{SomanetPos, SOMANET_ID, CAN_OD_MODES, 0, &off_pdo2_out},
	{SomanetPos, SOMANET_ID, CAN_OD_TOR_TARGET, 0, &off_pdo3_out},
	{SomanetPos, SOMANET_ID, CAN_OD_POS_TARGET, 0, &off_pdo4_out},
	{SomanetPos, SOMANET_ID, CAN_OD_VEL_TARGET, 0, &off_pdo5_out},
	/* TX */
	{SomanetPos, SOMANET_ID, CAN_OD_STATUS_WORD, 0, &off_pdo1_in},
	{SomanetPos, SOMANET_ID, CAN_OD_MODES_DISP, 0, &off_pdo2_in},
	{SomanetPos, SOMANET_ID, CAN_OD_POS_VALUE, 0, &off_pdo3_in},
	{SomanetPos, SOMANET_ID, CAN_OD_VEL_VALUE, 0, &off_pdo4_in},
	{SomanetPos, SOMANET_ID, CAN_OD_TOR_VALUE, 0, &off_pdo5_in},
    {0}
};
#endif

static unsigned int counter = 0;
static unsigned int blink = 0;

/*****************************************************************************/

#if CONFIGURE_PDOS

#ifdef CIA402
/* Master 0, Slave 0, "Synapticon-ECAT"
 * Vendor ID:       0x000022d2
 * Product code:    0x00000201
 * Revision number: 0x0a000002
 */

ec_pdo_entry_info_t slave_0_pdo_entries[] = {
    {0x6040, 0x00, 16}, /* control word */
    {0x6060, 0x00, 8}, /* operating modes */
    {0x6071, 0x00, 16}, /* torque target */
    {0x607a, 0x00, 32}, /* position target */
    {0x60ff, 0x00, 32}, /* velocity target */
    {0x6041, 0x00, 16}, /* status */
    {0x6061, 0x00, 8}, /* modes display */
    {0x6064, 0x00, 32}, /* position value */
    {0x606c, 0x00, 32}, /* velocity value */
    {0x6077, 0x00, 16}, /* torque value */
};

ec_pdo_info_t slave_0_pdos[] = {
    {0x1600, 5, slave_0_pdo_entries + 0}, /* Rx PDO Mapping */
    {0x1a00, 5, slave_0_pdo_entries + 5}, /* Tx PDO Mapping */
};

ec_sync_info_t slave_0_syncs[] = {
    {0, EC_DIR_OUTPUT, 0, NULL, EC_WD_DISABLE},
    {1, EC_DIR_INPUT, 0, NULL, EC_WD_DISABLE},
    {2, EC_DIR_OUTPUT, 1, slave_0_pdos + 0, EC_WD_DISABLE},
    {3, EC_DIR_INPUT, 1, slave_0_pdos + 1, EC_WD_DISABLE},
    {0xff}
};

#endif


#endif

static void logmsg(int lvl, const char *format, ...);

/*****************************************************************************/

#if SDO_ACCESS
static ec_sdo_request_t *sdo;

/* additional sdo requests */
static ec_sdo_request_t *request[3];

static ec_sdo_request_t *sdo_download_requests[1]; // one for each object
static unsigned sdoexample;
#endif

/*****************************************************************************/

void check_domain1_state(void)
{
    ec_domain_state_t ds;

    ecrt_domain_state(domain1, &ds);

    if (ds.working_counter != domain1_state.working_counter)
      //  logmsg(1, "Domain1: WC %u.\n", ds.working_counter);
    if (ds.wc_state != domain1_state.wc_state)
    //	logmsg(1, "Domain1: State %u.\n", ds.wc_state);

    domain1_state = ds;
}

/*****************************************************************************/

void check_master_state(void)
{
    ec_master_state_t ms;

    ecrt_master_state(master, &ms);

    if (ms.slaves_responding != master_state.slaves_responding)
        logmsg(1, "%u slave(s).\n", ms.slaves_responding);
    if (ms.al_states != master_state.al_states)
        logmsg(1, "AL states: 0x%02X.\n", ms.al_states);
    if (ms.link_up != master_state.link_up)
        logmsg(1, "Link is %s.\n", ms.link_up ? "up" : "down");

    master_state = ms;
}

/*****************************************************************************/

void check_slave_config_states(void)
{
    ec_slave_config_state_t s;

    ecrt_slave_config_state(sc_data_in, &s);

    if (s.al_state != sc_data_in_state.al_state)
        logmsg(1, "AnaIn: State 0x%02X.\n", s.al_state);
    if (s.online != sc_data_in_state.online)
        logmsg(1, "AnaIn: %s.\n", s.online ? "online" : "offline");
    if (s.operational != sc_data_in_state.operational)
        logmsg(1, "AnaIn: %soperational.\n",
                s.operational ? "" : "Not ");

    sc_data_in_state = s;
}

/*****************************************************************************/

#if SDO_ACCESS
void read_sdo(ec_sdo_request_t *req)
{
    switch (ecrt_sdo_request_state(req)) {
        case EC_REQUEST_UNUSED: // request was not used yet
            ecrt_sdo_request_read(req); // trigger first read
            break;
        case EC_REQUEST_BUSY:
            fprintf(stderr, "SDO still busy...\n");
            break;
        case EC_REQUEST_SUCCESS:
            logmsg(1, "SDO value read: 0x%X\n",
                    EC_READ_U32(ecrt_sdo_request_data(req)));
            ecrt_sdo_request_read(req); // trigger next read
            break;
        case EC_REQUEST_ERROR:
            fprintf(stderr, "Failed to read SDO!\n");
            ecrt_sdo_request_read(req); // retry reading
            break;
    }
}

void write_sdo(ec_sdo_request_t *req, unsigned data)
{
	EC_WRITE_U32(ecrt_sdo_request_data(req), data&0xffffffff);

	switch (ecrt_sdo_request_state(req)) {
		case EC_REQUEST_UNUSED: // request was not used yet
			ecrt_sdo_request_write(req); // trigger first read
			break;
		case EC_REQUEST_BUSY:
			fprintf(stderr, "SDO write still busy...\n");
			break;
		case EC_REQUEST_SUCCESS:
			logmsg(1, "SDO value written: 0x%X\n", data);
			ecrt_sdo_request_write(req); // trigger next read ???
			break;
		case EC_REQUEST_ERROR:
			fprintf(stderr, "Failed to write SDO!\n");
			ecrt_sdo_request_write(req); // retry writing
			break;
	}
}
#endif

/****************************************************************************/

void cyclic_task(void)
{
	if(sig_alarms == user_alarms) pause();
	while (sig_alarms != user_alarms) {
		/* sync the dc clock of the slaves */
	//	ecrt_master_sync_slave_clocks(master);

		// receive process data
		ecrt_master_receive(master);
		ecrt_domain_process(domain1);

		// check process data state (optional)
		check_domain1_state();

	//	if (counter) {
	//		counter--;
	//	} else { // do this at 1 Hz
	//		counter = FREQUENCY;
	//
	//		// calculate new process data
	//		blink = !blink;
	//
	//		// check for master state (optional)
			check_master_state();

			// check for islave configuration state(s) (optional)
			check_slave_config_states();

	#if SDO_ACCESS
	//		// read process data SDO
	//		read_sdo(sdo);
	//		read_sdo(request[0]);
	//		read_sdo(request[1]);
	//		read_sdo(request[2]);
	//
	//		write_sdo(sdo_download_requests[0], sdoexample); /* SDO download value to the node */
	#endif
		//}

		/* Read process data */
		unsigned int sn_status = EC_READ_U16(domain1_pd + off_pdo1_in);
		unsigned int sn_modes = EC_READ_U8(domain1_pd + off_pdo2_in);
		unsigned int sn_position = EC_READ_U32(domain1_pd + off_pdo3_in);
		unsigned int sn_velocity = EC_READ_U32(domain1_pd + off_pdo4_in);
		unsigned int sn_torque = EC_READ_U16(domain1_pd + off_pdo5_in);

	//	printf("\n%x", sn_status );
	//	printf("\n%x",  sn_modes);
	//	printf("\n%x",  sn_position);
	//	printf("\n%x",  sn_velocity);
	//	printf("\n%x",  sn_torque);




	#ifdef CIA402
	#define STATUSW1   0x88AA
	#define OPMODES1   0xf1
	#define TORVAL1    0xcbab
	#define POSVAL1    0xe4e4e2e2
	#define VELVAL1    0x2d2d4d4d

	#define STATUSW2   0xAA88
	#define OPMODES2   0x1f
	#define TORVAL2    0xcaba
	#define POSVAL2    0x2e2e4e4e
	#define VELVAL2    0xd4d4d2d2

		EC_WRITE_U16(domain1_pd + off_pdo1_out, (blink ? STATUSW1 : STATUSW2)&0xffff);
		EC_WRITE_U8(domain1_pd + off_pdo2_out, (blink ? OPMODES1 : OPMODES2)&0xff);
		EC_WRITE_U16(domain1_pd + off_pdo3_out, (blink ? TORVAL1 : TORVAL2)&0xffff);
		EC_WRITE_U32(domain1_pd + off_pdo4_out, blink ? POSVAL1 : POSVAL2);
		EC_WRITE_U32(domain1_pd + off_pdo5_out, blink ? VELVAL1 : VELVAL2);

	#endif


		// send process data
		ecrt_domain_queue(domain1);
		ecrt_master_send(master);
		//printf("Wrote %x to slave\n",  blink ? TESTWORD1 : TESTWORD2);
		  user_alarms++;
	}
}

/****************************************************************************/

void signal_handler(int signum) {
    switch (signum) {
        case SIGALRM:
            sig_alarms++;
            break;
    }
}

/****************************************************************************/

static void logmsg(int lvl, const char *format, ...)
{
	if (lvl > g_dbglvl)
		return;

	va_list ap;
	va_start(ap, format);
	vprintf(format, ap);
	va_end(ap);
}

static inline const char *_basename(const char *prog)
{
	const char *p = prog;
	const char *i = p;
	for (i = p; *i != '\0'; i++) {
		if (*i == '/')
			p = i+1;
	}

	return p;
}

static void printversion(const char *prog)
{
	printf("%s v%d.%d\n", _basename(prog), MAJOR, MINOR);
}

static void printhelp(const char *prog)
{
	printf("Usage: %s [-h] [-v] [-l <level>]\n", _basename(prog));
	printf("\n");
	printf("  -h           print this help and exit\n");
	printf("  -v           print version and exit\n");
	printf("  -l <level>   set log level (0..3)\n");
}

static void cmdline(int argc, char **argv)
{
	int flags, opt;
	int nsecs, tfnd;

	const char *options = "hvl:";

	nsecs = 0;
	tfnd = 0;
	flags = 0;

	while ((opt = getopt(argc, argv, options)) != -1) {
		switch (opt) {
		case 'v':
			printversion(argv[0]);
			exit(0);
			break;

		case 'l':
			g_dbglvl = atoi(optarg);
			if (g_dbglvl<0 || g_dbglvl>MAXDBGLVL) {
				fprintf(stderr, "Error unsuported debug level %d.\n", g_dbglvl);
				exit(1);
			}
			break;

		case 'h':
		default:
			printhelp(argv[0]);
			exit(1);
			break;
		}
	}
}

int init_master(void)
{
	//cmdline(argc, argv);

    ec_slave_config_t *sc;
    struct sigaction sa;
    struct itimerval tv;

    master = ecrt_request_master(0);
    if (!master)
        return -1;

    domain1 = ecrt_master_create_domain(master);
    if (!domain1)
        return -1;

    if (!(sc_data_in = ecrt_master_slave_config(
                    master, SomanetPos, SOMANET_ID))) {
        fprintf(stderr, "Failed to get slave configuration.\n");
        return -1;
    }

#if SDO_ACCESS
    fprintf(stderr, "Creating SDO requests...\n");
    if (!(sdo = ecrt_slave_config_create_sdo_request(sc_data_in, 0x6041, 0, 1))) {
        fprintf(stderr, "Failed to create SDO request.\n");
        return -1;
    }
    ecrt_sdo_request_timeout(sdo, 500); // ms

    if (!(request[0] = ecrt_slave_config_create_sdo_request(sc_data_in, CAN_OD_POS_VALUE, 0, 4))) {
	    fprintf(stderr, "Failed to create SDO request for object 0x%4x\n", CAN_OD_POS_VALUE);
	    return -1;
    }
    ecrt_sdo_request_timeout(request[0], 500); // ms

    if (!(request[1] = ecrt_slave_config_create_sdo_request(sc_data_in, CAN_OD_VEL_VALUE, 0, 4))) {
	    fprintf(stderr, "Failed to create SDO request for object 0x%4x\n", CAN_OD_VEL_VALUE);
	    return -1;
    }
    ecrt_sdo_request_timeout(request[1], 500); // ms

    if (!(request[2] = ecrt_slave_config_create_sdo_request(sc_data_in, CAN_OD_TOR_VALUE, 0, 2))) {
	    fprintf(stderr, "Failed to create SDO request for object 0x%4x\n", CAN_OD_TOR_VALUE);
	    return -1;
    }
    ecrt_sdo_request_timeout(request[2], 500); // ms

    /* register sdo download request */
    if (!(sdo_download_requests[0] = ecrt_slave_config_create_sdo_request(sc_data_in, CAN_OD_MODES, 0, 4))) {
	    fprintf(stderr, "Failed to create SDO download request for object 0x%4x\n", CAN_OD_MODES);
	    return -1;
    }
    ecrt_sdo_request_timeout(sdo_download_requests[0], 500); // ms

    /* set the sdoexample to a specific bit muster */
    sdoexample = 0x22442244;
#endif

#if CONFIGURE_PDOS
    logmsg(1, "Configuring PDOs...\n");
    if (ecrt_slave_config_pdos(sc_data_in, EC_END, slave_0_syncs)) {
        fprintf(stderr, "Failed to configure PDOs.\n");
        return -1;
    }

    if (!(sc = ecrt_master_slave_config(
                    master, SomanetPos, SOMANET_ID))) {
        fprintf(stderr, "Failed to get slave configuration.\n");
        return -1;
    }
#endif

    // Create configuration for bus coupler
    sc = ecrt_master_slave_config(master, SomanetPos /*BusCouplerPos*/, SOMANET_ID/*Beckhoff_EK1100*/);
    if (!sc)
        return -1;

    if (ecrt_domain_reg_pdo_entry_list(domain1, domain1_regs)) {
        fprintf(stderr, "PDO entry registration failed!\n");
        return -1;
    }

	if (ecrt_master_set_send_interval(master, FREQUENCY) != 0) {
		fprintf(stderr, "failed to set send interval\n");
		exit(-1);
	}
    logmsg(1, "Activating master...\n");
    if (ecrt_master_activate(master))
        return -1;

    if (!(domain1_pd = ecrt_domain_data(domain1))) {
        return -1;
    }

#if PRIORITY
    pid_t pid = getpid();
    if (setpriority(PRIO_PROCESS, pid, -19))
        fprintf(stderr, "Warning: Failed to set priority: %s\n",
                strerror(errno));
#endif

    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGALRM, &sa, 0)) {
        fprintf(stderr, "Failed to install signal handler!\n");
        return -1;
    }

    logmsg(1, "Starting timer...\n");
    tv.it_interval.tv_sec = 0;
    tv.it_interval.tv_usec = 1000000 / FREQUENCY;
    tv.it_value.tv_sec = 0;
    tv.it_value.tv_usec = 1000;
    if (setitimer(ITIMER_REAL, &tv, NULL)) {
        fprintf(stderr, "Failed to start timer: %s\n", strerror(errno));
        return 1;
    }

    logmsg(0, "Started.\n");

//    while (1) {
//        pause();
//        while (sig_alarms != user_alarms) {
//            cyclic_task();
//            user_alarms++;
//        }
//    }

    //return 0;
}

/****************************************************************************/
