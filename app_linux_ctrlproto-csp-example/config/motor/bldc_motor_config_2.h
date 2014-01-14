
/**
 *
 * \file bldc_motor_config_2.h
 *	Motor Control config file for motor 2 on PC
 *
 *	Please define your the motor specifications here
 *
 * Copyright (c) 2013, Synapticon GmbH
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

#ifndef _MOTOR_2
#define _MOTOR_2
#include <common_config.h>

/**
 * define Motor Specific Constants (found in motor specification sheet)
 * Mandatory constants to be set
 */
#define POLE_PAIRS_2  					7
#define MAX_NOMINAL_SPEED_2  			4000	// rpm
#define MAX_NOMINAL_CURRENT_2  			2		// A
#define MOTOR_TORQUE_CONSTANT_2			33		// mNm/A

/**
 * If you have any gears added specify gear-ratio
 * and any additional encoders attached specify encoder resolution here (optional)
 */
#define GEAR_RATIO_2 					32		// if no gears are attached - set to gear ratio to 1
#define ENCODER_RESOLUTION_2 			4000	// 4 x Max count of Quadrature Encoder (4X decoding)

/* Somanet IFM Internal Config */
#define IFM_RESOLUTION_2				DC100_RESOLUTION 	// DC300_RESOLUTION /* Specifies the current sensor resolution/A */

/*Changes direction of the motor drive*/
#define POLARITY_2 						1		// 1 / -1

/* Profile defines (Mandatory for profile modes)*/
#define MAX_PROFILE_VELOCITY_2  		MAX_NOMINAL_SPEED_2
#define PROFILE_VELOCITY_2				1000	// rpm
#define MAX_ACCELERATION_2   			5000    // rpm/s
#define PROFILE_ACCELERATION_2			2000	// rpm/s
#define PROFILE_DECELERATION_2  		2000	// rpm/s
#define QUICK_STOP_DECELERATION_2 		2000	// rpm/s
#define MAX_TORQUE_2					MOTOR_TORQUE_CONSTANT_2 * IFM_RESOLUTION_2 * MAX_NOMINAL_CURRENT_2
#define TORQUE_SLOPE_2 					60 		// mNm/s


/*Position Sensor Types (select your sensor type here) */
#define SENSOR_SELECTION_CODE_2         HALL	// HALL/QEI_INDEX/QEI_NO_INDEX

/* Control specific constants/variables */
	/* Torque Control (Mandatory if Torque control used) */
#define TORQUE_Kp_NUMERATOR_2 	   		20
#define TORQUE_Kp_DENOMINATOR_2  		10
#define TORQUE_Ki_NUMERATOR_2    		11
#define TORQUE_Ki_DENOMINATOR_2  		110
#define TORQUE_Kd_NUMERATOR_2    		1
#define TORQUE_Kd_DENOMINATOR_2  		10

	/* Velocity Control (Mandatory if Velocity control used) */
#define VELOCITY_Kp_NUMERATOR_2 		5
#define VELOCITY_Kp_DENOMINATOR_2  		10
#define VELOCITY_Ki_NUMERATOR_2    		5
#define VELOCITY_Ki_DENOMINATOR_2  		100
#define VELOCITY_Kd_NUMERATOR_2   		0
#define VELOCITY_Kd_DENOMINATOR_2 		1

	/* Position Control (Mandatory if Position control used) */
#define POSITION_Kp_NUMERATOR_2 		180
#define POSITION_Kp_DENOMINATOR_2  		2000
#define POSITION_Ki_NUMERATOR_2    		50
#define POSITION_Ki_DENOMINATOR_2  		102000
#define POSITION_Kd_NUMERATOR_2    		100
#define POSITION_Kd_DENOMINATOR_2  		10000
#define MAX_POSITION_LIMIT_2 			350		// degree should not exceed 359
#define MIN_POSITION_LIMIT_2 			-350	// degree should not exceed -359

#endif
