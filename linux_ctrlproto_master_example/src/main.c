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
			slv_handles[0].motorctrl_cmd= CTRL_F_TORQUE;
			slv_handles[0].torque_setpoint=fromFloat(0.300);
	      	slv_handles[0].speed_setpoint=300;
	     	slv_handles[0].position_setpoint=5709;
	      	slv_handles[0].userdef_setpoint=1234;

			printf("Motor: %i\n",slv_handles[0].motorctrl_cmd);
			printf("Position: %i\n",slv_handles[0].position_in);
			printf("Speed: %i\n",slv_handles[0].speed_in);
			printf("Torque: %f\n",toFloat(slv_handles[0].torque_in));
			printf("Userdef: %i\n",slv_handles[0].userdef_in);

		}
		else
		{
			//This is just a example what CAN be done when the master recognizes
			//that, for example, a slave is now missing...
			slv_handles[0].motorctrl_cmd= CTRL_STOP;
			slv_handles[0].torque_setpoint=0;
			slv_handles[0].speed_setpoint=0;
			slv_handles[0].position_setpoint=0;
			slv_handles[0].userdef_setpoint=0;
		}
	}
	return 0;
}
