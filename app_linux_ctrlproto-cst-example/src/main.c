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
{//float
	int final_target_torque = -3960;			//mNm
	int initial_torque = 0;					//mNm
	int acceleration = 1000;				//mNm/s^2
	int deceleration = 1000;				//mNm/s^2
	int steps = 0;
	int i = 1;

	int target_torque = 0;
	int actual_torque = 0;

	int slave_number = 0;
	int status_word = 0;

	init_master(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	//void initial_torque(slave_number, slv_handles)

	set_operation_mode(CST, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	enable_operation(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	i = 0;
	steps = init_linear_profile(final_target_torque, initial_torque, acceleration, deceleration, 17800); //max_torque
	printf("steps %d \n", steps);
	while(1)
	{
		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

		if(master_setup.op_flag)//Check if we are up
		{
			if(i<steps)
			{
				target_torque = linear_profile_generate(i);
				set_torque(target_torque, slave_number, slv_handles);
				actual_torque= get_torque_actual(slave_number, slv_handles);
				printf("torque %d \n",actual_torque);
				i = i+1;
			}

			if(i >= steps)
			{
				break;
			}
		}
	}

	quick_stop_torque(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);


	renable_ctrl_quick_stop(CST, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	/*while(1)
	{
		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

		if(master_setup.op_flag)//Check if we are up
		{

			actual_torque= get_torque_actual(slave_number, slv_handles);
			printf("torque %d \n",actual_torque);

		}
	}*/
	set_operation_mode(CST, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	enable_operation(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);


	i = 0;
	final_target_torque = -3960;
	actual_torque= get_torque_actual(slave_number, slv_handles);
	steps = init_linear_profile(final_target_torque, 0, acceleration, deceleration, 17800); //max_torque
	printf("steps %d \n", steps);
	while(1)
	{
		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

		if(master_setup.op_flag)//Check if we are up
		{
			if(i<steps)
			{
				target_torque = linear_profile_generate(i);
				set_torque(target_torque, slave_number, slv_handles);
				actual_torque= get_torque_actual(slave_number, slv_handles);
				printf("torque %d \n",target_torque);
				i = i+1;
			}
			if(i >= steps)
			{
				break;
			}
		}
	}
	/*i = 0;
	flag = 0;
	final_target_velocity = 1000; //rpm
	initial_velocity = get_velocity_actual(slave_number, slv_handles); //rpm
	steps = init_velocity_profile(final_target_velocity, initial_velocity, acceleration, deceleration, MAX_PROFILE_VELOCITY(1));

	while(1)
	{
		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

		if(master_setup.op_flag)//Check if we are up
		{
			if(i<steps)
			{
				target_velocity = velocity_profile_generate(i);
				set_velocity(target_velocity, slave_number, slv_handles);
				actual_velocity = get_velocity_actual(slave_number, slv_handles);
				printf("velocity %d \n",actual_velocity);
				i = i+1;
			}
			if(i>=steps && flag == 0)
			{
				initial_velocity = get_velocity_actual(slave_number, slv_handles);
				final_target_velocity = 0; //rpm
				steps = init_velocity_profile(final_target_velocity, initial_velocity, acceleration, deceleration, MAX_PROFILE_VELOCITY(1));
				i = 1;
				flag = 1;
			}
			if(i >= steps && flag == 1)
			{
				break;
			}
		}
	}

	shutdown_operation(CSV, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);
*/
	return 0;
}


