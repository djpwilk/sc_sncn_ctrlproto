/*
 * drive_function.c
 *
 *  Created on: Jul 26, 2013
 *      Author: pkanajar
 */
#include "drive_function.h"
#include <stdio.h>

int check_ready(int status_word) {
	return (status_word & READY_TO_SWITCH_ON_STATE);
}

int check_switch_enable(int status_word) {
	return (~((status_word & SWITCH_ON_DISABLED_STATE) >> 6) & 0x1);
}

int check_switch_on(int status_word) {
	return (status_word & SWITCHED_ON_STATE) >> 1;
}

int check_op_enable(int status_word) {
	return (status_word & OPERATION_ENABLED_STATE) >> 2;
}

int check_quick_stop_active(int status_word){
	return (status_word & QUICK_STOP_STATE)>>5;
}

int check_target_reached(int status_word)
{
	return (status_word & TARGET_REACHED)>>10;
}



void set_controlword(int controlword, int slave_number, ctrlproto_slv_handle *slv_handles)
{
	slv_handles[slave_number].motorctrl_out = controlword;
}

int read_statusword(int slave_number, ctrlproto_slv_handle *slv_handles)
{
	return slv_handles[slave_number].motorctrl_status_in;
}

int set_operation_mode(int operation_mode, int slave_number, master_setup_variables_t *master_setup, ctrlproto_slv_handle *slv_handles, int total_no_of_slaves)
{
	int ready = 0;
	int switch_enable = 0;
	int status_word = 0;
	int switch_on_state = 0;
	int op_enable_state = 0;

	/**********************check ready***********************/
	while(!ready)
	{
		pdo_handle_ecat(master_setup, slv_handles, total_no_of_slaves);
		if(master_setup->op_flag)
		{
			//check ready
			status_word = read_statusword(slave_number, slv_handles);
			ready = check_ready(status_word);
		}
		else
			continue;
	}

	#ifndef print_slave
	printf("ready\n");
	#endif

	/**********************check switch_enable***********************/
	while(!switch_enable)
	{
		pdo_handle_ecat(master_setup, slv_handles, total_no_of_slaves);
		if(master_setup->op_flag)
		{
			//check switch
			status_word = read_statusword(slave_number, slv_handles);
			switch_enable = check_switch_enable(status_word);
		}
		else
			continue;
	}

	#ifndef print_slave
	printf("switch_enable\n");
	#endif


	/************************output switch on**************************/
	while(!switch_on_state)
	{
		pdo_handle_ecat(master_setup, slv_handles, total_no_of_slaves);
		if(master_setup->op_flag)
		{
			set_controlword(SWITCH_ON_CONTROL, slave_number, slv_handles);
			/*************check switch_on_state***************/
			status_word = read_statusword(slave_number, slv_handles);
			switch_on_state = check_switch_on(status_word);
		}
		else
			continue;
	}

	#ifndef print_slave
	printf("switch_on_state\n");
	#endif


	printf("updating parameters\n");
	/***** Set up Parameters *****/
	while(1)
	{
		if(slv_handles[slave_number].motor_config_param.update_flag == 1)
		{
			slv_handles[slave_number].motor_config_param.update_flag = 0;	// reset to update next set of paramaters
			break;
		}

		else
		{
			sdo_handle_ecat(master_setup, slv_handles, total_no_of_slaves, MOTOR_PARAM_UPDATE);
			printf (".");
			fflush(stdout);

		}
	}

	if (operation_mode == CSV)
	{
		while(1)
		{
			if(slv_handles[slave_number].motor_config_param.update_flag == 1)
			{
				slv_handles[slave_number].motor_config_param.update_flag = 0; // reset to update next set of paramaters
				break;
			}
			else
			{
				sdo_handle_ecat(master_setup, slv_handles, total_no_of_slaves, VELOCITY_CTRL_UPDATE);  //mode specific updates
				printf (".");
			}
		}

		while(1)
		{
			if(slv_handles[slave_number].motor_config_param.update_flag == 1)
			{
				//slv_handles[slave_number].motor_config_param.update_flag = 0;
				break;
			}
			else
			{
				sdo_handle_ecat(master_setup, slv_handles, total_no_of_slaves, CSV_MOTOR_UPDATE);		//mode specific updates
				printf (".");
			}
		}
	}


	/**********************output Mode of Operation******************/
	while(1)
	{
			pdo_handle_ecat(master_setup, slv_handles, total_no_of_slaves);
			if(master_setup->op_flag)
			{
				slv_handles[slave_number].operation_mode = operation_mode;
				/*************check operation_mode display**************/
				if (slv_handles[slave_number].operation_mode_disp == operation_mode)
					break;
			}
			else
				continue;
	}
	#ifndef print_slave
	printf("operation_mode enabled\n");
	#endif

	return 1;
}


int enable_operation(int slave_number, master_setup_variables_t *master_setup, ctrlproto_slv_handle *slv_handles, int total_no_of_slaves)
{
	int op_enable_state = 0;
	int status_word = 0;

	while(!op_enable_state && master_setup->op_flag)
	{
		pdo_handle_ecat(master_setup, slv_handles, total_no_of_slaves);
		if(master_setup->op_flag)
		{
			set_controlword(SWITCH_ON, slave_number, slv_handles);
			/*************check op_enable_state**************/
			status_word = read_statusword(slave_number, slv_handles);
			op_enable_state = check_op_enable(status_word);
		}
		else
			continue;
	}

	#ifndef print_slave
	printf("operation enabled\n");
	#endif

	return 1;
}

void run_drive()
{
/*	int ready = 0;
	int switch_enable = 0;
	int status_word = 0;
	int switch_on_state = 0;
	int op_enable_state = 0;
	int control_word;

	while(!ready)
	{
		//check ready
		status_word = get_statusword(info);
		ready = check_ready(status_word);
	}
#ifndef print_slave
	printstrln("ready");
#endif

	while(!switch_enable)
	{
		//check switch
		status_word = get_statusword(info);
		switch_enable = check_switch_enable(status_word);
	}
#ifndef print_slave
	printstrln("switch_enable");
#endif

	//out CW for S ON
	control_word = SWITCH_ON_CONTROL;
	set_controlword(control_word, info);


	while(!switch_on_state)
	{
		set_controlword(control_word, info);
		printintln(control_word);
		//check switch_on_state
		status_word = get_statusword(info);
		switch_on_state = check_switch_on(status_word);
	}

#ifndef print_slave
	printstrln("switch_on_state");
#endif
	//out CW for En OP
	control_word = ENABLE_OPERATION_CONTROL;
	set_controlword(control_word, info);

	while(!op_enable_state)
	{
		//check op_enable_state
		status_word = get_statusword(info);
		op_enable_state = check_op_enable(status_word);
	}

#ifndef print_slave
	printstrln("op_enable_state");
#endif
*/



}
