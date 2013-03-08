#include "ctrlproto.h"
#include <xs1.h>
#include <print.h>
#include <ethercat.h>
#include <foefs.h>

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

	unsigned int inBuffer[64];
	unsigned int outBuffer[64];
	unsigned int count=0;
	unsigned int outCount=0;
	unsigned int tmp;
	static uint16_t cmd, cmd_old;
	static int16_t value, value_old;
	unsigned ready = 0;
	int i;

	static int16_t output,  outval;
	static uint8_t  heartbeat;

	timer t;
	const unsigned int delay = 100;
	unsigned int time = 0;


	t :> time;

	t when timerafter(time+delay) :> void;

		count = 0;

		pdo_in <: DATA_REQUEST;
		pdo_in :> count;
		for (i=0; i<count; i++) {
			pdo_in :> inBuffer[i];
		}

		if(count==2)
		{
			cmd=inBuffer[0];
			value=inBuffer[1];
		}
		if(cmd_old != cmd || value != value_old)
		{
			cmd_old = cmd;
			value_old=value;
			heartbeat++;
		}


		if(cmd<=__ENUM_MC_END) //Motor control command
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

			switch(output)
			{
			case GET_POSITION:
					//printstrln("POS");
					outval=InOut.out_position;
				break;
			case GET_SPEED:
					//printstrln("SPE");
					outval=InOut.out_speed;
				break;
			case GET_TORQUE:
					//printstrln("TRQ");
					outval=InOut.out_torque;
				break;
			case GET_SETPOINT_POSITION:
				 	//printstrln("SPOS");
					outval=InOut.in_position;
				break;
			case GET_SETPOINT_SPEED:
			 		//printstrln("SSPE");
					outval=InOut.in_speed;
				break;
			case GET_SETPOINT_TORQUE:
			 		//printstrln("STRQ");
					outval=InOut.in_torque;
				break;
			}

			output&=0xFF;
			tmp=output|(heartbeat<<8);

			inBuffer[0]=tmp;
			tmp = outval;
			inBuffer[1]=tmp;

		if (count>0) {
			pdo_out <: count;
			for (i=0; i<count; i++)
			{
				pdo_out <: inBuffer[i];
			}
		}

//	}
}
