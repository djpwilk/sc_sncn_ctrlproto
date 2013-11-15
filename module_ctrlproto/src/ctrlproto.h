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
	int8_t  operation_mode;			// 	Modes of Operation
	int16_t  control_word;			// 	Control Word

	int16_t  target_torque;
	int32_t target_velocity;
	int32_t target_position;


	int8_t operation_mode_display;	//	Modes of Operation Display
	int16_t status_word;			//  Status Word

	int16_t torque_actual;
	int32_t velocity_actual;
	int32_t position_actual;

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
int ctrlproto_protocol_handler_function(chanend pdo_out, chanend pdo_in, ctrl_proto_values_t &InOut);

/**
 *  \brief
 * 	 This function initializes a struct from the type of ctrl_proto_values_t
 *
 * 	\return ctrl_proto_values_t with values initialised
 */
ctrl_proto_values_t init_ctrl_proto(void);

void config_sdo_handler(chanend coe_out);

/**
 * \brief read sensor select from Ethercat
 *
 * \return sensor_select HALL/QEI
 *
 */
int sensor_select_sdo(chanend coe_out);

/**
 * \brief read qei params from Ethercat
 *
 * \return real_counts
 * \return gear_ratio
 * \return qei_type
 *
 */
{int, int, int} qei_sdo_update(chanend coe_out);

/**
 * \brief read hall params from Ethercat
 *
 * \return gear_ratio
 * \return pole_pairs
 *
 */
{int, int} hall_sdo_update(chanend coe_out);

/**
 * \brief read profile torque params from Ethercat
 *
 * \return torque_slope
 * \return polarity
 *
 */
{int, int} pt_sdo_update(chanend coe_out);

/**
 * \brief read profile velocity params from Ethercat
 *
 * \return max_profile_velocity
 * \return profile_acceleration
 * \return profile_deceleration
 * \return quick_stop_deceleration
 * \return polarity
 *
 */
{int, int, int, int, int} pv_sdo_update(chanend coe_out);


/**
 * \brief read profile position params from Ethercat
 *
 * \return max_profile_velocity
 * \return profile_acceleration
 * \return profile_deceleration
 * \return quick_stop_deceleration
 * \return profile_velocity
 * \return min
 * \return max
 * \return polarity
 * \return max_acceleration
 *
 */
{int, int, int, int, int, int, int, int, int} pp_sdo_update(chanend coe_out);

/**
 * \brief read cyclic synchronous torque params from Ethercat
 *
 * \return nominal_current
 * \return max_motor_speed
 * \return polarity
 * \return max_torque
 * \return motor_torque_constant
 *
 */
{int, int, int, int, int} cst_sdo_update(chanend coe_out);

/**
 * \brief read cyclic synchronous velocity params from Ethercat
 *
 * \return max_motor_speed
 * \return nominal_current
 * \return polarity
 * \return motor_torque_constant
 * \return max_acceleration
 *
 */
{int, int, int, int, int} csv_sdo_update(chanend coe_out);

/**
 * \brief read cyclic synchronous position params from Ethercat
 *
 * \return max_motor_speed
 * \return polarity
 * \return nominal_current
 * \return min
 * \return max
 * \return max_acceleration
 *
 */
{int, int, int, int, int, int} csp_sdo_update(chanend coe_out);

/**
 * \brief read torque control params from Ethercat
 *
 * \return Kp
 * \return Ki
 * \return Kd
 *
 */
{int, int, int} torque_sdo_update(chanend coe_out);

/**
 * \brief read velocity control params from Ethercat
 *
 * \return Kp
 * \return Ki
 * \return Kd
 *
 */
{int, int, int} velocity_sdo_update(chanend coe_out);

/**
 * \brief read position control params from Ethercat
 *
 * \return Kp
 * \return Ki
 * \return Kd
 *
 */
{int, int, int} position_sdo_update(chanend coe_out);

/**
 * \brief read nominal speed from Ethercat
 *
 * \return nominal_speed
 *
 */
int speed_sdo_update(chanend coe_out);

#endif
