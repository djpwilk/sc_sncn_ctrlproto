
/**
 * \file main.c
 * \brief Example Master App for Cyclic Synchronous Velocity (on PC)
 * \author Pavan Kanajar <pkanajar@synapticon.com>
 * \author Christian Holl <choll@synapticon.com>
 * \version 1.0
 * \date 10/04/2014
 */
/*
 * Copyright (c) 2014, Synapticon GmbH
 * All rights reserved.
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

#include <ctrlproto_m.h>
#include <ecrt.h>
#include "ethercat_setup.h"
#include <stdio.h>
#include <stdbool.h>
#include "profile.h"
#include "drive_function.h"
#include <motor_define.h>
#include <sys/time.h>
#include <time.h>


int main()
{
	int flag = 0;

	int final_target_velocity = -500;			//rpm
	int acceleration= 1000;						//rpm/s
	int deceleration = 1000;					//rpm/s
	int steps = 0;
	int i = 1;
	int target_velocity = 0;					// rpm
	int actual_velocity = 0;					// rpm
	int actual_position;						// ticks
	float actual_torque;						// mNm

	int slave_number = 0;

	/* Initialize Ethercat Master */
	init_master(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	/* Initialize torque parameters */
	initialize_torque(slave_number, slv_handles);

	/* Initialize all connected nodes with Mandatory Motor Configurations (specified under config/motor/)*/
	init_nodes(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	/* Initialize the node specified with slave_number with CSV configurations (specified under config/motor/)*/
	set_operation_mode(CSV, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	/* Enable operation of node in CSV mode */
	enable_operation(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	i = 1;
	/* Initialize velocity profile parameters */
	steps = init_velocity_profile_params(final_target_velocity, actual_velocity, acceleration, \
				deceleration, slave_number, slv_handles);


	while(1)
	{
		/* Update the process data (EtherCat packets) sent/received from the node */
		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

		if(master_setup.op_flag) /*Check if the master is active*/
		{
			if(i<steps)
			{
				/* Generate target velocity steps */
				target_velocity = generate_profile_velocity( i, slave_number, slv_handles);

				/* Send target velocity for the node specified by slave_number */
				set_velocity_rpm(target_velocity, slave_number, slv_handles);

				/* Read actual node sensor values */
				actual_velocity = get_velocity_actual_rpm(slave_number, slv_handles);
				actual_position = get_position_actual_ticks(slave_number, slv_handles);
				actual_torque = get_torque_actual_mNm(slave_number, slv_handles);
				printf("Velocity: %d Position: %d Torque: %f\n", actual_velocity, actual_position, actual_torque);
				i = i+1;
			}
		/*	if(i>=steps && flag == 0)
			{
				actual_velocity = get_velocity_actual_rpm(slave_number, slv_handles);
				final_target_velocity = 1000; //rpm
				steps = init_velocity_profile_params(final_target_velocity, actual_velocity, acceleration, \
							deceleration, slave_number, slv_handles);
				i = 1;
				flag = 1;
			}
			if(i>=steps && flag == 1)
			{
				actual_velocity = get_velocity_actual_rpm(slave_number, slv_handles);
				final_target_velocity = -500;	//rpm
				steps = init_velocity_profile_params(final_target_velocity, actual_velocity, acceleration, \
							deceleration, slave_number, slv_handles);
				i = 1;
				flag = 2;
			}*/
			if(i >= steps && flag == 0)
			{
				break;
			}
		}
	}

	/* Quick stop velocity mode (for emergency) */
	quick_stop_velocity(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	/* Regain control of node to continue after quick stop */
	renable_ctrl_quick_stop(CSV, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	set_operation_mode(CSV, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	enable_operation(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	/*i = 0;
	flag = 0;
	final_target_velocity = 1000; //rpm
	actual_velocity = get_velocity_actual_rpm(slave_number, slv_handles); //rpm
	steps = init_velocity_profile_params(final_target_velocity, actual_velocity, acceleration, \
							deceleration, slave_number, slv_handles);

	while(1)
	{
		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

		if(master_setup.op_flag)//Check if we are up
		{
			if(i<steps)
			{
				target_velocity = generate_profile_velocity( i, slave_number, slv_handles);
				set_velocity_rpm(target_velocity, slave_number, slv_handles);
				actual_velocity = get_velocity_actual_rpm(slave_number, slv_handles);
				printf("velocity %d position %d\n",actual_velocity, get_position_actual_ticks(slave_number, slv_handles));
				i = i+1;
			}
			if(i>=steps && flag == 0)
			{
				actual_velocity = get_velocity_actual_rpm(slave_number, slv_handles);
				final_target_velocity = 0; //rpm
				steps = init_velocity_profile_params(final_target_velocity, actual_velocity, acceleration, \
							deceleration, slave_number, slv_handles);
				i = 1;
				flag = 1;
			}
			if(i >= steps && flag == 1)
			{
				break;
			}
		}
	}

*/
	/* Shutdown node operations */
	shutdown_operation(CSV, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	return 0;
}


