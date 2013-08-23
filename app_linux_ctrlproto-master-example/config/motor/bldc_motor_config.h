/*
 * define Motor Specific Constants
 */
#define POLE_PAIRS  				8
#define GEAR_RATIO  				26
#define MAX_NOMINAL_SPEED  			4000		// in rpm
#define MAX_NOMINAL_CURRENT  		2		// in A
#define MAX_ACCELERATION   			5000     // rpm/s
#define ENCODER_RESOLUTION 			4000		// Max count of Quadrature Encoder
#define POLARITY 					1					// 1 / -1

/*single variable Sensor selection code*/
#define QEI_WITH_INDEX				1
#define QEI_WITH_NO_INDEX 			0
#define QEI_SENSOR_TYPE  			QEI_WITH_INDEX//QEI_WITH_NO_INDEX


/*
 * control specific constants/variables
 */
//velocity control
#define VELOCITY_Kp_NUMERATOR 	 	5
#define VELOCITY_Kp_DENOMINATOR  	10
#define VELOCITY_Ki_NUMERATOR    	5
#define VELOCITY_Ki_DENOMINATOR  	100
#define VELOCITY_Kd_NUMERATOR   	0
#define VELOCITY_Kd_DENOMINATOR 	1

//position control

#define MAX_FOLLOWING_ERROR 		0
#define MAX_POSITION_LIMIT 			359
#define MIN_POSITION_LIMIT 			-359

#define POSITION_Kp_NUMERATOR 	 	180
#define POSITION_Kp_DENOMINATOR  	2000
#define POSITION_Ki_NUMERATOR    	50
#define POSITION_Ki_DENOMINATOR  	102000
#define POSITION_Kd_NUMERATOR    	100
#define POSITION_Kd_DENOMINATOR  	10000
