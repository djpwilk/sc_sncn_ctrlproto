#include <ctrlproto_m.h>
#include <ecrt.h>
#include <stdio.h>
#include "ethercat_setup.h"

int main()
{
	int i=0;
	int j=0;

	unsigned int old_cmd=0;
	int16_t old_value=0;

		init_master(&master_setup, slv_handles, 1);
	while(1)
	{
		j++;
		handleEcat(&master_setup,slv_handles, NUM_SLAVES, 1000);



	}
	return 0;
}
