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
#include <ctrlproto_m.h>
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

/*EtherCAT
static ec_master_t *master = NULL;
static ec_master_state_t master_state;

static ec_domain_t *domain1 = NULL;
static ec_domain_state_t domain1_state;

static ec_slave_config_t *sc_data_in = NULL;
static ec_slave_config_state_t sc_data_in_state;
*/

// Timer
static unsigned int sig_alarms = 0;
static unsigned int user_alarms = 0;

/****************************************************************************/



static unsigned int counter = 0;
static unsigned int blink = 0;

/*****************************************************************************/

static void logmsg(int lvl, const char *format, ...);

/*****************************************************************************

#if SDO_ACCESS
static ec_sdo_request_t *sdo;

/* additional sdo requests
static ec_sdo_request_t *request[3];

static ec_sdo_request_t *sdo_download_requests[1]; // one for each object
static unsigned sdoexample;
#endif

/*****************************************************************************/

void check_domain1_state(master_setup_variables_t *master_setup)
{
    ec_domain_state_t ds;

    ecrt_domain_state(master_setup->domain, &ds);

    if (ds.working_counter != master_setup->domain_state.working_counter)
        logmsg(1, "Domain1: WC %u.\n", ds.working_counter);
    if (ds.wc_state != master_setup->domain_state.wc_state)
    	logmsg(1, "Domain1: State %u.\n", ds.wc_state);

    	master_setup->domain_state = ds;
}

/****************************************************************************/

void check_master_state(master_setup_variables_t *master_setup)
{
    ec_master_state_t ms;

    ecrt_master_state(master_setup->master, &ms);

    if (ms.slaves_responding != master_setup->master_state.slaves_responding)
        logmsg(1, "%u slave(s).\n", ms.slaves_responding);
    if (ms.al_states != master_setup->master_state.al_states)
        logmsg(1, "AL states: 0x%02X.\n", ms.al_states);
    if (ms.link_up != master_setup->master_state.link_up)
        logmsg(1, "Link is %s.\n", ms.link_up ? "up" : "down");

    master_setup->master_state = ms;
}

/****************************************************************************

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

/****************************************************************************

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

void handle_ecat(master_setup_variables_t *master_setup,
        ctrlproto_slv_handle *slv_handles,
        unsigned int slave_num)
{
	int slv;
	if(sig_alarms == user_alarms) pause();
	while (sig_alarms != user_alarms)
	{
		/* sync the dc clock of the slaves */
		//	ecrt_master_sync_slave_clocks(master);

		// receive process data
		ecrt_master_receive(master_setup->master);
		ecrt_domain_process(master_setup->domain);

		// check process data state (optional)
		//check_domain1_state(master_setup);

		// check for master state (optional)
		//check_master_state(master_setup);

		// check for islave configuration state(s) (optional)
		// check_slave_config_states();

	#if SDO_ACCESS
	//		// read process data SDO
	//		read_sdo(sdo);
	//		read_sdo(request[0]);
	//		read_sdo(request[1]);
	//		read_sdo(request[2]);
	//
	//		write_sdo(sdo_download_requests[0], sdoexample); /* SDO download value to the node */
	#endif

		for(slv=0;slv<slave_num;++slv)
		{
			/* Read process data */
			slv_handles[slv].motorctrl_status_in = EC_READ_U16(master_setup->domain_pd + slv_handles[slv].__ecat_slave_in[0]);
			slv_handles[slv].operation_mode_disp = EC_READ_U8(master_setup->domain_pd + slv_handles[slv].__ecat_slave_in[1]);
			slv_handles[slv].position_in = EC_READ_U32(master_setup->domain_pd + slv_handles[slv].__ecat_slave_in[2]);
			slv_handles[slv].speed_in = EC_READ_U32(master_setup->domain_pd + slv_handles[slv].__ecat_slave_in[3]);
			slv_handles[slv].torque_in = EC_READ_U16(master_setup->domain_pd + slv_handles[slv].__ecat_slave_in[4]);
		}

//		printf("\n%x", 	slv_handles[slv].motorctrl_status_in);
//		printf("\n%x",  slv_handles[slv].operation_mode_disp);
//		printf("\n%x",  slv_handles[slv].position_in);
//		printf("\n%x",  slv_handles[slv].speed_in);
//		printf("\n%x",  slv_handles[slv].torque_in);


		for(slv=0;slv<slave_num;++slv)
		{
			EC_WRITE_U16(master_setup->domain_pd + slv_handles[slv].__ecat_slave_out[0], (slv_handles[slv].motorctrl_out)&0xffff);
			EC_WRITE_U8(master_setup->domain_pd + slv_handles[slv].__ecat_slave_out[1], (slv_handles[slv].operation_mode)&0xff);
			EC_WRITE_U16(master_setup->domain_pd + slv_handles[slv].__ecat_slave_out[2], (slv_handles[slv].torque_setpoint)&0xffff);
			EC_WRITE_U32(master_setup->domain_pd + slv_handles[slv].__ecat_slave_out[3], slv_handles[slv].position_setpoint);
			EC_WRITE_U32(master_setup->domain_pd + slv_handles[slv].__ecat_slave_out[4], slv_handles[slv].speed_setpoint);
		}

		// send process data
		ecrt_domain_queue(master_setup->domain);
		ecrt_master_send(master_setup->master);

		//Check for master und domain state
		ecrt_master_state(master_setup->master, &master_setup->master_state);
		ecrt_domain_state(master_setup->domain, &master_setup->domain_state);

		if (master_setup->domain_state.wc_state == EC_WC_COMPLETE && !master_setup->opFlag)
		{
			//printf("System up!\n");
			master_setup->opFlag = 1;
		}
		else
		{
			if(master_setup->domain_state.wc_state != EC_WC_COMPLETE && master_setup->opFlag)
			{
				//printf("System down!\n");
				master_setup->opFlag = 0;
			}
		}

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


