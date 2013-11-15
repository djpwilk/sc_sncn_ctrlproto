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
	float target_torque = -25.0; //mNm
	float actual_torque = 0;
	float tolerance = 0.76; //mNm
	int ack = 0;

	int slave_number = 0;

	init_master(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	init_node(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	initialize_torque(slave_number, slv_handles);

	set_operation_mode(TQ, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	enable_operation(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);


	while(1)
	{
		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

		if(master_setup.op_flag)//Check if we are up
		{

			set_torque_mNm(target_torque, slave_number, slv_handles);
			ack = target_torque_reached(slave_number, target_torque, tolerance, slv_handles);
			actual_torque= get_torque_actual_mNm(slave_number, slv_handles);
			printf("target_torque %f \n",target_torque);
			printf("actual_torque %f ack %d\n", actual_torque, ack);
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
			actual_torque =  get_torque_actual_mNm(slave_number, slv_handles);
			if(actual_torque > tolerance || actual_torque < -tolerance)
			{
				quick_stop_torque(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);
				ack = 1;
			}
			printf("actual_torque %f ack %d\n", actual_torque, ack);
		}
	}
	printf("reached \n");

	renable_ctrl_quick_stop(TQ, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	set_operation_mode(TQ, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	enable_operation(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	target_torque = 15.0; //mNm
	while(1)
	{
		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

		if(master_setup.op_flag)//Check if we are up
		{

			set_torque_mNm(target_torque, slave_number, slv_handles);
			ack = target_torque_reached(slave_number, target_torque, tolerance, slv_handles);
			actual_torque = get_torque_actual_mNm(slave_number, slv_handles);
			printf("target_torque %f \n",target_torque);
			printf("actual_torque %f ack %d\n", actual_torque, ack);
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

			actual_torque = get_torque_actual_mNm(slave_number, slv_handles);
			printf("actual_torque %f \n",actual_torque);
		}
	}



	//shutdown_operation(TQ, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	return 0;
}


