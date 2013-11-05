#ifndef MOTOR_2
#define MOTOR_2
#include<common_config.h>
/*
 * define Motor Specific Constants
 */
#define POLE_PAIRS_2  				12
#define GEAR_RATIO_2  				27
#define MAX_NOMINAL_SPEED_2  		4001		// in rpm
#define MAX_NOMINAL_CURRENT_2  		3		// in A
#define MAX_ACCELERATION_2   		5001     // rpm/s
#define ENCODER_RESOLUTION_2 		4001		// Max count of Quadrature Encoder
#define MOTOR_TORQUE_CONSTANT_2		33					// mNm/A
#define POLARITY_2 					-1					// 1 / -1

/* Somanet IFM Internal Config */
#define DC100_RESOLUTION 				740 				// resolution/A
#define DC900_RESOLUTION				264 				// resolution/A
#define IFM_RESOLUTION_2				DC900_RESOLUTION

/* Profile defines */
#define MAX_PROFILE_VELOCITY_2  	MAX_NOMINAL_SPEED_2
#define PROFILE_VELOCITY_2			1000
#define PROFILE_ACCELERATION_2		2000
#define PROFILE_DECELERATION_2   	2000
#define QUICK_STOP_DECELERATION_2 	2000
#define MAX_TORQUE_2					MOTOR_TORQUE_CONSTANT_2 * IFM_RESOLUTION_2 * MAX_NOMINAL_CURRENT_2
#define TORQUE_SLOPE_2 					6000 				// mNm/s * IFM_RESOLUTION

/*config*/
#define QEI_OFFSET_CLK_2 				184
#define QEI_OFFSET_CCLK_2 				104
#define QEI_COMMUTATION_OFFSET_CLK_2 	682
#define QEI_COMMUTATION_OFFSET_CCLK_2 	2773

/*Position Sensor Types*/
#define SENSOR_SELECTION_CODE_2       HALL

/* control specific constants/variables */
	/*Torque Control*/
#define TORQUE_Kp_NUMERATOR_2 	   		50
#define TORQUE_Kp_DENOMINATOR_2  		10
#define TORQUE_Ki_NUMERATOR_2    		11
#define TORQUE_Ki_DENOMINATOR_2  		110
#define TORQUE_Kd_NUMERATOR_2    		1
#define TORQUE_Kd_DENOMINATOR_2  		10

	/*Velocity Control*/
#define VELOCITY_Kp_NUMERATOR_2 	 	6
#define VELOCITY_Kp_DENOMINATOR_2  	10
#define VELOCITY_Ki_NUMERATOR_2   	6
#define VELOCITY_Ki_DENOMINATOR_2  	100
#define VELOCITY_Kd_NUMERATOR_2   	1
#define VELOCITY_Kd_DENOMINATOR_2 	1

	/*position control*/
#define MAX_POSITION_LIMIT_2 			359
#define MIN_POSITION_LIMIT_2 			-359

#define POSITION_Kp_NUMERATOR_2 	 	180
#define POSITION_Kp_DENOMINATOR_2  	2000
#define POSITION_Ki_NUMERATOR_2    	50
#define POSITION_Ki_DENOMINATOR_2  	102000
#define POSITION_Kd_NUMERATOR_2    	100
#define POSITION_Kd_DENOMINATOR_2  	10000
#endif