void init_master(master_setup_variables_t *master_setup, ctrlproto_slv_handle *slv_handles, unsigned int slave_num)
{
	int slv;

    struct sigaction sa;
    struct itimerval tv;

    master_setup->master = ecrt_request_master(0);
    if (!master_setup->master)
        exit(-1);

    master_setup->domain = ecrt_master_create_domain(master_setup->master);
    if (!master_setup->domain)
        exit(-1);

	for (slv = 0; slv < slave_num; ++slv)
	{
		if (!( slv_handles[slv].slave_config= ecrt_master_slave_config(   //sc_data_in
						master_setup->master, slv_handles[slv].slave_alias, slv_handles[slv].slave_pos , slv_handles[slv].slave_vendorid, slv_handles[slv].slave_productid))) {
			fprintf(stderr, "Failed to get slave configuration.\n");
			exit(-1);
		}

		logmsg(1, "Configuring PDOs...\n");
		if (ecrt_slave_config_pdos(slv_handles[slv].slave_config, EC_END, slv_handles[slv].__sync_info)) { //slave_0_syncs
		  fprintf(stderr, "Failed to configure PDOs.\n");
		  exit(-1);
		}
	}
//      if (!(sc = ecrt_master_slave_config(
//                      master, SomanetPos, SOMANET_ID))) {
//          fprintf(stderr, "Failed to get slave configuration.\n");
//          exit(-1);
//      }
//#if SDO_ACCESS
/*    fprintf(stderr, "Creating SDO requests...\n");
    if (!(sdo = ecrt_slave_config_create_sdo_request(sc_data_in, 0x6041, 0, 1))) {
        fprintf(stderr, "Failed to create SDO request.\n");
        exit(-1);
    }
    ecrt_sdo_request_timeout(sdo, 500); // ms

    if (!(request[0] = ecrt_slave_config_create_sdo_request(sc_data_in, CAN_OD_POS_VALUE, 0, 4))) {
	    fprintf(stderr, "Failed to create SDO request for object 0x%4x\n", CAN_OD_POS_VALUE);
	    exit(-1);
    }
    ecrt_sdo_request_timeout(request[0], 500); // ms

    if (!(request[1] = ecrt_slave_config_create_sdo_request(sc_data_in, CAN_OD_VEL_VALUE, 0, 4))) {
	    fprintf(stderr, "Failed to create SDO request for object 0x%4x\n", CAN_OD_VEL_VALUE);
	    exit(-1);
    }
    ecrt_sdo_request_timeout(request[1], 500); // ms

    if (!(request[2] = ecrt_slave_config_create_sdo_request(sc_data_in, CAN_OD_TOR_VALUE, 0, 2))) {
	    fprintf(stderr, "Failed to create SDO request for object 0x%4x\n", CAN_OD_TOR_VALUE);
	    exit(-1);
    }
    ecrt_sdo_request_timeout(request[2], 500); // ms

    /* register sdo download request
    if (!(sdo_download_requests[0] = ecrt_slave_config_create_sdo_request(sc_data_in, CAN_OD_MODES, 0, 4))) {
	    fprintf(stderr, "Failed to create SDO download request for object 0x%4x\n", CAN_OD_MODES);
	    exit(-1);
    }
    ecrt_sdo_request_timeout(sdo_download_requests[0], 500); // ms

    /* set the sdoexample to a specific bit muster
    sdoexample = 0x22442244;
#endif*/

//#if CONFIGURE_PDOS

//#endif

    // Create configuration for bus coupler
//    sc = ecrt_master_slave_config(master, SomanetPos /*BusCouplerPos*/, SOMANET_ID/*Beckhoff_EK1100*/);
//    if (!sc)
//        exit(-1);

    if (ecrt_domain_reg_pdo_entry_list(master_setup->domain, master_setup->domain_regs)) {
        fprintf(stderr, "PDO entry registration failed!\n");
        exit(-1);
    }

	if (ecrt_master_set_send_interval(master_setup->master, FREQUENCY) != 0) {
		fprintf(stderr, "failed to set send interval\n");
		exit(-1);
	}
    logmsg(1, "Activating master...\n");
    if (ecrt_master_activate(master_setup->master))
        exit(-1);

    if (!(master_setup->domain_pd = ecrt_domain_data(master_setup->domain))) {
        exit(-1);
    }


    pid_t pid = getpid();
    if (setpriority(PRIO_PROCESS, pid, -19))
        fprintf(stderr, "Warning: Failed to set priority: %s\n",
                strerror(errno));

    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGALRM, &sa, 0)) {
        fprintf(stderr, "Failed to install signal handler!\n");
        exit(-1);
    }

    logmsg(1, "Starting timer...\n");
    tv.it_interval.tv_sec = 0;
    tv.it_interval.tv_usec = 1000000 / FREQUENCY;
    tv.it_value.tv_sec = 0;
    tv.it_value.tv_usec = 1000;
    if (setitimer(ITIMER_REAL, &tv, NULL)) {
        fprintf(stderr, "Failed to start timer: %s\n", strerror(errno));
        exit(-1);
    }

    logmsg(0, "Started.\n");

}

/****************************************************************************/
