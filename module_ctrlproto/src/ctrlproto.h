#include <stdint.h>

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
