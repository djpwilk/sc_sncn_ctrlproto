#include <ctrlproto_m.h>
#include <ecrt.h>
#include <stdio.h>
#include "ethercat_setup.h"

int main()
{
	int i=0;
	int j=0;

	init_master(&master_setup, slv_handles, 1);
	while(1)
	{
		j++;
		handleEcat(&master_setup,slv_handles, NUM_SLAVES);
    	slv_handles[0].motorctrl_cmd=CTRL_F_POSITION | CTRL_F_SPEED | CTRL_F_TORQUE;
      	slv_handles[0].torque_setpoint=200003;
      	slv_handles[0].speed_setpoint=300002;
      	slv_handles[0].position_setpoint=400001;
      	slv_handles[0].userdef_setpoint=500004;

      	printf("%i\n",slv_handles[0].motorctrl_cmd_readback);
      	printf("%i\n",slv_handles[0].speed_in);
      	printf("%i\n",slv_handles[0].torque_in);
		printf("%i\n",slv_handles[0].position_in);
	}
	return 0;
}
