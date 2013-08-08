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

	unsigned int inBuffer[64];
	unsigned int count = 0;
	int i;

	pdo_in <: DATA_REQUEST;
	pdo_in :> count;
	//printintln(count);
	for (i = 0; i < count; i++) {
		pdo_in :> inBuffer[i];
	}

	//Test for matching number of words
	if(count == 9)
	{
		InOut.control_word 	  = inBuffer[0];
		InOut.operation_mode  = inBuffer[1] & 0xFF;
		InOut.target_torque   = (inBuffer[2] ) & 0xFFFF;
		InOut.target_velocity = inBuffer[3]  | (inBuffer[4] << 16);
		InOut.target_position = inBuffer[5]  | (inBuffer[6] << 16);
	}

	if(count == 9)
	{
		pdo_out <: count;
		for (i=0; i<count; i++)
		{
			unsigned int pdo_tmp = 0;
			switch(i)
			{
				case 0:
					pdo_tmp = InOut.status_word;
					break;
				case 1:
					pdo_tmp = InOut.operation_mode_display ;
					break;
				case 2:
					pdo_tmp = InOut.torque_actual;

					break;
				case 3:
					pdo_tmp = InOut.velocity_actual & 0xffff;


					break;
				case 4:
					pdo_tmp = (InOut.velocity_actual >> 16) & 0xffff;


					break;
				case 5:
					pdo_tmp = InOut.position_actual & 0xffff;

					break;
				case 6:
					pdo_tmp = (InOut.position_actual >> 16) & 0xffff;

					break;
				default:
					pdo_tmp = 0xEEEE;
					break;
			}
			pdo_out <: pdo_tmp;
		}
	}
}
