/*
 * drive_function.c
 *
 *  Created on: Jul 26, 2013
 *      Author: pkanajar
 */
#include "drive_function.h"

int check_ready(int status_word) {
	return (status_word & READY_TO_SWITCH_ON_STATE);
}

int check_switch_enable(int status_word) {
	return (~((status_word & SWITCH_ON_DISABLED_STATE) >> 6) & 0x1);
}

int check_switch_on(int status_word) {
	return (status_word & SWITCHED_ON_STATE) >> 1;
}

int check_op_enable(int status_word) {
	return (status_word & OPERATION_ENABLED_STATE) >> 2;
}

void run_drive()
{
/*	int ready = 0;
	int switch_enable = 0;
	int status_word = 0;
	int switch_on_state = 0;
	int op_enable_state = 0;
	int control_word;

	while(!ready)
	{
		//check ready
		status_word = get_statusword(info);
		ready = check_ready(status_word);
	}
#ifndef print_slave
	printstrln("ready");
#endif

	while(!switch_enable)
	{
		//check switch
		status_word = get_statusword(info);
		switch_enable = check_switch_enable(status_word);
	}
#ifndef print_slave
	printstrln("switch_enable");
#endif

	//out CW for S ON
	control_word = SWITCH_ON_CONTROL;
	set_controlword(control_word, info);


	while(!switch_on_state)
	{
		set_controlword(control_word, info);
		printintln(control_word);
		//check switch_on_state
		status_word = get_statusword(info);
		switch_on_state = check_switch_on(status_word);
	}

#ifndef print_slave
	printstrln("switch_on_state");
#endif
	//out CW for En OP
	control_word = ENABLE_OPERATION_CONTROL;
	set_controlword(control_word, info);

	while(!op_enable_state)
	{
		//check op_enable_state
		status_word = get_statusword(info);
		op_enable_state = check_op_enable(status_word);
	}

#ifndef print_slave
	printstrln("op_enable_state");
#endif
*/



}
