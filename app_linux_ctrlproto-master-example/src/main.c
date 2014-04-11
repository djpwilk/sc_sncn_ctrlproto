
/**
 * \file main.c
 * \brief Example Master App to test Ethercat (on PC)
 * \author Christian Holl <choll@synapticon.com>
 * \version 1.0
 * \date 10/04/2014
 */

/*
 * Copyright (c) 2014, Synapticon GmbH
 * All rights reserved.
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

#include <ctrlproto_m.h>
#include <ecrt.h>
#include "ethercat_setup.h"
#include <stdio.h>
#include <motor_define.h>
#include <sys/time.h>
#include <time.h>



int main()
{
	int slave_number = 0;

	/* Initialize EtherCAT Master */
	init_master(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	printf("starting Master application\n");
	while(1)
	{
		/* Update the process data (EtherCAT packets) sent/received from the node */
		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

		if(master_setup.op_flag) /*Check if the master is active*/
		{
			/* Write Process data */
			slv_handles[slave_number].motorctrl_out = 12;
			slv_handles[slave_number].torque_setpoint = 200;
			slv_handles[slave_number].speed_setpoint = 4000;
			slv_handles[slave_number].position_setpoint = 10000;
			slv_handles[slave_number].operation_mode = 125;

			/* Read Process data */
			printf("Status: %d\n", slv_handles[slave_number].motorctrl_status_in);
			printf("Position: %d \n", slv_handles[slave_number].position_in);
			printf("Speed: %d\n", slv_handles[slave_number].speed_in);
			printf("Torque: %d\n", slv_handles[slave_number].torque_in);
			printf("Operation Mode disp: %d\n", slv_handles[slave_number].operation_mode_disp);
		}
	}


	return 0;
}



