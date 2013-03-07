#include <ctrlproto_m.h>
#include <string.h>
#include <stdio.h>
#include <ecrt.h>

#define SOMANET_ID 0x000022d2, 0x00000201



int16_t fromFloat(float value)
{
	return (int16_t)(value*1000);
}

float toFloat(int16_t value)
{
	return (float)(value/1000);
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
}



void handleEcat(master_setup_variables_t *master_setup,
		        ctrlproto_slv_handle *slv_handles,
		        unsigned int slave_num,
		        unsigned int pause_betw_loops)
{
	int slv;

	//If first Run, skip this.
	if(master_setup->nFirstRun)
	{
		usleep(pause_betw_loops);
		//Sending values out
	    for(slv=0;slv<slave_num;++slv)
	    {
	    	//Write slave values
	    	EC_WRITE_U16(master_setup->domain_pd+slv_handles[slv].__ecat_slave_out_1,slv_handles[slv].out[0]);
	    	EC_WRITE_U16(master_setup->domain_pd+slv_handles[slv].__ecat_slave_out_2,slv_handles[slv].out[1]);
	    }
		ecrt_master_send(master_setup->master);
		ecrt_domain_queue(master_setup->domain);
	}
	else
	{
		init_master(master_setup, slv_handles, slave_num);
	}

	//LOOP BEGIN -->
	ecrt_master_receive(master_setup->master);
    ecrt_domain_process(master_setup->domain);
	if(!master_setup->nFirstRun)
	{
		master_setup->nFirstRun=1;
	}

	//Receiving
	for(slv=0;slv<slave_num;++slv)
	{
		slv_handles[slv].in[0]=EC_READ_U16(master_setup->domain_pd+slv_handles[slv].__ecat_slave_in_1);
		slv_handles[slv].in[1]=EC_READ_U16(master_setup->domain_pd+slv_handles[slv].__ecat_slave_in_2);
		uint8_t hb=(uint8_t)(slv_handles[slv].in[0]&0xFF00);
		slv_handles[slv].in[0]&=0xFF;
		slv_handles[slv].is_responding=slv_handles[slv].__last_heartbeat_value!=hb;
	}

	//Check for master und domain state
	ecrt_master_state(master_setup->master, &master_setup->master_state);
	ecrt_domain_state(master_setup->domain, &master_setup->domain_state);
	if (master_setup->domain_state.wc_state == EC_WC_COMPLETE && !master_setup->opFlag)
	{
		printf("Operational!\n");
			master_setup->opFlag = 1;
	}

	if(!master_setup->opFlag)
	{
		ecrt_domain_state(master_setup->domain, &master_setup->domain_state);
	}

}

bool setSlave(unsigned int slave_no, ctrl_proto_xmos_cmd_t cmd, int16_t value, bool force_write, ctrlproto_slv_handle *slv_handles)
{
	ctrlproto_slv_handle* ptr=slv_handles+slave_no;
	if(!ptr->is_responding && !force_write)
	{
		return false;
	}
	ptr->out[0]=cmd;
	ptr->out[1]=value;
	return ptr->is_responding;
}

bool getSlave(unsigned int slave_no, ctrl_proto_xmos_cmd_t *what, int16_t *value, ctrlproto_slv_handle *slv_handles)
{
	ctrlproto_slv_handle* ptr=slv_handles+slave_no;
	*what=ptr->in[0]&0xFF;
	*value=ptr->in[1];

	if(!ptr->is_responding)
	{
		return false;
	}
	else
	{
		return true;
	}
}
