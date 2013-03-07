#include <ctrlproto_m.h>
#include <ctrlproto_command.h>
#include <ecrt.h>
#include <stdio.h>
#include "ethercat_setup.h"

int main()
{
	int i=0;
	while(1)
	{
		handleEcat(&master_setup,slv_handles, NUM_SLAVES, 1000);

		if(slv_handles[0].in[0]!=0)printf("slv0_in0: %i\n",slv_handles[0].in[0]);

		switch(i)
		{
		case 0: setSlave(0,CTRL_TORQUE,0,true,slv_handles);
		break;

		case 1: setSlave(0,SET_SETPOINT_TORQUE,-100,true,slv_handles);
		break;

		case 2: setSlave(0,SET_SETPOINT_POSITION,200,true,slv_handles);
		break;

		case 3: setSlave(0,SET_SETPOINT_SPEED,-300,true,slv_handles);
		break;
		}
		i++;
		if(i==4)i=0;
	}
	return 0;
}
