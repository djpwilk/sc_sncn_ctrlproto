
/**
 *
 * \file drive_function.c
 *
 * \brief Motor Drive functions over Ethercat
 *
 * Copyright (c) 2014, Synapticon GmbH
 * All rights reserved.
 * Author: Pavan Kanajar <pkanajar@synapticon.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Execution of this software or parts of it exclusively takes place on hardware
 *    produced by Synapticon GmbH.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the Synapticon GmbH.
 *
 */

#include "drive_function.h"
#include <stdio.h>
#include <math.h>

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

int check_quick_stop_inactive(int status_word)
{
	return (status_word & QUICK_STOP_STATE)>>5;
}

int check_shutdown_active(int status_word)
{
	return (status_word & VOLTAGE_ENABLED_STATE)>>4;
}

void set_velocity_rpm(int target_velocity, int slave_number, ctrlproto_slv_handle *slv_handles)
{
	slv_handles[slave_number].speed_setpoint = target_velocity;
}

void initialize_torque(int slave_number, ctrlproto_slv_handle *slv_handles)
{

	slv_handles[slave_number].factor_torq = (float) slv_handles[slave_number].motor_config_param.s_motor_torque_constant.motor_torque_constant;
	slv_handles[slave_number].factor_torq *= slv_handles[slave_number].factor_torq;
	slv_handles[slave_number].factor_torq *= ((float) slv_handles[slave_number].motor_config_param.s_nominal_current.nominal_current);
	slv_handles[slave_number].factor_torq /= ((float) slv_handles[slave_number].motor_config_param.s_max_torque.max_torque);

	return;
}

void set_torque_mNm(float target_torque, int slave_number, ctrlproto_slv_handle *slv_handles)
{
	slv_handles[slave_number].torque_setpoint =  (int) round( (target_torque)/slv_handles[slave_number].factor_torq);

}

float get_torque_actual_mNm(int slave_number, ctrlproto_slv_handle *slv_handles)
{
	return (  ((float) slv_handles[slave_number].torque_in ) * slv_handles[slave_number].factor_torq );
}

int get_velocity_actual_rpm(int slave_number, ctrlproto_slv_handle *slv_handles)
{
	return slv_handles[slave_number].speed_in;
}

float get_position_actual_degree(int slave_number, ctrlproto_slv_handle *slv_handles)
{
	return ((float)slv_handles[slave_number].position_in )/10000.0f;
}



int position_limit(float target_position, int slave_number, ctrlproto_slv_handle *slv_handles)
{

	float position_min = (float) slv_handles[slave_number].motor_config_param.s_software_position_min.software_position_min;
	float position_max =(float) slv_handles[slave_number].motor_config_param.s_software_position_max.software_position_max;

	if (target_position > position_max)
	{
		return   (int) round( (position_max * 10000.0f) );
	}
	else if (target_position < position_min)
	{
		return (int) round( (position_min * 10000.0f) );
	}
	else if (target_position >= position_min && target_position <= position_max)
	{
		return  (int) round( (target_position*10000.0f) );
	}
}

void set_position_degree(int target_position, int slave_number, ctrlproto_slv_handle *slv_handles)
{
	int position = 0;

	if(target_position > slv_handles[slave_number].motor_config_param.s_software_position_max.software_position_max*10000)
	{
		position = slv_handles[slave_number].motor_config_param.s_software_position_max.software_position_max*10000;
	}
	else if (target_position < slv_handles[slave_number].motor_config_param.s_software_position_min.software_position_min * 10000)
	{
		position = slv_handles[slave_number].motor_config_param.s_software_position_min.software_position_min * 10000;
	}
	else if (target_position >= slv_handles[slave_number].motor_config_param.s_software_position_min.software_position_min * 10000 \
			&& target_position <=  slv_handles[slave_number].motor_config_param.s_software_position_max.software_position_max*10000)
	{
		position = target_position;
	}
	slv_handles[slave_number].position_setpoint = position;
}

void set_profile_position_degree(float target_position, int slave_number, ctrlproto_slv_handle *slv_handles)
{
	slv_handles[slave_number].position_setpoint = position_limit(target_position, slave_number, slv_handles);
}

int position_set_flag(int slave_number, ctrlproto_slv_handle *slv_handles)
{
	int flag = check_target_reached(read_statusword(slave_number, slv_handles));
	if(flag == 0)
		return 1;
	else
		return 0;
}

