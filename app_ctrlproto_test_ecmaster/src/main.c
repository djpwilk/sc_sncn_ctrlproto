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
    	slv_handles[0].motorctrl_cmd_out=9;
      	slv_handles[0].torque_out=200003;
      	slv_handles[0].speed_out=300002;
      	slv_handles[0].position_out=400001;
      	slv_handles[0].userdef_out=500004;

      	printf("%i\n",slv_handles[0].motorctrl_cmd_in);
      	printf("%i\n",slv_handles[0].speed_in);
      	printf("%i\n",slv_handles[0].torque_in);
		printf("%i\n",slv_handles[0].position_in);
	}
	return 0;
}
