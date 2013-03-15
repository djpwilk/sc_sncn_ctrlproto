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
	unsigned int count=0;
	int i;
	count = 0;

	pdo_in <: DATA_REQUEST;
	pdo_in :> count;
	for (i=0; i<count; i++) {
		pdo_in :> inBuffer[i];
	}

	//Test for matching number of words
	if(count==9)
	{
		InOut.ctrl_motor=inBuffer[0]&0xFF;
		InOut.command_number=(inBuffer[0]&0xFF00)>>8;
		InOut.in_torque=(inBuffer[1])  | (inBuffer[2]<<16);
		InOut.in_speed=(inBuffer[3])  | (inBuffer[4]<<16);
		InOut.in_position=(inBuffer[5])  | (inBuffer[6]<<16);
		InOut.in_userdefined=(inBuffer[7])  | (inBuffer[8]<<16);
	}

	if(count==9)
	{
		pdo_out <: count;
		for (i=0; i<count; i++)
		{
			unsigned int pdo_tmp=0;
			switch(i)
			{
			case 0:
				pdo_tmp=InOut.command_number<<8 | InOut.ctrl_motor;
				break;
			case 1:
				pdo_tmp=InOut.out_torque&0x0000FFFF;
				break;
			case 2:
				pdo_tmp=(InOut.out_torque&0xFFFF0000)>>16;
				break;
			case 3:
				pdo_tmp=InOut.out_speed&0x0000FFFF;
				break;
			case 4:
				pdo_tmp=(InOut.out_speed&0xFFFF0000)>>16;
				break;
			case 5:
				pdo_tmp=InOut.out_position&0x0000FFFF;
				break;
			case 6:
				pdo_tmp=(InOut.out_position&0xFFFF0000)>>16;
				break;
			case 7:
				pdo_tmp=InOut.out_userdefined&0x0000FFFF;
				break;
			case 8:
				pdo_tmp=(InOut.out_userdefined&0xFFFF0000)>>16;
				break;
			default:
				pdo_tmp=0xEEEEEEEE;
				break;
			}
			pdo_out <: pdo_tmp;
		}
	}
}
