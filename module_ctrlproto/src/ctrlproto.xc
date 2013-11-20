
/**
 *
 * \file ctrlproto.xc
 *
 * \brief Control Protocol Handler
 *
 * Copyright (c) 2013, Synapticon GmbH
 * All rights reserved.
 * Author: Christian Holl <choll@synapticon.com> & Pavan Kanajar <pkanajar@synapticon.com>
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

#include "ctrlproto.h"
#include <xs1.h>
#include <print.h>
#include <ethercat.h>
#include <foefs.h>

ctrl_proto_values_t init_ctrl_proto(void)
{
	ctrl_proto_values_t InOut;

	InOut.control_word    = 0x00;    		// shutdown
	InOut.operation_mode  = 0xff;  			// undefined

	InOut.target_torque   = 0x0;
	InOut.target_velocity = 0x0;
	InOut.target_position = 0x0;

	InOut.status_word     = 0x0000;  		// not set
	InOut.operation_mode_display = 0xff; 	// undefined

	InOut.torque_actual   = 0x0;
	InOut.velocity_actual = 0x0;
	InOut.position_actual = 0x0;

	return InOut;
}



void config_sdo_handler(chanend coe_out)
{
	int sdo_value;


/*	GET_SDO_DATA(CIA402_QEI_OFFSET, 1, sdo_value);
	printintln(sdo_value);
	GET_SDO_DATA(CIA402_QEI_OFFSET, 2, sdo_value);
	printintln(sdo_value);
	GET_SDO_DATA(CIA402_QEI_OFFSET, 3, sdo_value);
	printintln(sdo_value);
	GET_SDO_DATA(CIA402_QEI_OFFSET, 4, sdo_value);
	printintln(sdo_value);*/
	GET_SDO_DATA(CIA402_MOTOR_SPECIFIC, 6, sdo_value);  //motor tor const
	printintln(sdo_value);

	GET_SDO_DATA(CIA402_CURRENT_GAIN, 1, sdo_value);
	printintln(sdo_value);
	GET_SDO_DATA(CIA402_CURRENT_GAIN, 2, sdo_value);
	printintln(sdo_value);
	GET_SDO_DATA(CIA402_CURRENT_GAIN, 3, sdo_value);
	printintln(sdo_value);

	GET_SDO_DATA(CIA402_MAX_TORQUE, 0, sdo_value);
	printintln(sdo_value);
	GET_SDO_DATA(CIA402_TORQUE_SLOPE, 0, sdo_value);
	printintln(sdo_value);

	GET_SDO_DATA(CIA402_MAX_ACCELERATION, 0, sdo_value);
	printintln(sdo_value);
	GET_SDO_DATA(CIA402_GEAR_RATIO, 0, sdo_value);
	printintln(sdo_value);
	GET_SDO_DATA(CIA402_MOTOR_SPECIFIC, 1, sdo_value);
	printintln(sdo_value);
	GET_SDO_DATA(CIA402_MOTOR_SPECIFIC, 4, sdo_value);
	printintln(sdo_value);
	GET_SDO_DATA(CIA402_POLARITY, 0, sdo_value);
	printintln(sdo_value);  // -1 in 2'complement 255
	GET_SDO_DATA(CIA402_MOTOR_SPECIFIC, 3, sdo_value);
	printintln(sdo_value);
	GET_SDO_DATA(CIA402_POSITION_ENC_RESOLUTION, 0, sdo_value);
	printintln(sdo_value);
	GET_SDO_DATA(CIA402_SENSOR_SELECTION_CODE, 0, sdo_value);
	printintln(sdo_value);
	GET_SDO_DATA(CIA402_VELOCITY_GAIN, 1, sdo_value);
	printintln(sdo_value);
	GET_SDO_DATA(CIA402_VELOCITY_GAIN, 2, sdo_value);
	printintln(sdo_value);
	GET_SDO_DATA(CIA402_VELOCITY_GAIN, 3, sdo_value);
	printintln(sdo_value);
	GET_SDO_DATA(CIA402_POSITION_GAIN, 1, sdo_value);
	printintln(sdo_value);
	GET_SDO_DATA(CIA402_POSITION_GAIN, 2, sdo_value);
	printintln(sdo_value);
	GET_SDO_DATA(CIA402_POSITION_GAIN, 3, sdo_value);
	printintln(sdo_value);
	GET_SDO_DATA(CIA402_SOFTWARE_POSITION_LIMIT, 1, sdo_value);
	printintln(sdo_value);
	GET_SDO_DATA(CIA402_SOFTWARE_POSITION_LIMIT, 2, sdo_value);
	printintln(sdo_value);

	GET_SDO_DATA(CIA402_MAX_PROFILE_VELOCITY, 0, sdo_value);
	printintln(sdo_value);
	GET_SDO_DATA(CIA402_PROFILE_VELOCITY, 0, sdo_value);
	printintln(sdo_value);
	GET_SDO_DATA(CIA402_PROFILE_ACCELERATION, 0, sdo_value);
	printintln(sdo_value);
	GET_SDO_DATA(CIA402_PROFILE_DECELERATION, 0, sdo_value);
	printintln(sdo_value);
	GET_SDO_DATA(CIA402_QUICK_STOP_DECELERATION, 0, sdo_value);
	printintln(sdo_value);

}

