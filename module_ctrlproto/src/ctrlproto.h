#ifndef CTRLPROTO_H_
#define CTRLPROTO_H_
#pragma once

#include <stdint.h>
#include <coecmd.h>
#include <canod.h>


/*internal qei single variable selection code*/
#define QEI_WITH_INDEX				1
#define QEI_WITH_NO_INDEX 			0
#define QEI_SENSOR_TYPE  			QEI_WITH_INDEX//QEI_WITH_NO_INDEX

/*Position Sensor Types*/
#define HALL 						1
#define QEI_INDEX  					2
#define QEI_NO_INDEX				3

#define GET_SDO_DATA(index, sub_index, value) coe_out <: CAN_GET_OBJECT; coe_out <: CAN_OBJ_ADR(index, sub_index);	coe_out :> value;
/**
 * \brief
 *		Struct for Tx, Rx PDOs
 */
typedef struct
{
	int operation_mode;			// 	Modes of Operation
	int control_word;			// 	Control Word

	int target_torque;
	int target_velocity;
	int target_position;


	int operation_mode_display;	//	Modes of Operation Display
	int status_word;			//  Status Word

	int torque_actual;
	int velocity_actual;
	int position_actual;

} ctrl_proto_values_t;



/**
 * \brief
 *  This function receives channel communication from the ctrlproto_pdo_handler_thread
 *  It updates the referenced values according to the command and has to be placed
 *  inside the control loop.
 *
 *  This function is not considered as stand alone thread! It's for being executed in
 *  the motor control thread
 *
 * \param pdo_out					The channel for outgoing process data objects
 * \param pdo_in					The channel for incoming process data objects
 * \param InOut						The struct for exchanging data with the motor control functions
 */
void ctrlproto_protocol_handler_function(chanend pdo_out, chanend pdo_in, ctrl_proto_values_t &InOut);

/**
 *  \brief
 * 	 This function initializes a struct from the type of ctrl_proto_values_t
 */
ctrl_proto_values_t init_ctrl_proto(void);

void config_sdo_handler(chanend coe_out);

{int, int, int} velocity_sdo_update(chanend coe_out);

{int, int} hall_sdo_update(chanend coe_out);

{int, int, int} csv_sdo_update(chanend coe_out);

{int, int, int, int, int, int} csp_sdo_update(chanend coe_out);

{int, int, int} qei_sdo_update(chanend coe_out);

{int, int, int} position_sdo_update(chanend coe_out);

int sensor_select_sdo(chanend coe_out);

#endif
