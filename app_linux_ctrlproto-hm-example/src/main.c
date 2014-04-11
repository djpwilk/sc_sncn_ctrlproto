
/**
 * \file main.c
 * \brief Example Master App for homing mode (on PC)
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

	int acceleration = 350;				// rpm/s
	int deceleration = 350;   			// rpm/s
	int velocity = 350;					// rpm
	int actual_position = 0;			// ticks
	int target_position = 0;			// ticks
	int actual_velocity;				// rpm
	float actual_torque;				// mNm
	int steps = 0;
	int i = 1;
	int position_ramp = 0;
	int ack;
	int home_velocity = 250;      		// rpm
	int home_acceleration = 250;		// rpm/s

	int slave_number = 0;

	/* Initialize Ethercat Master */
	init_master(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	/* Initialize torque parameters */
	initialize_torque(slave_number, slv_handles);

	/* Initialize all connected nodes with Mandatory Motor Configurations (specified under config/motor/)*/
	init_nodes(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	/* Initialize the node specified with slave_number with Homing configurations (specified under config/motor/)*/
	set_operation_mode(HM, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	/* Enable operation of node in Homing mode */
	enable_operation(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	start_homing(&master_setup, slv_handles, home_velocity, home_acceleration, slave_number, TOTAL_NUM_OF_SLAVES);

	/* Shutdown Homing Operation */
	shutdown_operation(HM, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	/*
	 * Acquire actual position from the node a few times after homing and
	 * set it as target position. (wait for controller to settle)
	 */
	i = 0;
	int difference = 1500;
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
				break;
			}
			previous = actual_position;
		}

	}


	/* Now initialize the node specified with slave_number with CSP configurations (specified under config/motor/)*/
	set_operation_mode(CSP, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	/* Enable operation of node in CSP mode */
	enable_operation(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	/* Initialize position profile parameters */
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
		/* Update the process data (EtherCat packets) sent/received from the node */
		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

		if(master_setup.op_flag)	/*Check if the master is active*/
		{
			if(i<steps)
			{
				/* Generate target position steps */
				position_ramp = generate_profile_position(i, slave_number, slv_handles);

				/* Send target position for the node specified by slave_number */
				set_position_ticks(position_ramp, slave_number, slv_handles);
				i = i+1;
			}
			if(i >= steps)
			{
				printf("ack received");
				break;
			}


			/* Read actual node sensor values */
			actual_position = get_position_actual_ticks(slave_number, slv_handles);
			actual_velocity = get_velocity_actual_rpm(slave_number, slv_handles);
			actual_torque = get_torque_actual_mNm(slave_number, slv_handles);
			printf("actual position %d actual velocity %d actual_torque %f\n", actual_position, actual_velocity, actual_torque);
		}
	}

	/* Quick stop position mode (for emergency) */
	quick_stop_position(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	/* Regain control of node to continue after quick stop */
	renable_ctrl_quick_stop(CSP, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	set_operation_mode(CSP, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);			// set operation mode to CSP

	enable_operation(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);				// enable operation

	/* Shutdown node operations */
	shutdown_operation(CSP, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);			// stop the node operation. or can continue with new position profile

	return 0;
}