{int, int, int, int, int, int, int, int, int} pp_sdo_update(chanend coe_out)
{
	int max_profile_velocity, profile_acceleration, profile_deceleration, quick_stop_deceleration, profile_velocity, min, max, polarity, max_acc;

	GET_SDO_DATA(CIA402_MAX_PROFILE_VELOCITY, 0, max_profile_velocity);
	GET_SDO_DATA(CIA402_PROFILE_VELOCITY, 0, profile_velocity);
	GET_SDO_DATA(CIA402_PROFILE_ACCELERATION, 0, profile_acceleration);
	GET_SDO_DATA(CIA402_PROFILE_DECELERATION, 0, profile_deceleration);
	GET_SDO_DATA(CIA402_QUICK_STOP_DECELERATION, 0, quick_stop_deceleration);
	GET_SDO_DATA(CIA402_SOFTWARE_POSITION_LIMIT, 1, min);
	GET_SDO_DATA(CIA402_SOFTWARE_POSITION_LIMIT, 2, max);
	GET_SDO_DATA(CIA402_POLARITY, 0, polarity);
	GET_SDO_DATA(CIA402_MAX_ACCELERATION, 0, max_acc);

	return {max_profile_velocity, profile_velocity, profile_acceleration, profile_deceleration, quick_stop_deceleration, min, max, polarity, max_acc};
}

{int, int, int, int, int} pv_sdo_update(chanend coe_out)
{
	int max_profile_velocity, profile_acceleration, profile_deceleration, quick_stop_deceleration, polarity;

	GET_SDO_DATA(CIA402_MAX_PROFILE_VELOCITY, 0, max_profile_velocity);
	GET_SDO_DATA(CIA402_PROFILE_ACCELERATION, 0, profile_acceleration);
	GET_SDO_DATA(CIA402_PROFILE_DECELERATION, 0, profile_deceleration);
	GET_SDO_DATA(CIA402_QUICK_STOP_DECELERATION, 0, quick_stop_deceleration);
	GET_SDO_DATA(CIA402_POLARITY, 0, polarity);
	return {max_profile_velocity, profile_acceleration, profile_deceleration, quick_stop_deceleration, polarity};
}

{int, int} pt_sdo_update(chanend coe_out)
{
	int torque_slope, polarity;
	GET_SDO_DATA(CIA402_TORQUE_SLOPE, 0, torque_slope);
	GET_SDO_DATA(CIA402_POLARITY, 0, polarity);
	return {torque_slope, polarity};
}

{int, int, int} position_sdo_update(chanend coe_out)
{
	int Kp, Ki, Kd;

	GET_SDO_DATA(CIA402_POSITION_GAIN, 1, Kp);
	GET_SDO_DATA(CIA402_POSITION_GAIN, 2, Ki);
	GET_SDO_DATA(CIA402_POSITION_GAIN, 3, Kd);

	return {Kp, Ki, Kd};
}

{int, int, int} torque_sdo_update(chanend coe_out)
{
	int Kp, Ki, Kd;

	GET_SDO_DATA(CIA402_CURRENT_GAIN, 1, Kp);
	GET_SDO_DATA(CIA402_CURRENT_GAIN, 2, Ki);
	GET_SDO_DATA(CIA402_CURRENT_GAIN, 3, Kd);

	return {Kp, Ki, Kd};
}

{int, int, int, int, int} cst_sdo_update(chanend coe_out)
{
	int  nominal_current, max_motor_speed, polarity, max_torque, motor_torque_constant;

	GET_SDO_DATA(CIA402_MOTOR_SPECIFIC, 1, nominal_current);
	GET_SDO_DATA(CIA402_MOTOR_SPECIFIC, 4, max_motor_speed);
	GET_SDO_DATA(CIA402_POLARITY, 0, polarity);
	GET_SDO_DATA(CIA402_MAX_TORQUE, 0, max_torque);
	GET_SDO_DATA(CIA402_MOTOR_SPECIFIC, 6, motor_torque_constant);

	return {nominal_current, max_motor_speed, polarity, max_torque, motor_torque_constant};
}

{int, int, int, int, int} csv_sdo_update(chanend coe_out)
{
	int max_motor_speed, nominal_current, polarity, motor_torque_constant, max_acceleration;

	GET_SDO_DATA(CIA402_MOTOR_SPECIFIC, 1, nominal_current);
	GET_SDO_DATA(CIA402_MOTOR_SPECIFIC, 4, max_motor_speed);
	GET_SDO_DATA(CIA402_MOTOR_SPECIFIC, 6, motor_torque_constant);

	GET_SDO_DATA(CIA402_POLARITY, 0, polarity);
	GET_SDO_DATA(CIA402_MAX_ACCELERATION, 0, max_acceleration)
	//printintln(max_motor_speed);printintln(nominal_current);printintln(polarity);printintln(max_acceleration);printintln(motor_torque_constant);
	return {max_motor_speed, nominal_current, polarity, max_acceleration, motor_torque_constant};
}

