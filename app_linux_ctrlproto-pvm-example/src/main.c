
/**
 *
 * \file main.c
 *
 * \brief Example Master App for Profile Velocity (on PC)
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

//#define print_slave


int main()
{
	int flag = 0;

	int actual_velocity = 0;	//rpm
	int target_velocity = 4000;	//rpm
	int tolerance = 20; 		//rpm


	int flag_velocity_set = 0;

	int slave_number = 0;
	int ack = 0;

	init_master(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	init_node(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	set_operation_mode(PV, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	enable_operation(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	while(1)
	{

		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

		if(master_setup.op_flag)//Check if we are up
		{
			set_velocity_rpm(target_velocity, slave_number, slv_handles);
			ack = target_velocity_reached(slave_number, target_velocity, tolerance, slv_handles);
			actual_velocity =  get_velocity_actual_rpm(slave_number, slv_handles);
			printf("velocity %d ack %d\n", actual_velocity, ack);
		}
		if(ack == 1)
		{
			break;
		}
	}

	printf("reached \n");

	ack = 0;
	while(!ack)
	{
		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);
		if(master_setup.op_flag)//Check if we are up
		{
			actual_velocity =  get_velocity_actual_rpm(slave_number, slv_handles);
			if(actual_velocity > 0 || actual_velocity < 0)
			{
				quick_stop_velocity(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);
				ack = 1;
			}
			printf("velocity %d ack %d\n", actual_velocity, ack);
		}
	}
	printf("reached \n");

//	while(1)
//	{
//		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);
//		if(master_setup.op_flag)//Check if we are up
//		{
//			actual_velocity =  get_velocity_actual_rpm(slave_number, slv_handles);
//			printf("velocity %d \n", actual_velocity);
//		}
//	}


	renable_ctrl_quick_stop(PV, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	set_operation_mode(PV, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	enable_operation(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	//shutdown_operation(PV, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	target_velocity = -300;
	while(1)
	{

		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

		if(master_setup.op_flag)//Check if we are up
		{
			set_velocity_rpm(target_velocity, slave_number, slv_handles);
			ack = target_velocity_reached(slave_number, target_velocity, tolerance, slv_handles);
			actual_velocity =  get_velocity_actual_rpm(slave_number, slv_handles);
			printf("velocity %d ack %d\n", actual_velocity, ack);
		}
		if(ack == 1)
		{
			break;
		}
	}

	printf("reached \n");
	while(1)
	{
		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);
		if(master_setup.op_flag)//Check if we are up
		{
			actual_velocity =  get_velocity_actual_rpm(slave_number, slv_handles);
			printf("velocity %d \n", actual_velocity);
		}
	}
	return 0;
}


