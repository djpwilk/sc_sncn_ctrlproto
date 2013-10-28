/* app_ethercat_test
 *
 * Test appliction for module_ethercat.
 *
 * Copyright 2011, Synapticon GmbH. All rights reserved.
 * Author: Frank Jeschke <jeschke@fjes.de>
 * Changes: Christian Holl <choll@synapticon.com>
 * TODO make independent of motor_ctrl
 */

#include <platform.h>
#include <xs1.h>
#include <ethercat.h>
#include <foefs.h>
#include <print.h>
#include <ctrlproto.h>


static void pdo_handler(chanend coe_out, chanend pdo_out, chanend pdo_in)
{
	timer t;

	const unsigned int delay = 100000;
	unsigned int time = 0;

	static int16_t i=0;
	ctrl_proto_values_t InOut;
	ctrl_proto_values_t InOutOld;
	InOut = init_ctrl_proto();
	t :> time;

	while(1)
	{
		i++;
		if(i >= 999)
			i = 100;
		InOut.position_actual=i;
		InOut.torque_actual=i;
		InOut.velocity_actual=i;



		InOut.status_word = 32000;	//undefined

		ctrlproto_protocol_handler_function(pdo_out,pdo_in,InOut);
		//velocity_sdo_update(coe_out);
		//config_sdo_handler(coe_out);

		InOut.operation_mode_display = InOut.operation_mode;
//		printhexln(InOut.control_word);
//		printhexln(InOut.operation_mode);
//		printhexln(InOut.target_position);
//		printhexln(InOut.target_velocity);
//		printhexln(InOut.target_torque);

//		if(InOutOld.control_word != InOut.control_word)
//		{
//			printstr("\nMotor: ");
//			printintln(InOut.control_word);
//		}
//
//		if(InOutOld.operation_mode != InOut.operation_mode )
//		{
//			printstr("\nOperation mode: ");
//			printintln(InOut.operation_mode);
//		}
//
//		if(InOutOld.target_position != InOut.target_position)
//		{
//			printstr("\nPosition: ");
//			printintln(InOut.target_position);
//		}
//
//		if(InOutOld.target_velocity != InOut.target_velocity)
//		{
//			printstr("\nSpeed: ");
//			printintln(InOut.target_velocity);
//		}
//
//		if(InOutOld.target_torque != InOut.target_torque )
//		{
//			printstr("\nTorque: ");
//			printintln(InOut.target_torque);
//		}
//	   InOutOld.control_word 	= InOut.control_word;
//	   InOutOld.target_position = InOut.target_position;
//	   InOutOld.target_velocity = InOut.target_velocity;
//	   InOutOld.target_torque = InOut.target_torque;
//	   InOutOld.operation_mode = InOut.operation_mode;

	   t when timerafter(time+delay) :> time;

	}

}



int main(void) {
	chan coe_in;   ///< CAN from module_ethercat to consumer
	chan coe_out;  ///< CAN from consumer to module_ethercat
	chan eoe_in;   ///< Ethernet from module_ethercat to consumer
	chan eoe_out;  ///< Ethernet from consumer to module_ethercat
	chan eoe_sig;
	chan foe_in;   ///< File from module_ethercat to consumer
	chan foe_out;  ///< File from consumer to module_ethercat
	chan pdo_in;
	chan pdo_out;

	par
	{
		on stdcore[0] : {
			ecat_init();
			ecat_handler(coe_out, coe_in, eoe_out, eoe_in, eoe_sig, foe_out, foe_in, pdo_out, pdo_in);
		}


		on stdcore[0] : {
			pdo_handler(coe_out, pdo_out, pdo_in);
		}
	}

	return 0;
}