int velocity_set_flag(int slave_number, ctrlproto_slv_handle *slv_handles)
{
	int flag = check_target_reached(read_statusword(slave_number, slv_handles));
	if(flag == 0)
		return 1;
	else
		return 0;
}

void set_controlword(int controlword, int slave_number, ctrlproto_slv_handle *slv_handles)
{
	slv_handles[slave_number].motorctrl_out = controlword;
}

int read_statusword(int slave_number, ctrlproto_slv_handle *slv_handles)
{
	return slv_handles[slave_number].motorctrl_status_in;
}

int init_position_profile_params(float target_position, float actual_position, int velocity, int acceleration, int deceleration)
{
	int target = (int) (target_position * 10000.0f);
	int actual = (int) (actual_position * 10000.0f);
	init_position_profile(target, actual,	velocity, acceleration, deceleration);
}

int init_linear_profile_params(float target_torque, float actual_torque, float torque_slope, int slave_number, ctrlproto_slv_handle *slv_handles)
{
	float max_torque =  slv_handles[slave_number].motor_config_param.s_motor_torque_constant.motor_torque_constant \
			* slv_handles[slave_number].motor_config_param.s_nominal_current.nominal_current;

	return init_linear_profile_float(target_torque, actual_torque, torque_slope, torque_slope, max_torque);
}

int target_position_reached(int slave_number, float target_position, float tolerance, ctrlproto_slv_handle *slv_handles)
{
	float actual_position =  get_position_actual_degree(slave_number, slv_handles);

	if(actual_position > target_position-tolerance/2.0f && actual_position < target_position + tolerance/2.0f)
	{	if(check_target_reached(read_statusword(slave_number, slv_handles)))
		{
			return 1;
		}
		else
			return 0;
	}
	else
		return 0;
}

int target_velocity_reached(int slave_number, int target_velocity, int tolerance, ctrlproto_slv_handle *slv_handles)
{
	int actual_velocity =  get_velocity_actual_rpm(slave_number, slv_handles);

	if(actual_velocity > target_velocity-tolerance/2 && actual_velocity < target_velocity + tolerance/2)
	{	if(check_target_reached(read_statusword(slave_number, slv_handles)))
		{
			return 1;
		}
		else
			return 0;
	}
	else
		return 0;
}

int target_torque_reached(int slave_number, float target_torque, float tolerance, ctrlproto_slv_handle *slv_handles)
{
	float actual_torque =  get_torque_actual_mNm(slave_number, slv_handles);

	if(actual_torque > target_torque-tolerance/2.0f && actual_torque < target_torque + tolerance/2.0f)
	{	if(check_target_reached(read_statusword(slave_number, slv_handles)))
		{
			return 1;
		}
		else
			return 0;
	}
	else
		return 0;
}

