/* app_ethercat_test
 *
 * Test appliction for module_ethercat.
 *
 * Copyright 2011, Synapticon GmbH. All rights reserved.
 * Author: Frank Jeschke <jeschke@fjes.de>
 * Changes: Christian Holl <choll@synapticon.com>
 */

#include <platform.h>
#include <xs1.h>
#include <ethercat.h>
#include <foefs.h>
#include <print.h>
#include <ctrlproto.h>


//#include <uip.h>
//#include <xtcp.h>

#define MAX_BUFFER_SIZE   1024

on stdcore[1] : out port ledBlue = LED_BLUE;
on stdcore[1] : out port ledGreen = LED_GREEN;
on stdcore[1] : out port ledRed = LED_RED;


static void pdo_handler(chanend pdo_out, chanend pdo_in)
{
	timer t;

	const unsigned int delay = 125000;
	unsigned int time = 0;

	static int16_t i=0;
	ctrl_proto_values_t InOut;
	ctrl_proto_values_t InOutOld;
	init_ctrl_proto(InOut);


	while(1)
	{
		i++;
		if(i>=1000)i=0;
		InOut.position_actual=i;
		InOut.torque_actual=i;
		InOut.velocity_actual=i;


		ctrlproto_protocol_handler_function(pdo_out,pdo_in,InOut);
		t :> time;

		if(InOutOld.control_word != InOut.control_word)
		{
			printstr("\nMotor: ");
			printintln(InOut.control_word);
		}

		if(InOutOld.target_position != InOut.target_position)
		{
			printstr("\nPosition: ");
			printintln(InOut.target_position);
		}

		if(InOutOld.target_velocity != InOut.target_velocity)
		{
			printstr("\nSpeed: ");
			printintln(InOut.target_velocity);
		}

		if(InOutOld.target_torque != InOut.target_torque )
		{
			printstr("\nTorque: ");
			printintln(InOut.target_torque);
		}

	   InOutOld.control_word 	= InOut.control_word;
	   InOutOld.target_position = InOut.target_position;
	   InOutOld.target_velocity = InOut.target_velocity;
	   InOutOld.target_torque = InOut.target_torque;

		t when timerafter(time+delay) :> void;
		t:>time;
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


		on stdcore[1] : {
			pdo_handler(pdo_out, pdo_in);
		}
	}

	return 0;
}



