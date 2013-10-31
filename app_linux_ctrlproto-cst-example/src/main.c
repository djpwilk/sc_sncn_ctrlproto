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
	float final_target_torque = 28.0;			//mNm
	float initial_torque = 0;					//mNm
	float torque_slope = 10.0;					//mNm/s
	int steps = 0;
	int i = 1;

	float target_torque = 0.0;
	float actual_torque = 0.0;

	int slave_number = 0;

	init_master(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	initialize_torque(slave_number, slv_handles);

	set_operation_mode(CST, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	enable_operation(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	i = 0;
	steps = init_linear_profile_params(final_target_torque, actual_torque, torque_slope, slave_number, slv_handles);
	printf("steps %d \n", steps);
	while(1)
	{
		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

		if(master_setup.op_flag)//Check if we are up
		{
			if(i<steps)
			{
				target_torque = linear_profile_generate_float(i);
				printf("target_torque %f \n",target_torque);
				set_torque_mNm(target_torque, slave_number, slv_handles);
				actual_torque= get_torque_actual_mNm(slave_number, slv_handles);
				printf("actual_torque %f \n",actual_torque);
				i = i+1;
			}

			if(i >= steps)
			{
				break;
			}
		}
	}


	i = 0;
	final_target_torque = 18.0;
	steps = init_linear_profile_params(final_target_torque, actual_torque, torque_slope, slave_number, slv_handles);
	printf("steps %d \n", steps);
	while(1)
	{
		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

		if(master_setup.op_flag)//Check if we are up
		{
			if(i<steps)
			{
				target_torque = linear_profile_generate_float(i);
				printf("target_torque %f \n",target_torque);
				set_torque_mNm(target_torque, slave_number, slv_handles);
				actual_torque= get_torque_actual_mNm(slave_number, slv_handles);
				printf("actual_torque %f \n",actual_torque);
				i = i+1;
			}

			if(i >= steps)
			{
				break;
			}
		}
	}

	quick_stop_torque(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);




	/*while(1)
	{
		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

		if(master_setup.op_flag)//Check if we are up
		{

			actual_torque = get_torque_actual_mNm(slave_number, slv_handles);
			printf("actual_torque %f \n",actual_torque);
		}
	}*/

	renable_ctrl_quick_stop(CST, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	set_operation_mode(CST, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	enable_operation(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);


	i = 0;
	final_target_torque = 0.0;
	actual_torque= 	get_torque_actual_mNm(slave_number, slv_handles);

	steps = init_linear_profile_params(final_target_torque, actual_torque, torque_slope, slave_number, slv_handles);
	printf("steps %d \n", steps);
	while(1)
	{
		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

		if(master_setup.op_flag)//Check if we are up
		{
			if(i<steps)
			{
				target_torque = linear_profile_generate_float(i);
				printf("target_torque %f \n",target_torque);
				set_torque_mNm(target_torque, slave_number, slv_handles);
				actual_torque= get_torque_actual_mNm(slave_number, slv_handles);
				printf("actual_torque %f \n",actual_torque);
				i = i+1;
			}
			if(i >= steps)
			{
				break;
			}
		}
	}
	/*while(1)
	{
		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

		if(master_setup.op_flag)//Check if we are up
		{

			actual_torque = get_torque_actual_mNm(slave_number, slv_handles);
			printf("actual_torque %f \n",actual_torque);
		}
	}*/



	shutdown_operation(CST, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	return 0;
}


