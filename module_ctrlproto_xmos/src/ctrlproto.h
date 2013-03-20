#include <stdint.h>
#include "../../common_cmd/cmds.h"

/**
 *	In/Output Struct for ctrlproto
 */
typedef struct
{
	int32_t out_speed;			//!< 	Sensored speed
	int32_t out_torque;			//!< 	Sensored torque
	int32_t out_position;		//!<  	Sensored position
	uint8_t ctrl_motor;			//!< 	Output which controlling methods shall be used.
	int32_t in_speed;			//!< 	Speed value setting
	int32_t in_torque;			//!< 	Torque value setting
	int32_t in_position;		//!< 	Position value setting
	uint8_t	command_number;		//!< 	Command number
	uint32_t in_userdefined;	//!< 	Userdefined in
	uint32_t out_userdefined;	//!< 	Userdefined out
} ctrl_proto_values_t;

/**
 * This function receives channel communication from the ctrlproto_pdo_handler_thread
 * It updates the referenced values according to the command and has to be placed
 * inside the control loop.
 * This function is not considered as stand alone thread! It's for being executed in
 * the motor control thread
 *
 * @param pdo_out					The channel for outgoing process data objects
 * @param pdo_in					The channel for incoming process data objects
 * @param InOut						The struct for exchanging data with the motor control functions
 */
void ctrlproto_protocol_handler_function(chanend pdo_out, chanend pdo_in, ctrl_proto_values_t &InOut);

/**
 * This function initializes a struct from the type of ctrl_proto_values_t
 */
void init_ctrl_proto(ctrl_proto_values_t &InOut);
