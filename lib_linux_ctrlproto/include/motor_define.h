#ifndef MOTOR_DEFINE_H_
#define MOTOR_DEFINE_H_
#include "bldc_motor_config_1.h"
#include "bldc_motor_config_2.h"

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct
	{
		int pole_pair;
		int update_state;
	} pole_pair_s;

typedef struct
{
	int gear_ratio;
	int update_state;
} gear_ratio_s;

typedef struct
{
	int nominal_motor_speed;
	int update_state;
} nominal_motor_speed_s;

typedef struct
{
	int nominal_current;
	int update_state;
} nominal_current_s;

typedef struct
{
	int max_acceleration;
	int update_state;
} max_acceleration_s;

typedef struct
{
	int position_encoder_resolution;
	int update_state;
} position_encoder_resolution_s;

typedef struct
{
	int polarity;
	int update_state;
} polarity_s;

typedef struct
{
	int sensor_selection_code;
	int update_state;
} sensor_selection_code_s;

typedef struct
{
	int velocity_p_gain;
	int update_state;
} velocity_p_gain_s;

typedef struct
{
	int velocity_i_gain;
	int update_state;
} velocity_i_gain_s;

typedef struct
{
	int velocity_d_gain;
	int update_state;
} velocity_d_gain_s;

typedef struct
{
	pole_pair_s s_pole_pair;
	gear_ratio_s s_gear_ratio;
	nominal_motor_speed_s s_nominal_motor_speed;
	nominal_current_s s_nominal_current;
	max_acceleration_s s_max_acceleration;
	position_encoder_resolution_s s_position_encoder_resolution; //qei

	polarity_s s_polarity;
	sensor_selection_code_s s_sensor_selection_code; //hall/qei/...

	velocity_p_gain_s s_velocity_p_gain;
	velocity_i_gain_s s_velocity_i_gain;
	velocity_d_gain_s s_velocity_d_gain;

	int update_flag;
} motor_config;

#define GEAR_RATIO(n) 				GEAR_RATIO_##n
#define MAX_ACCELERATION(n) 		MAX_ACCELERATION_##n
#define MAX_NOMINAL_CURRENT(n) 		MAX_NOMINAL_CURRENT_##n
#define MAX_NOMINAL_SPEED(n) 		MAX_NOMINAL_SPEED_##n
#define POLARITY(n) 				POLARITY_##n
#define POLE_PAIRS(n) 				POLE_PAIRS_##n
#define ENCODER_RESOLUTION(n) 		ENCODER_RESOLUTION_##n
#define VELOCITY_Kp_NUMERATOR(n)   	VELOCITY_Kp_NUMERATOR_##n
#define VELOCITY_Kp_DENOMINATOR(n)  VELOCITY_Kp_DENOMINATOR_##n
#define VELOCITY_Ki_NUMERATOR(n)   	VELOCITY_Ki_NUMERATOR_##n
#define VELOCITY_Ki_DENOMINATOR(n)  VELOCITY_Ki_DENOMINATOR_##n
#define VELOCITY_Kd_NUMERATOR(n)  	VELOCITY_Kd_NUMERATOR_##n
#define VELOCITY_Kd_DENOMINATOR(n)  VELOCITY_Kd_DENOMINATOR_##n
#define SENSOR_SELECTION_CODE(n) 	SENSOR_SELECTION_CODE_##n

#ifdef __cplusplus
}
#endif

#endif
