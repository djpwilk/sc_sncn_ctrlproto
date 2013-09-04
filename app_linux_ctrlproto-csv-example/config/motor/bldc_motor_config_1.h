#ifndef _MOTOR_1
#define _MOTOR_1
#include<common_config.h>
/*
 * define Motor Specific Constants
 */
#define POLE_PAIRS_1  				8
#define GEAR_RATIO_1 				26
#define MAX_NOMINAL_SPEED_1  		4000		// in rpm
#define MAX_NOMINAL_CURRENT_1  		2		// in A
#define MAX_ACCELERATION_1   		5000     // rpm/s
#define ENCODER_RESOLUTION_1 		4000		// Max count of Quadrature Encoder
#define POLARITY_1 					1					// 1 / -1


/*Position Sensor Types*/
#define SENSOR_SELECTION_CODE_1     QEI_INDEX

/*
 * control specific constants/variables
 */
//velocity control
#define VELOCITY_Kp_NUMERATOR_1 	5
#define VELOCITY_Kp_DENOMINATOR_1  	10
#define VELOCITY_Ki_NUMERATOR_1    	5
#define VELOCITY_Ki_DENOMINATOR_1  	100
#define VELOCITY_Kd_NUMERATOR_1   	0
#define VELOCITY_Kd_DENOMINATOR_1 	1

//position control

#define MAX_FOLLOWING_ERROR 		0
#define MAX_POSITION_LIMIT_1 			359
#define MIN_POSITION_LIMIT_1 			-359

#define POSITION_Kp_NUMERATOR_1 	180
#define POSITION_Kp_DENOMINATOR_1  	2000
#define POSITION_Ki_NUMERATOR_1    	50
#define POSITION_Ki_DENOMINATOR_1  	102000
#define POSITION_Kd_NUMERATOR_1    	100
#define POSITION_Kd_DENOMINATOR_1  	10000

#endif
