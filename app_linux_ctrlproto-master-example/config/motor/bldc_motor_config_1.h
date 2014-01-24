
/**
 *
 * \file bldc_motor_config_1.h
 *	Motor Control config file for motor 1 on PC
 *
 *	Please define your the motor specifications here
 *
 * Copyright (c) 2014, Synapticon GmbH
 * All rights reserved.
 * Author: Pavan Kanajar <pkanajar@synapticon.com> & Martin Schwarz <mschwarz@synapticon.com>
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
 * define Motor Specific Constants (found in motor specification sheet)
 * Mandatory constants to be set
 */
#define POLE_PAIRS_1  					8
#define MAX_NOMINAL_SPEED_1  			4000	// rpm
#define MAX_NOMINAL_CURRENT_1  			2		// A
#define MOTOR_TORQUE_CONSTANT_1			33		// mNm/A

/**
 * If you have any gears added specify gear-ratio
 * and any additional encoders attached specify encoder resolution here (optional)
 */
#define GEAR_RATIO_1 					26		// if no gears are attached - set to gear ratio to 1
#define ENCODER_RESOLUTION_1 			4000	// 4 x Max count of Quadrature Encoder (4X decoding)

/* Somanet IFM Internal Config */
#define IFM_RESOLUTION_1				DC100_RESOLUTION 	// DC300_RESOLUTION /* Specifies the current sensor resolution/A */

/* Changes direction of the motor drive */
#define POLARITY_1 						1		// 1 / -1

/* Profile defines (Mandatory for profile modes) */
#define MAX_PROFILE_VELOCITY_1  		MAX_NOMINAL_SPEED_1
#define PROFILE_VELOCITY_1				1000	// rpm
#define MAX_ACCELERATION_1   			5000    // rpm/s
#define PROFILE_ACCELERATION_1			2000	// rpm/s
#define PROFILE_DECELERATION_1  		2000	// rpm/s
#define QUICK_STOP_DECELERATION_1 		2000	// rpm/s
#define MAX_TORQUE_1					MOTOR_TORQUE_CONSTANT_1 * IFM_RESOLUTION_1 * MAX_NOMINAL_CURRENT_1
#define TORQUE_SLOPE_1 					60 		// mNm/s


/* Position Sensor Types (select your sensor type here) */
#define SENSOR_SELECTION_CODE_1         HALL	// HALL/QEI_INDEX/QEI_NO_INDEX

/* Control specific constants/variables */
	/* Torque Control (Mandatory if Torque control used) */
#define TORQUE_Kp_NUMERATOR_1 	   		20
#define TORQUE_Kp_DENOMINATOR_1  		10
#define TORQUE_Ki_NUMERATOR_1    		11
#define TORQUE_Ki_DENOMINATOR_1  		110
#define TORQUE_Kd_NUMERATOR_1    		1
#define TORQUE_Kd_DENOMINATOR_1  		10

	/* Velocity Control (Mandatory if Velocity control used) */
#define VELOCITY_Kp_NUMERATOR_1 		5
#define VELOCITY_Kp_DENOMINATOR_1  		10
#define VELOCITY_Ki_NUMERATOR_1    		5
#define VELOCITY_Ki_DENOMINATOR_1  		100
#define VELOCITY_Kd_NUMERATOR_1   		0
#define VELOCITY_Kd_DENOMINATOR_1 		1

	/* Position Control (Mandatory if Position control used) */
#define POSITION_Kp_NUMERATOR_1 		180
#define POSITION_Kp_DENOMINATOR_1  		2000
#define POSITION_Ki_NUMERATOR_1    		50
#define POSITION_Ki_DENOMINATOR_1  		102000
#define POSITION_Kd_NUMERATOR_1    		100
#define POSITION_Kd_DENOMINATOR_1  		10000
#define MAX_POSITION_LIMIT_1 			350		// degree should not exceed 359
#define MIN_POSITION_LIMIT_1 			-350	// degree should not exceed -359

#endif
