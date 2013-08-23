/*
 * motor_define.c
 *
 *  Created on: Aug 22, 2013
 *      Author: pkanajar
 */
#include <motor_define.h>
#include "bldc_motor_config.h"
#include <stdio.h>

motor_config init_motor_config()
{
	motor_config motor_config_param;

	motor_config_param.s_gear_ratio.gear_ratio 										= GEAR_RATIO;
	motor_config_param.s_gear_ratio.update_state 									= 0;

	motor_config_param.s_max_acceleration.max_acceleration 							= MAX_ACCELERATION;
	motor_config_param.s_max_acceleration.update_state 								= 0;

	motor_config_param.s_nominal_current.nominal_current 							= MAX_NOMINAL_CURRENT;
	motor_config_param.s_nominal_current.update_state 								= 0;

	motor_config_param.s_nominal_motor_speed.nominal_motor_speed 					= MAX_NOMINAL_SPEED;
	motor_config_param.s_nominal_motor_speed.update_state 							= 0;

	motor_config_param.s_polarity.polarity 											= POLARITY;
	motor_config_param.s_polarity.update_state 										= 0;

	motor_config_param.s_pole_pair.pole_pair 										= POLE_PAIRS;
	motor_config_param.s_pole_pair.update_state 									= 0;

	motor_config_param.s_position_encoder_resolution.position_encoder_resolution	= ENCODER_RESOLUTION;
	motor_config_param.s_position_encoder_resolution.update_state 					= 0;

//	motor_config_param.s_sensor_selection_code.sensor_selection_code =
//	motor_config_param.s_sensor_selection_code.update_state = 0;
//	motor_config_param.s_velocity_d_gain.velocity_d_gain =
//	motor_config_param.s_velocity_d_gain.update_state = 0;
//	motor_config_param.s_velocity_i_gain.velocity_i_gain =
//	motor_config_param.s_velocity_i_gain.update_state = 0;
//	motor_config_param.s_velocity_p_gain.velocity_p_gain =
//	motor_config_param.s_velocity_p_gain.update_state = 0;
	motor_config_param.update_flag = 0;
	return motor_config_param;
}
