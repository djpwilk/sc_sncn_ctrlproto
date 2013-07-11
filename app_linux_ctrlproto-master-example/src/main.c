#include <ctrlproto_m.h>
#include <ecrt.h>
#include <stdio.h>

#include "ethercat_setup.h"
#include <sys/time.h>



int main()
{

	init_master(&master_setup, slv_handles, NUM_SLAVES);

	while(1)
	{

		handleEcat(&master_setup,slv_handles, NUM_SLAVES);
		if(master_setup.opFlag)//Check if we are up
		{
			slv_handles[0].motorctrl_out = 12;
			slv_handles[0].torque_setpoint = 200;
	      	slv_handles[0].speed_setpoint = 4000;
	     	slv_handles[0].position_setpoint = 10000;
	      	slv_handles[0].operation_mode = 234;

			printf("Status: %i\n",slv_handles[0].motorctrl_status_in);
			printf("Position: %i\n",slv_handles[0].position_in);
			printf("Speed: %i\n",slv_handles[0].speed_in);
			printf("Torque: %i\n",slv_handles[0].torque_in);
			printf("Operation Mode disp: %i\n",slv_handles[0].operation_mode_disp);

		}
		else
		{
			//This is just a example what CAN be done when the master recognizes
			//that, for example, a slave is now missing...
			slv_handles[0].motorctrl_out= 0;
			slv_handles[0].torque_setpoint=0;
			slv_handles[0].speed_setpoint=0;
			slv_handles[0].position_setpoint=0;
			slv_handles[0].operation_mode=0;
		}
	}
	return 0;
}