void init_node(int slave_number, master_setup_variables_t *master_setup, ctrlproto_slv_handle *slv_handles, int total_no_of_slaves)
{
	//check if node settings are up
	set_controlword(6, slave_number, slv_handles);
	while(1)
	{
		pdo_handle_ecat(master_setup, slv_handles, total_no_of_slaves);
		if(master_setup->op_flag)
		{
			if(slv_handles[slave_number].operation_mode_disp == 105)
				break;
		}
	}
}
int set_operation_mode(int operation_mode, int slave_number, master_setup_variables_t *master_setup, ctrlproto_slv_handle *slv_handles, int total_no_of_slaves)
{
	int ready = 0;
	int switch_enable = 0;
	int status_word = 0;
	int switch_on_state = 0;
	int op_enable_state = 0;


	set_controlword(0, slave_number, slv_handles);
	printf("updating motor parameters\n");
	fflush(stdout);
	/***** Set up Parameters *****/


	slv_handles[slave_number].motor_config_param.update_flag = 0;
	while(1)
	{
		if(slv_handles[slave_number].motor_config_param.update_flag == 1)
		{
			slv_handles[slave_number].motor_config_param.update_flag = 0;	// reset to update next set of paramaters
			break;
		}

		else
		{
			sdo_handle_ecat(master_setup, slv_handles, MOTOR_PARAM_UPDATE, slave_number); // motor config update
			printf (".");
			fflush(stdout);

		}
	}
	printf ("\n");
	fflush(stdout);

	set_controlword(5, slave_number, slv_handles);//*/

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
	fflush(stdout);
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
	fflush(stdout);
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
	fflush(stdout);
	#endif


	printf("updating control parameters\n");
	fflush(stdout);
	/***** Set up Parameters *****/


	if(operation_mode == CST || operation_mode == TQ)
	{
		slv_handles[slave_number].motor_config_param.update_flag = 0;
		while(1)
		{
			if(slv_handles[slave_number].motor_config_param.update_flag == 1)
			{
				slv_handles[slave_number].motor_config_param.update_flag = 0;
				break;
			}
			else
			{
				sdo_handle_ecat(master_setup, slv_handles, CST_MOTOR_UPDATE, slave_number);  //mode specific updates
				printf (".");
				fflush(stdout);
			}
		}

		slv_handles[slave_number].motor_config_param.update_flag = 0;
		while(1)
		{
			if(slv_handles[slave_number].motor_config_param.update_flag == 1)
			{
				slv_handles[slave_number].motor_config_param.update_flag = 0;
				break;
			}
			else
			{
				sdo_handle_ecat(master_setup, slv_handles, TORQUE_CTRL_UPDATE, slave_number);  //mode specific updates
				printf (".");
				fflush(stdout);
			}
		}

		slv_handles[slave_number].motor_config_param.update_flag = 0;
		while(1)
		{
			if(slv_handles[slave_number].motor_config_param.update_flag == 1)
			{
				slv_handles[slave_number].motor_config_param.update_flag = 0;
				break;
			}
			else
			{
				sdo_handle_ecat(master_setup, slv_handles, CSV_MOTOR_UPDATE, slave_number);		//mode specific updates
				printf (".");
				fflush(stdout);
			}
		}

		slv_handles[slave_number].motor_config_param.update_flag = 0;
		while(1)
		{
			if(slv_handles[slave_number].motor_config_param.update_flag == 1)
			{
				//slv_handles[slave_number].motor_config_param.update_flag = 0;
				break;
			}
			else
			{
				slv_handles[slave_number].motor_config_param.s_torque_slope.torque_slope = \
						ceil( (float) slv_handles[slave_number].motor_config_param.s_torque_slope.torque_slope / slv_handles[slave_number].factor_torq);

				sdo_handle_ecat(master_setup, slv_handles, TQ_MOTOR_UPDATE, slave_number);  //mode specific updates
				printf (".");
				fflush(stdout);
			}
		}
	}


	else if (operation_mode == CSV)
	{
		slv_handles[slave_number].motor_config_param.update_flag = 0;
		while(1)
		{
			if(slv_handles[slave_number].motor_config_param.update_flag == 1)
			{
				slv_handles[slave_number].motor_config_param.update_flag = 0; // reset to update next set of paramaters
				break;
			}
			else
			{
				sdo_handle_ecat(master_setup, slv_handles, VELOCITY_CTRL_UPDATE, slave_number);  //mode specific updates
				printf (".");
				fflush(stdout);
			}
		}

		slv_handles[slave_number].motor_config_param.update_flag = 0;
		while(1)
		{
			if(slv_handles[slave_number].motor_config_param.update_flag == 1)
			{
				//slv_handles[slave_number].motor_config_param.update_flag = 0;
				break;
			}
			else
			{
				sdo_handle_ecat(master_setup, slv_handles, CSV_MOTOR_UPDATE, slave_number);		//mode specific updates
				printf (".");
				fflush(stdout);
			}
		}
	}
	else if (operation_mode == CSP)
	{
		slv_handles[slave_number].motor_config_param.update_flag = 0;
		while(1)
		{
			if(slv_handles[slave_number].motor_config_param.update_flag == 1)
			{
				slv_handles[slave_number].motor_config_param.update_flag = 0;
				break;
			}
			else
			{
				sdo_handle_ecat(master_setup, slv_handles, POSITION_CTRL_UPDATE, slave_number);		//mode specific updates
				printf (".");
				fflush(stdout);
			}
		}

		slv_handles[slave_number].motor_config_param.update_flag = 0;
		while(1)
		{
			if(slv_handles[slave_number].motor_config_param.update_flag == 1)
			{
				//slv_handles[slave_number].motor_config_param.update_flag = 0;
				break;
			}
			else
			{
				sdo_handle_ecat(master_setup, slv_handles, CSV_MOTOR_UPDATE, slave_number);		//mode specific updates
				printf (".");
				fflush(stdout);
			}
		}

	}

	else if (operation_mode == PV)
	{
		slv_handles[slave_number].motor_config_param.update_flag = 0;
		while(1)
		{
			if(slv_handles[slave_number].motor_config_param.update_flag == 1)
			{
				slv_handles[slave_number].motor_config_param.update_flag = 0; // reset to update next set of paramaters
				break;
			}
			else
			{
				sdo_handle_ecat(master_setup, slv_handles, VELOCITY_CTRL_UPDATE, slave_number);  //mode specific updates
				printf (".");
				fflush(stdout);
			}
		}

		slv_handles[slave_number].motor_config_param.update_flag = 0;
		while(1)
		{
			if(slv_handles[slave_number].motor_config_param.update_flag == 1)
			{
				//slv_handles[slave_number].motor_config_param.update_flag = 0;
				break;
			}
			else
			{
				sdo_handle_ecat(master_setup, slv_handles, PV_MOTOR_UPDATE, slave_number);  //mode specific updates
				printf (".");
				fflush(stdout);
			}
		}
	}

	else if (operation_mode == PP)
	{
		slv_handles[slave_number].motor_config_param.update_flag = 0;
		while(1)
		{
			if(slv_handles[slave_number].motor_config_param.update_flag == 1)
			{
				slv_handles[slave_number].motor_config_param.update_flag = 0;
				break;
			}
			else
			{
				sdo_handle_ecat(master_setup, slv_handles, POSITION_CTRL_UPDATE, slave_number);		//mode specific updates
				printf (".");
				fflush(stdout);
			}
		}

		slv_handles[slave_number].motor_config_param.update_flag = 0;
		while(1)
		{
			if(slv_handles[slave_number].motor_config_param.update_flag == 1)
			{
				//slv_handles[slave_number].motor_config_param.update_flag = 0;
				break;
			}
			else
			{
				sdo_handle_ecat(master_setup, slv_handles, PP_MOTOR_UPDATE, slave_number);  //mode specific updates
				printf (".");
				fflush(stdout);
			}
		}
	}
	printf ("\n");
	fflush(stdout);




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

				//printf("operation_mode in loop\n");
			}
			else
				continue;
	}
	#ifndef print_slave
	printf("operation_mode enabled\n");
	fflush(stdout);
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
	fflush(stdout);
	#endif

	return 1;
}

