#include "ctrlproto.h"
#include <xs1.h>

void init_ctrl_proto(ctrl_proto_values_t &InOut)
{
	InOut.ctrl_motor=0;
	InOut.in_position=0;
	InOut.in_speed=0;
	InOut.in_torque=0;
	InOut.out_position=0;
	InOut.out_speed=0;
	InOut.out_torque=0;
}

void ctrlproto_protocol_handler_function(chanend pdo_out, chanend pdo_in, ctrl_proto_values_t &InOut)
{
	static int16_t output,  outval;
	static uint8_t  heartbeat;

	unsigned incoming=0;
	select
	{
		case pdo_in :> unsigned int from_pdo:
		{

			uint16_t cmd;
			int16_t value;
			int tmp;
			pdo_in :> tmp;
			cmd = tmp & 0xFFFF;
			pdo_in :> tmp;
			value = tmp & 0xFFFF;
			incoming =1;

			if(cmd<__ENUM_MC_END) //Motor control command
			{
				InOut.ctrl_motor=cmd;
			}
			if(cmd<__ENUM_SET_END) //Select output command
			{
				switch(cmd)
				{
				case SET_SETPOINT_SPEED:
					InOut.in_speed=value;
					break;
				case SET_SETPOINT_TORQUE:
					InOut.in_torque=value;
					break;
				case SET_SETPOINT_POSITION:
					InOut.in_position=value;
					break;
				}
			}
			else if(cmd <__ENUM_OUT_END)
			{
				output=cmd; //Set output
			}
		}
		break;
		default:
			if(incoming)
			{
				incoming=0;
				switch(output)
				{
				case GET_POSITION:
						outval=InOut.out_position;
					break;
				case GET_SPEED:
						outval=InOut.out_speed;
					break;
				case GET_TORQUE:
						outval=InOut.out_torque;
					break;
				case GET_SETPOINT_POSITION:
						outval=InOut.in_position;
					break;
				case GET_SETPOINT_SPEED:
						outval=InOut.in_speed;
					break;
				case GET_SETPOINT_TORQUE:
						outval=InOut.in_torque;
					break;
				}

				heartbeat++;
				output&=heartbeat<<8;
//
//				pdo_out<:2;
//				pdo_out<:output;
//				pdo_out<:outval;
			}
		break;
	}
}
