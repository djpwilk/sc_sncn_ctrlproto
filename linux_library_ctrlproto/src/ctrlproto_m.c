#include <ctrlproto_m.h>
#include <string.h>
#include <stdio.h>
#include <ecrt.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>


#define SOMANET_ID 0x000022d2, 0x00000201
static unsigned int user_alarms = 0;
static unsigned int sig_alarms = 0;
static struct itimerval tv;
struct sigaction sa;

int32_t fromFloat(float value)
{
	return (int32_t)(value*1000);
}

float toFloat(int32_t value)
{
	return (float)(value/1000);
}

void signal_handler(int signum) {
    switch (signum) {
        case SIGALRM:
            sig_alarms++;
            break;
    }
}

void init_master(master_setup_variables_t *master_setup,
				 ctrlproto_slv_handle *slv_handles,
				 unsigned int slave_num)
{
	int slv;
	//Request master
	master_setup->master = ecrt_request_master(0);
	if(!master_setup->master)
	{
			fprintf(stderr, "Unable to get requested master.\n");
			exit (-1);
	}
	else
	{
		printf("Connected to Master\n");
	}

	//Create Domain
	master_setup->domain = ecrt_master_create_domain(master_setup->master);
	if (!master_setup->domain)
	{
			fprintf(stderr, "Unable to create process data domain.\n");
			exit (-1);
	}

	//Slaves
	for (slv = 0; slv < slave_num; ++slv)
	{
		if (!(slv_handles[slv].slave_config = ecrt_master_slave_config(
						master_setup->master, slv_handles[slv].slave_alias, slv_handles[slv].slave_pos , slv_handles[slv].slave_vendorid, slv_handles[slv].slave_productid))) {
			fprintf(stderr, "Failed to get slave configuration.\n");
			exit (-1);
		}


	    if (ecrt_slave_config_pdos(slv_handles[slv].slave_config, EC_END, slv_handles[slv].__sync_info)) {
	        fprintf(stderr, "Failed to configure PDOs.\n");
	    	exit (-1);
	    }
	}





	printf("Registering pdo entry list...");
	if (ecrt_domain_reg_pdo_entry_list(master_setup->domain, master_setup->domain_regs))
	{
			printf("failed!\n");
			exit(-1);
	}
	printf("ok\n");



	if (ecrt_master_set_send_interval(master_setup->master, 100) != 0) {
		fprintf(stderr, "failed to set send interval\n");
		exit(-1);
	}


	//Activate Master
	printf("Activating master...");
	if (ecrt_master_activate(master_setup->master)) {
			fprintf(stderr,"activation failed.\n");
			exit (-1);
	}
	printf("ok!\n");

	//Domain init
	if (!(master_setup->domain_pd = ecrt_domain_data(master_setup->domain)))
	{
			fprintf(stderr,"Domain data initialization failed.\n");
			exit (-1);
	}
	printf("Domain data registered ok.\n");

	ecrt_master_state(master_setup->master, &master_setup->master_state);
	printf("%u slave(s).\n", master_setup->master_state.slaves_responding);
	printf("AL states: 0x%02X.\n", master_setup->master_state.al_states);
	printf("Link is %s.\n", master_setup->master_state.link_up ? "up" : "down");

	ecrt_domain_state(master_setup->domain, &master_setup->domain_state);


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
        exit(-1);
    }

    printf("Starting timer...");
    tv.it_interval.tv_sec = 0;
    tv.it_interval.tv_usec = 1000000 / 100; //FREQUENCY
    tv.it_value.tv_sec = 0;
    tv.it_value.tv_usec = 1000;

    if (setitimer(ITIMER_REAL, &tv, NULL)) {
    	printf("\nFailed to start timer!\n");
    	exit(-1);
    }
    else
    {
    	printf("ok \n");
    }

}


void handleEcat(master_setup_variables_t *master_setup,
		        ctrlproto_slv_handle *slv_handles,
		        unsigned int slave_num)
{
	int slv;

	//If first Run, skip this.
	if(master_setup->nFirstRun)
	{

		//Sending values out
	    for(slv=0;slv<slave_num;++slv)
	    {
	    	//Write slave values
	    	EC_WRITE_U16(master_setup->domain_pd+slv_handles[slv].__ecat_slave_out[0],slv_handles[slv].motorctrl_cmd);
	      	EC_WRITE_U32(master_setup->domain_pd+slv_handles[slv].__ecat_slave_out[1],slv_handles[slv].torque_setpoint);
	      	EC_WRITE_U32(master_setup->domain_pd+slv_handles[slv].__ecat_slave_out[2],slv_handles[slv].speed_setpoint);
	      	EC_WRITE_U32(master_setup->domain_pd+slv_handles[slv].__ecat_slave_out[3],slv_handles[slv].position_setpoint);
	      	EC_WRITE_U32(master_setup->domain_pd+slv_handles[slv].__ecat_slave_out[4],slv_handles[slv].userdef_setpoint);
	    }
		ecrt_domain_queue(master_setup->domain);
		ecrt_master_send(master_setup->master);
		user_alarms++;
	}
	else
	{
		master_setup->nFirstRun=1;
	}
	//EC CYCLE BEGIN -->

	if(sig_alarms == user_alarms) pause();

	//ecrt_master_sync_slave_clocks(master_setup->master);
	ecrt_master_receive(master_setup->master);
	ecrt_domain_process(master_setup->domain);


	//Receiving
	for(slv=0;slv<slave_num;++slv)
	{
		slv_handles[slv].motorctrl_cmd_readback=(EC_READ_U16(master_setup->domain_pd+slv_handles[slv].__ecat_slave_in[0]))&0xFF;
		slv_handles[slv].torque_in=EC_READ_U32(master_setup->domain_pd+slv_handles[slv].__ecat_slave_in[1]);
		slv_handles[slv].speed_in=EC_READ_U32(master_setup->domain_pd+slv_handles[slv].__ecat_slave_in[2]);
		slv_handles[slv].position_in=EC_READ_U32(master_setup->domain_pd+slv_handles[slv].__ecat_slave_in[3]);
		slv_handles[slv].userdef_in=EC_READ_U32(master_setup->domain_pd+slv_handles[slv].__ecat_slave_in[4]);
	}

	//Check for master und domain state
	ecrt_master_state(master_setup->master, &master_setup->master_state);
	ecrt_domain_state(master_setup->domain, &master_setup->domain_state);
	if (master_setup->domain_state.wc_state == EC_WC_COMPLETE && !master_setup->opFlag)
	{
		printf("System up!\n");
			master_setup->opFlag = 1;
	}
	else
	{
		if(master_setup->domain_state.wc_state != EC_WC_COMPLETE && master_setup->opFlag)
		{
			printf("System down!\n");
			master_setup->opFlag = 0;
		}
	}
}