int quick_stop_position(int slave_number, master_setup_variables_t *master_setup, ctrlproto_slv_handle *slv_handles, int total_no_of_slaves)
{
	int quick_stop_active = 0, quick_stop_inactive = 1;
	int status_word;
	while(!quick_stop_active && master_setup->op_flag)
	{
		pdo_handle_ecat(master_setup, slv_handles, total_no_of_slaves);
		if(master_setup->op_flag)
		{
			set_controlword(ENABLE_OPERATION_CONTROL|(~QUICK_STOP_CONTROL & 0x000F)|ENABLE_VOLTAGE_CONTROL|SWITCH_ON_CONTROL, slave_number, slv_handles);
			/*************check quick_stop_state**************/
			status_word = read_statusword(slave_number, slv_handles);
			quick_stop_active = check_quick_stop_active(status_word);
		}
		else
			continue;

	}
#ifndef print_slave
	printf("quick_stop_active\n");
	fflush(stdout);
#endif



	while(quick_stop_inactive)
	{
		pdo_handle_ecat(master_setup, slv_handles, total_no_of_slaves);
		if(master_setup->op_flag)
		{
			/*************check quick_stop_state**************/
			status_word = read_statusword(slave_number, slv_handles);
			quick_stop_inactive = check_quick_stop_active(status_word);

		}
		else
			continue;
	}
	set_position_degree(get_position_actual_degree(slave_number, slv_handles)*10000, slave_number, slv_handles);
	#ifndef print_slave
	printf("ack stop received \n");
	fflush(stdout);
	#endif
}

int renable_ctrl_quick_stop(int operation_mode, int slave_number, master_setup_variables_t *master_setup, ctrlproto_slv_handle *slv_handles, int total_no_of_slaves)
{
	/**********************output Mode of Operation******************/
	while(1)
	{
			pdo_handle_ecat(master_setup, slv_handles, total_no_of_slaves);
			if(master_setup->op_flag)
			{
				slv_handles[slave_number].operation_mode = 100;
				slv_handles[slave_number].speed_setpoint = slv_handles[slave_number].speed_in;
				slv_handles[slave_number].position_setpoint = slv_handles[slave_number].position_in;
				slv_handles[slave_number].torque_setpoint = slv_handles[slave_number].torque_in;


				/*************check operation_mode display**************/
				set_controlword(QUICK_STOP_CONTROL, slave_number, slv_handles);

				if (slv_handles[slave_number].operation_mode_disp == 100)
					break;
			}
			else
				continue;
	}
	#ifndef print_slave
	printf("operation_mode reset enabled\n");
	fflush(stdout);
	#endif


}

