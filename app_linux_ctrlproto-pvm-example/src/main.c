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


