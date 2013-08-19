#include "ctrlproto.h"
#include <xs1.h>
#include <print.h>
#include <ethercat.h>
#include <foefs.h>

ctrl_proto_values_t init_ctrl_proto(void)
{
	ctrl_proto_values_t InOut;

	InOut.control_word    = 0x0006;    		// shutdown
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



void ctrlproto_protocol_handler_function(chanend pdo_out, chanend pdo_in, ctrl_proto_values_t &InOut)
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
		printhexln(buffer[i]);
	}

	//Test for matching number of words
	if(count > 0)
	{
		InOut.control_word 	  = (buffer[0]) & 0xffff;
		InOut.operation_mode  = buffer[1] & 0xff;
		InOut.target_torque   =  ((buffer[2]<<8 & 0xff00) | (buffer[1]>>8 & 0xff)) & 0xffff;
		InOut.target_position = ((buffer[4]&0x00ff)<<24 | buffer[3]<<8 | (buffer[2] & 0xff00)>>8 )&0xffffffff;
		InOut.target_velocity = (buffer[6]<<24 | buffer[5]<<8 |  (buffer[4]&0xff00) >> 8)&0xffffffff;
//		printhexln(InOut.control_word);
//		printhexln(InOut.operation_mode);
//		printhexln(InOut.target_torque);
//		printhexln(InOut.target_position);
//		printhexln(InOut.target_velocity);




	}
//	InOut.status_word = 0xacac;
//	InOut.operation_mode_display = 0xf3;
//	InOut.position_actual = 0x95664577;
//	InOut.velocity_actual = 0x14521584;
//	InOut.torque_actual = 0x5544;


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
}