int shutdown_operation(int operation_mode, int slave_number, master_setup_variables_t *master_setup, ctrlproto_slv_handle *slv_handles, int total_no_of_slaves)
{
	int ack_stop = 1, status_word;
	while(ack_stop)
	{
			pdo_handle_ecat(master_setup, slv_handles, total_no_of_slaves);
			if(master_setup->op_flag)
			{

				slv_handles[slave_number].operation_mode = 100;
				/*************check operation_mode display**************/
				set_controlword(SHUTDOWN, slave_number, slv_handles);
				status_word = read_statusword(slave_number, slv_handles);
				ack_stop = check_shutdown_active(status_word);

			}
			else
				continue;
	}
	#ifndef print_slave
	printf("shutdown  \n");
	fflush(stdout);
	#endif
}

int quick_stop(int slave_number, master_setup_variables_t *master_setup, ctrlproto_slv_handle *slv_handles, int total_no_of_slaves)
{
	int quick_stop_active = 0, status_word, ack_stop;
	while(!quick_stop_active && master_setup->op_flag)
	{
		pdo_handle_ecat(master_setup, slv_handles, total_no_of_slaves);
		if(master_setup->op_flag)
		{
			set_controlword(ENABLE_OPERATION_CONTROL|(~QUICK_STOP_CONTROL & 0x000F)|ENABLE_VOLTAGE_CONTROL|SWITCH_ON_CONTROL, slave_number, slv_handles);
			/*************check quick_stop_state**************/
			status_word = read_statusword(slave_number, slv_handles);
			quick_stop_active = check_quick_stop_active(status_word);
		}
		else
			continue;

	}

	#ifndef print_slave
	printf("quick_stop_active\n");
	fflush(stdout);
	#endif

	ack_stop = 1;
	while(ack_stop)
	{
		pdo_handle_ecat(master_setup, slv_handles, total_no_of_slaves);
		if(master_setup->op_flag)
		{
			/*************check quick_stop_state**************/
			status_word = read_statusword(slave_number, slv_handles);
			ack_stop = check_quick_stop_inactive(status_word);;

		}
		else
			continue;
	}

	#ifndef print_slave
	printf("quick stop executed \n");
	fflush(stdout);
	#endif
}

int quick_stop_torque(int slave_number, master_setup_variables_t *master_setup, ctrlproto_slv_handle *slv_handles, int total_no_of_slaves)
{
	return quick_stop(slave_number, master_setup, slv_handles, total_no_of_slaves);
}

int quick_stop_velocity(int slave_number, master_setup_variables_t *master_setup, ctrlproto_slv_handle *slv_handles, int total_no_of_slaves)
{
	return quick_stop(slave_number, master_setup, slv_handles, total_no_of_slaves);
}

int renable_velocity_ctrl(int slave_number, master_setup_variables_t *master_setup, ctrlproto_slv_handle *slv_handles, int total_no_of_slaves)
{
	/**********************output Mode of Operation******************/
	while(1)
	{
			pdo_handle_ecat(master_setup, slv_handles, total_no_of_slaves);
			if(master_setup->op_flag)
			{
				slv_handles[slave_number].operation_mode = 100;
				/*************check operation_mode display**************/
				set_controlword(QUICK_STOP_CONTROL, slave_number, slv_handles);

				if (slv_handles[slave_number].operation_mode_disp == 100)
					break;
			}
			else
				continue;
	}
	#ifndef print_slave
	printf("operation_mode reset enabled\n");
	fflush(stdout);
	#endif

	while(1)
	{
			pdo_handle_ecat(master_setup, slv_handles, total_no_of_slaves);
			if(master_setup->op_flag)
			{
				slv_handles[slave_number].operation_mode = CSV;
				/*************check operation_mode display**************/

				if (slv_handles[slave_number].operation_mode_disp == CSV)
					break;
			}
			else
				continue;
	}
	#ifndef print_slave
	printf("operation_mode CSV enabled\n");
	fflush(stdout);
	#endif
}