int speed_sdo_update(chanend coe_out)
{
	int max_motor_speed;
	GET_SDO_DATA(CIA402_MOTOR_SPECIFIC, 4, max_motor_speed);
	return max_motor_speed;
}

{int, int, int, int, int, int} csp_sdo_update(chanend coe_out)
{
	int  max_motor_speed, polarity, nominal_current, min, max, max_acc;

	GET_SDO_DATA(CIA402_MOTOR_SPECIFIC, 4, max_motor_speed);
	GET_SDO_DATA(CIA402_POLARITY, 0, polarity);
	GET_SDO_DATA(CIA402_MOTOR_SPECIFIC, 1, nominal_current);
	GET_SDO_DATA(CIA402_SOFTWARE_POSITION_LIMIT, 1, min);
	GET_SDO_DATA(CIA402_SOFTWARE_POSITION_LIMIT, 2, max);
	GET_SDO_DATA(CIA402_MAX_ACCELERATION, 0, max_acc);

	return {max_motor_speed, polarity, nominal_current, min, max, max_acc};
}

int sensor_select_sdo(chanend coe_out)
{
	int sensor_select;
	GET_SDO_DATA(CIA402_SENSOR_SELECTION_CODE, 0, sensor_select);
	if(sensor_select == 2 || sensor_select == 3)
		sensor_select = 2; //qei
	return sensor_select;
}
{int, int, int} velocity_sdo_update(chanend coe_out)
{
	int Kp, Ki, Kd;

	GET_SDO_DATA(CIA402_VELOCITY_GAIN, 1, Kp);
	GET_SDO_DATA(CIA402_VELOCITY_GAIN, 2, Ki);
	GET_SDO_DATA(CIA402_VELOCITY_GAIN, 3, Kd);

	return {Kp, Ki, Kd};
}

{int, int} hall_sdo_update(chanend coe_out)
{
	int gear_ratio, pole_pairs;

	GET_SDO_DATA(CIA402_GEAR_RATIO, 0, gear_ratio);
	GET_SDO_DATA(CIA402_MOTOR_SPECIFIC, 3, pole_pairs);

	return {pole_pairs, gear_ratio};
}



{int, int, int} qei_sdo_update(chanend coe_out)
{
	int real_counts, gear_ratio, qei_type;

	GET_SDO_DATA(CIA402_GEAR_RATIO, 0, gear_ratio);
	GET_SDO_DATA(CIA402_POSITION_ENC_RESOLUTION, 0, real_counts);
	GET_SDO_DATA(CIA402_SENSOR_SELECTION_CODE, 0, qei_type);

	if(qei_type == QEI_INDEX)
		return {real_counts, gear_ratio, QEI_WITH_INDEX};
	else if(qei_type == QEI_NO_INDEX)
		return {real_counts, gear_ratio, QEI_WITH_NO_INDEX};
	else
		return {real_counts, gear_ratio, QEI_WITH_INDEX};//default
}

int ctrlproto_protocol_handler_function(chanend pdo_out, chanend pdo_in, ctrl_proto_values_t &InOut)
{

	int buffer[64];
	unsigned int count = 0;
	int i;


	pdo_in <: DATA_REQUEST;
	pdo_in :> count;
	//printstr("count  ");
	//printintln(count);
	for (i = 0; i < count; i++) {
		pdo_in :> buffer[i];
		//printhexln(buffer[i]);
	}

	//Test for matching number of words
	if(count > 0)
	{
		InOut.control_word 	  = (buffer[0]) & 0xffff;
		InOut.operation_mode  = buffer[1] & 0xff;
		InOut.target_torque   =  ((buffer[2]<<8 & 0xff00) | (buffer[1]>>8 & 0xff)) & 0x0000ffff;
		InOut.target_position = ((buffer[4]&0x00ff)<<24 | buffer[3]<<8 | (buffer[2] & 0xff00)>>8 )&0xffffffff;
		InOut.target_velocity = (buffer[6]<<24 | buffer[5]<<8 |  (buffer[4]&0xff00) >> 8)&0xffffffff;
//		printhexln(InOut.control_word);
//		printhexln(InOut.operation_mode);
//		printhexln(InOut.target_torque);
//		printhexln(InOut.target_position);
//		printhexln(InOut.target_velocity);
	}

	if(count > 0)
	{
		pdo_out <: 7;
		buffer[0] = InOut.status_word ;
		buffer[1] = (InOut.operation_mode_display | (InOut.position_actual&0xff)<<8) ;
		buffer[2] = (InOut.position_actual>> 8)& 0xffff;
		buffer[3] = (InOut.position_actual>>24) & 0xff | (InOut.velocity_actual&0xff)<<8 ;
		buffer[4] = (InOut.velocity_actual>> 8)& 0xffff;
		buffer[5] = (InOut.velocity_actual>>24) & 0xff | (InOut.torque_actual&0xff)<<8 ;
		buffer[6] = (InOut.torque_actual >> 8)&0xff;
		for (i = 0; i < 7; i++)
		{
			pdo_out <: (unsigned) buffer[i];
		}
	}
	return count;
}

