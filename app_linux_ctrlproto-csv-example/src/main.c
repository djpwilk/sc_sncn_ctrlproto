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
void run_drive();



//void set_controlword(int controlword)
//{
//	slv_handles[0].motorctrl_out = controlword;
//}
//
//int read_statusword()
//{
//	return slv_handles[0].motorctrl_status_in;
//}

void set_velocity(int target_velocity, int slave_number)
{
	slv_handles[slave_number].speed_setpoint = target_velocity;
}
int get_velocity_actual(int slave_number)
{
	return slv_handles[slave_number].speed_in;
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

	int final_target_velocity = -1000;
	int initial_velocity = 0;
	int acceleration= 1000;
	int deceleration = 1000;
	int steps = 0;
	int i = 1;
	int target_velocity = 0;

	int slave_number = 0;
//	int v_d = -1000;
//	int u = 0;
//	int acc= 1000;
//	int dec = 1000;
//	int op_enable_state = 0;
	int status_word = 0;

	init_master(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	set_operation_mode(CSV, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	enable_operation(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	i = 0;
	steps = init_velocity_profile(final_target_velocity, initial_velocity, acceleration, deceleration);
	while(1)
	{
		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

		if(master_setup.op_flag)//Check if we are up
		{
			if(i<steps)
			{
				target_velocity = velocity_profile_generate(i);
				set_velocity(target_velocity, slave_number);
				i = i+1;
			}
			if(i>=steps && flag == 0)
			{
				//printf("done");
				initial_velocity = get_velocity_actual(slave_number);
				final_target_velocity =2000;
				steps = init_velocity_profile(final_target_velocity, initial_velocity, acceleration, deceleration);
				i = 1;
				flag = 1;
			}
			if(i>=steps && flag == 1)
			{
				initial_velocity = get_velocity_actual(slave_number);
				final_target_velocity = -1000;
				steps = init_velocity_profile(final_target_velocity, initial_velocity, acceleration, deceleration);
				i = 1;
				flag = 2;
			}
			if(i >= steps && flag == 2)
			{
				break;
			}
		}
	}

	quick_stop_velocity(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);
//	quick_stop_active = 0;
//	while(!quick_stop_active && master_setup.op_flag)
//	{
//		pdo_handle_ecat(&master_setup,slv_handles, TOTAL_NUM_OF_SLAVES);
//		if(master_setup.op_flag)
//		{
//			set_controlword(ENABLE_OPERATION_CONTROL|(~QUICK_STOP_CONTROL & 0x000F)|ENABLE_VOLTAGE_CONTROL|SWITCH_ON_CONTROL, slave_number, slv_handles);
//			/*************check quick_stop_state**************/
//			status_word = read_statusword(slave_number, slv_handles);
//			quick_stop_active = check_quick_stop_active(status_word);
//		}
//		else
//			continue;
//	}
//
//	#ifndef print_slave
//	printf("quick_stop_active\n");
//	#endif
//
//	ack_stop = 0;
//	while(!ack_stop)
//	{
//		pdo_handle_ecat(&master_setup,slv_handles, TOTAL_NUM_OF_SLAVES);
//		if(master_setup.op_flag)
//		{
//			/*************check quick_stop_state**************/
//			status_word = read_statusword(slave_number, slv_handles);
//			ack_stop = check_target_reached(status_word);
//			//printf("%d\n",quick_stop_active);
//		}
//		else
//			continue;
//	}
//
//	#ifndef print_slave
//	printf("ack stop received \n");
//	#endif

	renable_ctrl(CSV, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	shutdown_operation(CSV, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);
//	/**********************check ready***********************/
//	while(!ready)
//	{
//		pdo_handle_ecat(&master_setup,slv_handles, NUM_SLAVES);
//		if(master_setup.op_flag)
//		{
//			//check ready
//			status_word = read_statusword();
//			ready = check_ready(status_word);
//		}
//		else
//			continue;
//	}
//
//	#ifndef print_slave
//	printf("ready\n");
//	#endif
//
//	/**********************check switch_enable***********************/
//	while(!switch_enable)
//	{
//		pdo_handle_ecat(&master_setup,slv_handles, NUM_SLAVES);
//		if(master_setup.op_flag)
//		{
//			//check switch
//			status_word = read_statusword();
//			switch_enable = check_switch_enable(status_word);
//		}
//		else
//			continue;
//	}
//
//	#ifndef print_slave
//	printf("switch_enable\n");
//	#endif
//
//
//	/************************output switch on**************************/
//	while(!switch_on_state)
//	{
//		pdo_handle_ecat(&master_setup, slv_handles, NUM_SLAVES);
//		if(master_setup.op_flag)
//		{
//			set_controlword(SWITCH_ON_CONTROL);
//			/*************check switch_on_state***************/
//			status_word = read_statusword();
//			switch_on_state = check_switch_on(status_word);
//		}
//		else
//			continue;
//	}
//
//	#ifndef print_slave
//	printf("switch_on_state\n");
//	#endif
//
//	printf("updating parameters\n");
///***** Set up Parameters *****/
//	while(1)
//	{
//		if(slv_handles[0].motor_config_param.update_flag == 1)
//		{
//			slv_handles[0].motor_config_param.update_flag = 0;
//			break;
//		}
//
//		else
//		{
//			sdo_handle_ecat(&master_setup, slv_handles, NUM_SLAVES, MOTOR_PARAM_UPDATE);
//			printf (".");
//			fflush(stdout);
//
//		}
//	}
//
//	while(1)
//	{
//		if(slv_handles[0].motor_config_param.update_flag == 1)
//		{
//			slv_handles[0].motor_config_param.update_flag = 0;
//			break;
//		}
//		else
//		{
//			sdo_handle_ecat(&master_setup, slv_handles, NUM_SLAVES, VELOCITY_CTRL_UPDATE);
//			printf (".");
//		}
//	}
//
//	while(1)
//	{
//		if(slv_handles[0].motor_config_param.update_flag == 1)
//		{
//			slv_handles[0].motor_config_param.update_flag = 0;
//			break;
//		}
//		else
//		{
//			sdo_handle_ecat(&master_setup, slv_handles, NUM_SLAVES, CSV_MOTOR_UPDATE);
//			printf (".");
//		}
//	}
//	/**********************output Mode of Operation******************/
//	while(1)
//	{
//			pdo_handle_ecat(&master_setup,slv_handles, NUM_SLAVES);
//			if(master_setup.op_flag)
//			{
//				slv_handles[0].operation_mode = CSV;
//				/*************check operation_mode display**************/
//				//status_word = read_statusword();
//				//op_enable_state = check_op_enable(status_word);
//				if (slv_handles[0].operation_mode_disp == CSV)
//					break;
//			}
//			else
//				continue;
//	}
//	#ifndef print_slave
//	printf("operation_mode enabled\n");
//	#endif
//
//	/*************************output enable op***********************/
//
//	op_enable_state = 0;
//	while(!op_enable_state && master_setup.op_flag)
//	{
//		pdo_handle_ecat(&master_setup,slv_handles, NUM_SLAVES);
//		if(master_setup.op_flag)
//		{
//			set_controlword(SWITCH_ON);
//			/*************check op_enable_state**************/
//			status_word = read_statusword();
//			op_enable_state = check_op_enable(status_word);
//		}
//		else
//			continue;
//	}
//
//	#ifndef print_slave
//	printf("op_enable_state\n");
//	#endif
//
//	i = 0;
//	steps = init_velocity_profile(v_d, u, acc, dec);
//	while(1)
//	{
//		pdo_handle_ecat(&master_setup,slv_handles, NUM_SLAVES);
//
//		if(master_setup.op_flag)//Check if we are up
//		{
//			if(i<steps)
//			{
//				target_velocity = velocity_profile_generate(i);
//				slv_handles[0].speed_setpoint = target_velocity;
//				i = i+1;
//			}
//			if(i>=steps && flag == 0)
//			{
//				//printf("done");
//				u = slv_handles[0].speed_in; v_d =2000;
//				steps = init_velocity_profile(v_d, u, acc, dec);
//				i = 1;
//				flag = 1;
//			}
//			if(i>=steps && flag == 1)
//			{
//				u = slv_handles[0].speed_in; v_d = -1000;
//				steps = init_velocity_profile(v_d, u, acc, dec);
//				i = 1;
//				flag = 2;
//			}
//			if(i >= steps && flag == 2)
//			{
//				break;
//			}
//		}
//	}
//
//	quick_stop_active = 0;
//	while(!quick_stop_active && master_setup.op_flag)
//	{
//		pdo_handle_ecat(&master_setup,slv_handles, TOTAL_NUM_OF_SLAVES);
//		if(master_setup.op_flag)
//		{
//			set_controlword(ENABLE_OPERATION_CONTROL|(~QUICK_STOP_CONTROL & 0x000F)|ENABLE_VOLTAGE_CONTROL|SWITCH_ON_CONTROL, slave_number, slv_handles);
//			/*************check quick_stop_state**************/
//			status_word = read_statusword(slave_number, slv_handles);
//			quick_stop_active = check_quick_stop_active(status_word);
//		}
//		else
//			continue;
//	}
//
//	#ifndef print_slave
//	printf("quick_stop_active\n");
//	#endif
//
//	ack_stop = 0;
//	while(!ack_stop)
//	{
//		pdo_handle_ecat(&master_setup,slv_handles, TOTAL_NUM_OF_SLAVES);
//		if(master_setup.op_flag)
//		{
//			/*************check quick_stop_state**************/
//			status_word = read_statusword(slave_number, slv_handles);
//			ack_stop = check_target_reached(status_word);
//			//printf("%d\n",quick_stop_active);
//		}
//		else
//			continue;
//	}
//
//	#ifndef print_slave
//	printf("ack stop received \n");
//	#endif
//
//	/**********************output Mode of Operation******************/
//	while(1)
//	{
//			pdo_handle_ecat(&master_setup,slv_handles, TOTAL_NUM_OF_SLAVES);
//			if(master_setup.op_flag)
//			{
//				slv_handles[0].operation_mode = 100;
//				/*************check operation_mode display**************/
//				set_controlword(QUICK_STOP_CONTROL, slave_number, slv_handles);
//				//status_word = read_statusword();
//				//op_enable_state = check_op_enable(status_word);
//				if (slv_handles[0].operation_mode_disp == 100)
//					break;
//			}
//			else
//				continue;
//	}
//	#ifndef print_slave
//	printf("operation_mode reset enabled\n");
//	#endif
//
//	while(1)
//	{
//			pdo_handle_ecat(&master_setup,slv_handles, TOTAL_NUM_OF_SLAVES);
//			if(master_setup.op_flag)
//			{
//				slv_handles[0].operation_mode = CSV;
//				/*************check operation_mode display**************/
//				//status_word = read_statusword();
//				//op_enable_state = check_op_enable(status_word);
//				if (slv_handles[0].operation_mode_disp == CSV)
//					break;
//			}
//			else
//				continue;
//	}
//	#ifndef print_slave
//	printf("operation_mode CSV enabled\n");
//	#endif
//
//	//shudown
//	ack_stop = 0;
//		while(!ack_stop)
//		{
//				pdo_handle_ecat(&master_setup,slv_handles, TOTAL_NUM_OF_SLAVES);
//				if(master_setup.op_flag)
//				{
//					slv_handles[0].operation_mode = 100;
//					/*************check operation_mode display**************/
//					set_controlword(SHUTDOWN, slave_number, slv_handles);
//					status_word = read_statusword(slave_number, slv_handles);
//					ack_stop = check_target_reached(status_word);
//					//op_enable_state = check_op_enable(status_word);
//				}
//				else
//					continue;
//		}
//	#ifndef print_slave
//	printf("shutdown  \n");
//	#endif
//
//
//
//	while(1)
//	{
//			pdo_handle_ecat(&master_setup,slv_handles, TOTAL_NUM_OF_SLAVES);
//			if(master_setup.op_flag)
//			{
//				slv_handles[0].operation_mode = CSV;
//				set_controlword(SWITCH_ON, slave_number, slv_handles);
//				/*************check operation_mode display**************/
//				//status_word = read_statusword();
//				//op_enable_state = check_op_enable(status_word);
//				if (slv_handles[0].operation_mode_disp == CSV)
//					break;
//			}
//			else
//				continue;
//	}
//	#ifndef print_slave
//	printf("operation_mode CSV enabled\n");
//	#endif
//	/*************************output enable op***********************/
//	op_enable_state = 0;
//	while(!op_enable_state && master_setup.op_flag)
//	{
//		pdo_handle_ecat(&master_setup,slv_handles, TOTAL_NUM_OF_SLAVES);
//		if(master_setup.op_flag)
//		{
//			set_controlword(SWITCH_ON, slave_number, slv_handles);
//			/*************check op_enable_state**************/
//			status_word = read_statusword(slave_number, slv_handles);
//			op_enable_state = check_op_enable(status_word);
//		}
//		else
//			continue;
//	}
//
//	#ifndef print_slave
//	printf("op_enable_state\n");
//	#endif
//
//
//	i = 1; u = slv_handles[0].speed_in; v_d = -4000;
//	flag = 0;
//	steps = init_velocity_profile(v_d, u, acc, dec);
//	//printf("%d\n",steps);
//	while(1)
//	{
//		pdo_handle_ecat(&master_setup,slv_handles, TOTAL_NUM_OF_SLAVES);
//
//		if(master_setup.op_flag)//Check if we are up
//		{
//			if(i<steps)
//				{
//
//					target_velocity = velocity_profile_generate(i);
//					slv_handles[0].speed_setpoint = target_velocity;
//					i = i+1;
//				}
//			if(i>=steps && flag == 0)
//			{
//				printf("done1");
//				u = slv_handles[0].speed_in; v_d =2000;
//				steps = init_velocity_profile(v_d, u, acc, dec);
//				i = 1;
//				flag = 1;
//			}
//			if(i>=steps && flag == 1)
//			{
//				printf("done2");
//				u = slv_handles[0].speed_in; v_d = -1000;
//				steps = init_velocity_profile(v_d, u, acc, dec);
//				i = 1;
//				flag = 2;
//			}
//			if(i >= steps && flag == 2)
//			{
//				break;
//			}
//		}
//	}
	return 0;
}


/*
 	if(master_setup.op_flag)//Check if we are up
	{
		slv_handles[0].motorctrl_out = 12;
		slv_handles[0].torque_setpoint = 200;
		slv_handles[0].speed_setpoint = 4000;
		slv_handles[0].position_setpoint = 10000;
		slv_handles[0].operation_mode = 234;

		printf("Status: %i\n",slv_handles[0].motorctrl_status_in);
		printf("Position: %i\n",slv_handles[0].position_in);
		printf("Speed: %i\n",slv_handles[0].speed_in);
		printf("Torque: %i\n",slv_handles[0].torque_in);
		printf("Operation Mode disp: %i\n",slv_handles[0].operation_mode_disp);

	}
	else
	{
		//This is just a example what CAN be done when the master recognizes
		//that, for example, a slave is now missing...
		slv_handles[0].motorctrl_out= 0;
		slv_handles[0].torque_setpoint=0;
		slv_handles[0].speed_setpoint=0;
		slv_handles[0].position_setpoint=0;
		slv_handles[0].operation_mode=0;
	}
*/
