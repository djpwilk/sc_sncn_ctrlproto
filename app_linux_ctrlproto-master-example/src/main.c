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

	init_master(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	printf("starting \n");
	while(1)
	{
		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);
		if(master_setup.op_flag)
		{
			slv_handles[slave_number].motorctrl_out = 12;
			slv_handles[slave_number].torque_setpoint = 200;
			slv_handles[slave_number].speed_setpoint = 4000;
			slv_handles[slave_number].position_setpoint = 10000;
			slv_handles[slave_number].operation_mode = 125;

			printf("Status: %d\n",slv_handles[slave_number].motorctrl_status_in);
			printf("Position: %d \n",slv_handles[slave_number].position_in);
			printf("Speed: %d\n",slv_handles[slave_number].speed_in);
			printf("Torque: %d\n",slv_handles[slave_number].torque_in);
			printf("Operation Mode disp: %d\n",slv_handles[slave_number].operation_mode_disp);
		}
	}


	return 0;
}



