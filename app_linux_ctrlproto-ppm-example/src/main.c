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

void set_velocity(int target_velocity, int slave_number)
{
	slv_handles[slave_number].speed_setpoint = target_velocity;
}
int get_velocity_actual(int slave_number)
{
	return slv_handles[slave_number].speed_in;
}
int get_position_actual_deg(int slave_number)
{
	return slv_handles[slave_number].position_in/10000;
}
void set_position_deg(int target_position, int slave_number)
{
	slv_handles[slave_number].position_setpoint = target_position;
}
int main()
{
//	int ready = 0;
//	int switch_enable = 0;
//	int status_word = 0;
//	int switch_on_state = 0;
//	int op_enable_state = 0;
	int quick_stop_active = 0;
	int ack_stop = 0;
	int control_word;
	int flag = 0;

	int acc = 350;				//rpm/s
	int dec = 350;   			//rpm/s
	int velocity =200;			//rpm
	int actual_position = 0;	//degree
	int target_position = 350;	//degree
	int steps = 0, i = 1;
	int position_ramp = 0;


	int slave_number = 0;


	int op_enable_state = 0;
	int status_word = 0;
	printf(" %d %d %d %d %d ", slv_handles[0].motor_config_param.s_max_profile_velocity.max_profile_velocity, slv_handles[0].motor_config_param.s_profile_velocity.profile_velocity,  \
	 				slv_handles[0].motor_config_param.s_profile_acceleration.profile_acceleration, slv_handles[0].motor_config_param.s_profile_deceleration.profile_deceleration,\
	 				slv_handles[0].motor_config_param.s_quick_stop_deceleration.quick_stop_deceleration);

	init_master(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	set_operation_mode(PP, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

//	enable_operation(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);
//
//
//	init_position_profile_limits(GEAR_RATIO_1, MAX_ACCELERATION_1, MAX_NOMINAL_SPEED_1);
//	steps = init_position_profile(target_position, actual_position,	velocity, acc, dec);
//
//
//	while(1)
//	{
//
//		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);
//
//		if(master_setup.op_flag)//Check if we are up
//		{
//
//			if(i<steps && flag == 0)
//			{
//				position_ramp = position_profile_generate(i);
//				set_position_deg(position_ramp, slave_number);
//				i = i+1;
//			}
//
//			if(i<steps - steps/2&& flag == 1)
//			{
//				position_ramp = position_profile_generate(i);
//
//				set_position_deg(position_ramp, slave_number);
//				i = i+1;
//			}
//			else if(flag == 1 && i >=steps-steps/2)//i >= steps )//&& flag == 2)
//			{
//							break;
//			}
//			if(i>=steps && flag == 0)
//			{
//				//printf("done");
//				actual_position = get_position_actual_deg(slave_number);
//				target_position = -100; velocity = 350; acc = 350; dec = 350;
//				steps = init_position_profile(target_position, actual_position,	velocity, acc, dec);
//				i = 1;
//				flag = 1;
//			}
//			/*if(i>=steps && flag == 1)
//			{
//				actual_position = get_position_actual_deg(slave_number);
//				target_position = 30; velocity = 3500; acc = 350; dec = 350;
//				steps = init_position_profile(target_position, actual_position,	velocity, acc, dec);
//				i = 1;
//				flag = 2;
//			}*/
////			else if(flag ==1)//i >= steps )//&& flag == 2)
////			{
////				break;
////			}
//		}
//	}// while

//	quick_stop_position(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);
////	quick_stop_active = 0;
////	while(!quick_stop_active && master_setup.op_flag)
////	{
////		pdo_handle_ecat(&master_setup,slv_handles, TOTAL_NUM_OF_SLAVES);
////		if(master_setup.op_flag)
////		{
////			set_controlword(ENABLE_OPERATION_CONTROL|(~QUICK_STOP_CONTROL & 0x000F)|ENABLE_VOLTAGE_CONTROL|SWITCH_ON_CONTROL, slave_number, slv_handles);
////			/*************check quick_stop_state**************/
////			status_word = read_statusword(slave_number, slv_handles);
////			quick_stop_active = check_quick_stop_active(status_word);
////		}
////		else
////			continue;
////	}
////#ifndef print_slave
////	printf("quick_stop_active\n");
////#endif
////
////		ack_stop = 0;
////		while(!ack_stop)
////		{
////			pdo_handle_ecat(&master_setup,slv_handles, TOTAL_NUM_OF_SLAVES);
////			if(master_setup.op_flag)
////			{
////				/*************check quick_stop_state**************/
////				status_word = read_statusword(slave_number, slv_handles);
////				ack_stop = check_target_reached(status_word);
////				//printf("%d\n",quick_stop_active);
////			}
////			else
////				continue;
////		}
////
////		#ifndef print_slave
////		printf("ack stop received \n");
////		#endif
//		renable_ctrl(CSP, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);
////		/**********************output Mode of Operation******************/
////		while(1)
////		{
////				pdo_handle_ecat(&master_setup,slv_handles, TOTAL_NUM_OF_SLAVES);
////				if(master_setup.op_flag)
////				{
////					slv_handles[0].operation_mode = 100;
////					/*************check operation_mode display**************/
////					set_controlword(QUICK_STOP_CONTROL, slave_number, slv_handles);
////					//status_word = read_statusword();
////					//op_enable_state = check_op_enable(status_word);
////					if (slv_handles[0].operation_mode_disp == 100)
////						break;
////				}
////				else
////					continue;
////		}
////		#ifndef print_slave
////		printf("operation_mode reset enabled\n");
////		#endif
////
////		/**********************output Mode of Operation******************/
////		while(1)
////		{
////				pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);
////				if(master_setup.op_flag)
////				{
////					slv_handles[slave_number].operation_mode = CSP;
////					/*************check operation_mode display**************/
////					if (slv_handles[slave_number].operation_mode_disp == CSP)
////						break;
////				}
////				else
////					continue;
////		}
////		#ifndef print_slave
////		printf("operation_mode enabled\n");
////		#endif
//
//		enable_operation(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);
//
//
//		actual_position = get_position_actual_deg(slave_number);
//		target_position = actual_position+200; velocity = 350; acc = 350; dec = 350;
//
//		steps = init_position_profile(target_position, actual_position,	velocity, acc, dec);
//		i = 0;
//printf("\nsteps %d\n", steps);
//		while(1)
//		{
//			pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);
//
//
//			if(master_setup.op_flag)//Check if we are up
//			{
//
//				if(i<steps)
//				{
//					position_ramp = position_profile_generate(i);
//					set_position_deg(position_ramp, slave_number);
//					i = i+1;
//				}
//				else if(i>=steps)
//					break;
//			}
//		}// while
//
//
//	//shutdown
//	shutdown_operation(CSP, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);
//	//.....
////	ack_stop = 0;
////	while(!ack_stop)
////	{
////			pdo_handle_ecat(&master_setup,slv_handles, TOTAL_NUM_OF_SLAVES);
////			if(master_setup.op_flag)
////			{
////				//slv_handles[0].operation_mode = 100;
////				/*************check operation_mode display**************/
////				set_controlword(SHUTDOWN, slave_number, slv_handles);
////				status_word = read_statusword(slave_number, slv_handles);
////				ack_stop = check_target_reached(status_word);
////				//op_enable_state = check_op_enable(status_word);
////			}
////			else
////				continue;
////	}
////
////	#ifndef print_slave
////	printf("shutdown  \n");
////	fflush(stdout);
////	#endif
////	}


	return 0;
}


