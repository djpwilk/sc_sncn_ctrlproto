#ifndef _MOTOR_1
#define _MOTOR_1
#include <common_config.h>
/*
 * define Motor Specific Constants
 */
#define POLE_PAIRS_1  					8
#define GEAR_RATIO_1 					26
#define MAX_NOMINAL_SPEED_1  			4000		// in rpm
#define MAX_NOMINAL_CURRENT_1  			2		// in A
#define MAX_ACCELERATION_1   			5000     // rpm/s
#define ENCODER_RESOLUTION_1 			4000		// Max count of Quadrature Encoder
#define MOTOR_TORQUE_CONSTANT_1			33					// mNm/A
#define POLARITY_1 						1					// 1 / -1

/* Somanet IFM Internal Config */
#define DC100_RESOLUTION 				740 				// resolution/A
#define DC900_RESOLUTION				264 				// resolution/A
#define IFM_RESOLUTION_1				DC900_RESOLUTION

/* Profile defines */
#define MAX_PROFILE_VELOCITY_1  		MAX_NOMINAL_SPEED_1
#define PROFILE_VELOCITY_1				1001
#define PROFILE_ACCELERATION_1			2002
#define PROFILE_DECELERATION_1  		2004
#define QUICK_STOP_DECELERATION_1 		2005
#define MAX_TORQUE_1					MOTOR_TORQUE_CONSTANT_1 * IFM_RESOLUTION_1 * MAX_NOMINAL_CURRENT_1
#define TORQUE_SLOPE_1 					6000 				// mNm/s * IFM_RESOLUTION

/*config*/
#define QEI_OFFSET_CLK_1 				184
#define QEI_OFFSET_CCLK_1 				104
#define QEI_COMMUTATION_OFFSET_CLK_1 	682
#define QEI_COMMUTATION_OFFSET_CCLK_1 	2773

/*Position Sensor Types*/
#define SENSOR_SELECTION_CODE_1    		QEI_INDEX

/* control specific constants/variables */
	/*Torque Control*/
#define TORQUE_Kp_NUMERATOR_1 	   		50
#define TORQUE_Kp_DENOMINATOR_1  		10
#define TORQUE_Ki_NUMERATOR_1    		11
#define TORQUE_Ki_DENOMINATOR_1  		110
#define TORQUE_Kd_NUMERATOR_1    		1
#define TORQUE_Kd_DENOMINATOR_1  		10

	/*Velocity Control*/
#define VELOCITY_Kp_NUMERATOR_1 		5
#define VELOCITY_Kp_DENOMINATOR_1  		10
#define VELOCITY_Ki_NUMERATOR_1    		5
#define VELOCITY_Ki_DENOMINATOR_1  		100
#define VELOCITY_Kd_NUMERATOR_1   		0
#define VELOCITY_Kd_DENOMINATOR_1 		1

	/*Position Control*/
#define MAX_POSITION_LIMIT_1 			359
#define MIN_POSITION_LIMIT_1 			-359

#define POSITION_Kp_NUMERATOR_1 		180
#define POSITION_Kp_DENOMINATOR_1  		2000
#define POSITION_Ki_NUMERATOR_1    		50
#define POSITION_Ki_DENOMINATOR_1  		102000
#define POSITION_Kd_NUMERATOR_1    		100
#define POSITION_Kd_DENOMINATOR_1  		10000

#endif
