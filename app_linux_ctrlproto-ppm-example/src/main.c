
/**
 *
 * \file main.c
 *
 * \brief Example Master App for Profile Position (on PC)
 *
 *
 *
 * Copyright (c) 2014, Synapticon GmbH
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
	int flag_position_set = 0;

	float actual_position = 0;			// degree
	float target_position = 350.0f;		// degree

	float tolerance = 1.0f;	 			// 1 degree

	int slave_number = 0;
	int ack = 0;

	init_master(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	init_node(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	set_operation_mode(PP, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	enable_operation(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);


	ack = 0;
	while(1)
	{
		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

		if(master_setup.op_flag)	// Check if the master is active
		{
			set_profile_position_degree(target_position, slave_number, slv_handles);
			ack = target_position_reached(slave_number, target_position, tolerance, slv_handles);
			actual_position = get_position_actual_degree(slave_number, slv_handles);
			printf("position %f ack %d\n", actual_position, ack);
		}
		if(ack == 1)
		{
			break;
		}
	}

	printf("reached \n");

	flag_position_set = 0;
	target_position = 30.0f;
	while(1)
	{
		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

		if(master_setup.op_flag)	// Check if the master is active
		{
			set_profile_position_degree(target_position, slave_number, slv_handles);
			flag_position_set = position_set_flag(slave_number, slv_handles); 		//ensures the new way point is taken awhen ack = 0;
			actual_position = get_position_actual_degree(slave_number, slv_handles);

			printf("position %f ack %d   position_set_flag %d\n", actual_position, ack , flag_position_set);
		}
		if(flag_position_set == 1)
		{
			printf("\nexecuting");
			break;
		}
	}

	ack = 0;
	while(!ack)
	{
		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);
		if(master_setup.op_flag)//Check if we are up
		{
			actual_position = get_position_actual_degree(slave_number, slv_handles);
			if(actual_position < 50.0f)
			{
				quick_stop_position(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);
				ack = 1;
			}
			printf("position %f ack %d\n", actual_position, ack);
		}
	}
	printf("reached \n");


	renable_ctrl_quick_stop(PP, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	set_operation_mode(PP, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	enable_operation(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	shutdown_operation(PP, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	return 0;
}

