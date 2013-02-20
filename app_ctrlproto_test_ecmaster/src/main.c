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
		//Currently this is a workaround because we are currently not able to receive a response from a slave
		//Comming soon, should be there in March 2013
		handleEcat(&master_setup,slv_handles, NUM_SLAVES, 100);
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
