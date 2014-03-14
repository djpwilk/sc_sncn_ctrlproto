
/**
 *
 * \file main.c
 *
 * \brief Example Master App for Cyclic Synchronous Position (on PC)
 *
 *
 *
 * Copyright (c) 2013, Synapticon GmbH
 * All rights reserved.
 * Author: Pavan Kanajar <pkanajar@synapticon.com> & Christian Holl <choll@synapticon.com>
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

	int acceleration = 350;				// rpm/s
	int deceleration = 350;   			// rpm/s
	int velocity = 350;					// rpm
	int actual_position = 0;			// ticks
	int target_position = 0;			// ticks
	int steps = 0;
	int i = 1;
	int position_ramp = 0;
	int status_word;
	int ack;
	int home_velocity = 250;      		// rpm
	int home_acceleration = 250;		// rpm/s

	int slave_number = 0;


	init_master(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	init_nodes(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	/* Set operation mode to Homing */
	set_operation_mode(HM, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	/* Enable Homing Operation */
	enable_operation(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	start_homing(&master_setup, slv_handles, home_velocity, home_acceleration, slave_number, TOTAL_NUM_OF_SLAVES);

	/* Shutdown Homing Operation */
	shutdown_operation(HM, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	/*
	 * Acquire actual position from the node a few times after homing and
	 * set it as target position to avoid setting controller target position to 0 in one step
	 */
	i = 0;
	int difference =1500;
	int previous=0;
	while(1)
	{

		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

		if(master_setup.op_flag)	//Check if the master is active
		{
			actual_position = get_position_actual_ticks(slave_number, slv_handles);
			target_position = actual_position;
			set_profile_position_ticks(target_position, slave_number, slv_handles);
			i = i+1;

			difference = actual_position - previous;
			if(difference <1 && difference >-1)
			{
				//printf(" difference %d act %d\n",difference, actual_position);
				break;
			}

			previous = actual_position;

			//printf(" difference %d act %d\n",difference, actual_position);
		}

	}

	set_operation_mode(CSP, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	enable_operation(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);


	initialize_position_profile_limits(slave_number, slv_handles);//*/



	/* Now setting a new target position after homing */
	target_position = get_position_actual_ticks(slave_number, slv_handles) + 5000;
	if(target_position > 35000)
		target_position = 35000;
	printf(" target_position %d\n", target_position);

	/*calculate the no. of steps for the profile*/
	steps = init_position_profile_params(target_position, actual_position, velocity, acceleration, \
			deceleration, slave_number, slv_handles);

	/* Execute the position profile steps in a loop */
	i = 1;
	while(1)
	{

		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

		if(master_setup.op_flag)	//Check if the master is active
		{
			if(i<steps)
			{
				position_ramp = generate_profile_position(i, slave_number, slv_handles);
				set_position_ticks(position_ramp, slave_number, slv_handles);
				i = i+1;
			}
			if(i >= steps)
			{
				printf("ack received");
				break;
			}
			//printf("actual position %d \n", get_position_actual_ticks(slave_number, slv_handles));
		}
	}


	quick_stop_position(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);				// use quick stop in case of emergency

	renable_ctrl_quick_stop(CSP, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES); 	// after quick-stop enable control of node

	set_operation_mode(CSP, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);			// set operation mode to CSP

	enable_operation(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);				// enable operation

	shutdown_operation(CSP, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);			// stop the node operation. or can continue with new position profile




	return 0;
}

