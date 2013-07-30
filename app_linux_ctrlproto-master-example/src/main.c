#include <ctrlproto_m.h>
#include <ecrt.h>
#include <stdio.h>
#include "drive_function.h"

#include "ethercat_setup.h"
#include <sys/time.h>


#define print_slave
void run_drive();



void set_controlword(int controlword)
{
	slv_handles[0].motorctrl_out = controlword;
}

int read_statusword()
{
	return slv_handles[0].motorctrl_status_in;
}

int main()
{
	int ready = 0;
	int switch_enable = 0;
	int status_word = 0;
	int switch_on_state = 0;
	int op_enable_state = 0;
	int control_word;

	init_master(&master_setup, slv_handles, NUM_SLAVES);

	printf("master \n");



	while(!ready)
	{
		handleEcat(&master_setup,slv_handles, NUM_SLAVES);
		if(master_setup.opFlag)
		{
			//check ready
			status_word = read_statusword();
			ready = check_ready(status_word);
		}
		else
			continue;
	}

	#ifndef print_slave
	printf("ready");
	#endif

	while(!switch_enable)
	{
		handleEcat(&master_setup,slv_handles, NUM_SLAVES);
		if(master_setup.opFlag)
		{
			//check switch
			status_word = read_statusword();
			switch_enable = check_switch_enable(status_word);
		}
		else
			continue;
	}

	#ifndef print_slave
	printf("switch_enable");
	#endif

	//out CW for S ON

	while(!switch_on_state)
	{
		handleEcat(&master_setup,slv_handles, NUM_SLAVES);
		if(master_setup.opFlag)
		{
			set_controlword(SWITCH_ON_CONTROL);
			//check switch_on_state
			status_word = read_statusword();
			switch_on_state = check_switch_on(status_word);
		}
		else
			continue;
	}

	#ifndef print_slave
	printf("switch_on_state");
	#endif

	//out CW for En OP

	while(!op_enable_state && master_setup.opFlag)
	{
		handleEcat(&master_setup,slv_handles, NUM_SLAVES);
		if(master_setup.opFlag)
		{
			set_controlword(ENABLE_OPERATION_CONTROL|QUICK_STOP_CONTROL);
			//check op_enable_state
			status_word = read_statusword();
			op_enable_state = check_op_enable(status_word);
		}
		else
			continue;
	}

	#ifndef print_slave
	printf("op_enable_state");
	#endif

	return 0;
}


/*
 	if(master_setup.opFlag)//Check if we are up
	{
		slv_handles[0].motorctrl_out = 12;
		slv_handles[0].torque_setpoint = 200;
		slv_handles[0].speed_setpoint = 4000;
		slv_handles[0].position_setpoint = 10000;
		slv_handles[0].operation_mode = 234;

		printf("Status: %i\n",slv_handles[0].motorctrl_status_in);
		printf("Position: %i\n",slv_handles[0].position_in);
		printf("Speed: %i\n",slv_handles[0].speed_in);
		printf("Torque: %i\n",slv_handles[0].torque_in);
		printf("Operation Mode disp: %i\n",slv_handles[0].operation_mode_disp);

	}
	else
	{
		//This is just a example what CAN be done when the master recognizes
		//that, for example, a slave is now missing...
		slv_handles[0].motorctrl_out= 0;
		slv_handles[0].torque_setpoint=0;
		slv_handles[0].speed_setpoint=0;
		slv_handles[0].position_setpoint=0;
		slv_handles[0].operation_mode=0;
	}
*/
