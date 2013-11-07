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
	int flag_position_set = 0;

	float actual_position = 0;			// degree
	float target_position = -350.0f;	// degree

	float tolerance = .1f;	 			// 0.1 degree

	int slave_number = 0;
	int ack = 0;
	printf(" gear %d", slv_handles[slave_number].motor_config_param.s_gear_ratio.gear_ratio);

	init_master(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	init_node(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	set_operation_mode(PP, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	enable_operation(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);


	ack = 0;
	target_position = 350.0f;
	while(1)
	{
		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

		if(master_setup.op_flag)//Check if we are up
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

		if(master_setup.op_flag)//Check if we are up
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


//	while(1)
//	{
//		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);
//		if(master_setup.op_flag)//Check if we are up
//		{
//			actual_position = get_position_actual_degree(slave_number, slv_handles);
//			printf("position %f \n", actual_position);
//		}
//	}
	renable_ctrl_quick_stop(PP, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	set_operation_mode(PP, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	enable_operation(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

//	shutdown_operation(PP, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);


	target_position = 300.0f;
	while(1)
	{
		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

		if(master_setup.op_flag)//Check if we are up
		{
			set_profile_position_degree(target_position, slave_number, slv_handles);
			actual_position = get_position_actual_degree(slave_number, slv_handles);
			ack = target_position_reached(slave_number, target_position, tolerance, slv_handles);
			printf("position %f ack %d\n", actual_position, ack);
		}
		if(ack == 1)
		{
			break;
		}

	}

	printf("reached \n");

	//shutdown_operation(PP, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);//*/

	return 0;
}

