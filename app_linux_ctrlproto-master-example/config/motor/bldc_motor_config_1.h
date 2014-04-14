
/**
 * \file bldc_motor_config_1.h
 * \brief Motor Control config file for motor 1 (Please define your the motor specifications here)
 * \author Pavan Kanajar <pkanajar@synapticon.com>
 * \version 1.0
 * \date 10/04/2014
 */

/*
 * Copyright (c) 2014, Synapticon GmbH
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Execution of this software or parts of it exclusively takes place on hardware
 *    produced by Synapticon GmbH.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the Synapticon GmbH.
 *
 */

#ifndef _MOTOR_1
#define _MOTOR_1
#include <common_config.h>

/**
 * Define Motor Specific Constants (found in motor specification sheet)
 * Mandatory constants to be set
 */
#define POLE_PAIRS_1  					4		// Number of pole pairs
#define MAX_NOMINAL_SPEED_1  			3000	// rpm
#define MAX_NOMINAL_CURRENT_1  			6		// A
#define MOTOR_TORQUE_CONSTANT_1			112		// mNm/A

/**
 * If you have any gears added, specify gear-ratio
 * and any additional encoders attached specify encoder resolution here (Mandatory)
 */
#define GEAR_RATIO_1 					1		// if no gears are attached - set to gear ratio to 1
#define ENCODER_RESOLUTION_1 			4000	// 4 x Max count of Incremental Encoder (4X decoding - quadrature mode)

/* Somanet IFM Internal Config:  Specifies the current sensor resolution per Ampere
 *  (DC300_RESOLUTION / DC100_RESOLUTION / OLD_DC300_RESOLUTION) */
#define IFM_RESOLUTION_1				DC300_RESOLUTION

/* Position Sensor Types (select your sensor type here)
 * (HALL/ QEI_INDEX/ QEI_NO_INDEX) */
#define SENSOR_SELECTION_CODE_1         QEI_INDEX

/* Polarity is used to keep all position sensors to count ticks in the same direction
 *  (POLARITY_NORMAL/POLARITY_INVERTED) */
#define QEI_SENSOR_POLARITY_1			POLARITY_NORMAL

/* Commutation offset (range 0-4095) (HALL sensor based commutation) */
#define COMMUTATION_OFFSET_CLK_1		683
#define COMMUTATION_OFFSET_CCLK_1		2731

/* Motor Winding type (STAR_WINDING/DELTA_WINDING) */
#define WINDING_TYPE_1					STAR_WINDING

/* Specify Switch Types (ACTIVE_HIGH/ACTIVE_LOW) when switch is closed
 * (Only if you have any limit switches in the system for safety/homing ) */
#define LIMIT_SWITCH_TYPES_1			ACTIVE_HIGH

/* Define Homing method (HOMING_POSITIVE_SWITCH/HOMING_NEGATIVE_SWITCH)
 * this specifies direction for the motor to find the home switch */
#define HOMING_METHOD_1                 HOMING_NEGATIVE_SWITCH

/* Changes direction of the motor drive  (1 /-1) */
#define POLARITY_1 						1

/* Profile defines (Mandatory for profile modes) */
#define MAX_PROFILE_VELOCITY_1  		MAX_NOMINAL_SPEED_1
#define PROFILE_VELOCITY_1				1000	// rpm
#define MAX_ACCELERATION_1   			3000    // rpm/s
#define PROFILE_ACCELERATION_1			2000	// rpm/s
#define PROFILE_DECELERATION_1  		2000	// rpm/s
#define QUICK_STOP_DECELERATION_1 		2500 	// rpm/s
#define MAX_TORQUE_1					MOTOR_TORQUE_CONSTANT_1 * IFM_RESOLUTION_1 * MAX_NOMINAL_CURRENT_1 // calculated
#define TORQUE_SLOPE_1 					60 		// mNm/s


/* Control specific constants/variables */
	/* Torque Control (Mandatory if Torque control used)
	 * possible range of gains Kp/Ki/Kd: 1/65536 to 32760
	 * Note: gains are calculated as NUMERATOR/DENOMINATOR to give ranges */
#define TORQUE_Kp_NUMERATOR_1 	   		20
#define TORQUE_Kp_DENOMINATOR_1  		10
#define TORQUE_Ki_NUMERATOR_1    		11
#define TORQUE_Ki_DENOMINATOR_1  		110
#define TORQUE_Kd_NUMERATOR_1    		1
#define TORQUE_Kd_DENOMINATOR_1  		10

	/* Velocity Control (Mandatory if Velocity control used)
	 * possible range of gains Kp/Ki/Kd: 1/65536 to 32760
	 * Note: gains are calculated as NUMERATOR/DENOMINATOR to give ranges */
#define VELOCITY_Kp_NUMERATOR_1 		5
#define VELOCITY_Kp_DENOMINATOR_1  		10
#define VELOCITY_Ki_NUMERATOR_1    		5
#define VELOCITY_Ki_DENOMINATOR_1  		100
#define VELOCITY_Kd_NUMERATOR_1   		0
#define VELOCITY_Kd_DENOMINATOR_1 		1

	/* Position Control (Mandatory if Position control used)
	 * possible range of gains Kp/Ki/Kd: 1/65536 to 32760
	 * Note: gains are calculated as NUMERATOR/DENOMINATOR to give ranges */
#if(SENSOR_SELECTION_CODE_1 == QEI_INDEX || SENSOR_SELECTION_CODE_1 == QEI_NO_INDEX) // PID gains for position control with Incremental Encoder
	#define POSITION_Kp_NUMERATOR_1 		660
	#define POSITION_Kp_DENOMINATOR_1  		80
	#define POSITION_Ki_NUMERATOR_1    		1
	#define POSITION_Ki_DENOMINATOR_1  		25384
	#define POSITION_Kd_NUMERATOR_1    		0
	#define POSITION_Kd_DENOMINATOR_1  		100
	#define MAX_POSITION_LIMIT_1 			GEAR_RATIO_1*ENCODER_RESOLUTION_1		// ticks (max range: 2^30, limited for safe operation)
	#define MIN_POSITION_LIMIT_1 			-GEAR_RATIO_1*ENCODER_RESOLUTION_1		// ticks (min range: -2^30, limited for safe operation)
#endif
#if(SENSOR_SELECTION_CODE_1 == HALL)		// PID gains for position control with Hall Sensor
	#define POSITION_Kp_NUMERATOR_1 	 	2265
	#define POSITION_Kp_DENOMINATOR_1  		1000
	#define POSITION_Ki_NUMERATOR_1    		1
	#define POSITION_Ki_DENOMINATOR_1  		5000
	#define POSITION_Kd_NUMERATOR_1    		1
	#define POSITION_Kd_DENOMINATOR_1  		1000

	#define MAX_POSITION_LIMIT_1 			POLE_PAIRS_1*HALL_POSITION_INTERPOLATED_RANGE*GEAR_RATIO_1		// ticks (max range: 2^30, limited for safe operation) qei/hall/any position sensor
	#define MIN_POSITION_LIMIT_1			-POLE_PAIRS_1*HALL_POSITION_INTERPOLATED_RANGE*GEAR_RATIO_1		// ticks (min range: -2^30, limited for safe operation) qei/hall/any position sensor
#endif

#endif
