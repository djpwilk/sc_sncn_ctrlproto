
/**
 *
 * \file ctrlproto_m.h
 *
 *
 * Copyright (c) 2014, Synapticon GmbH
 * All rights reserved.
 * Author: Pavan Kanajar <pkanajar@synapticon.com>, Christian Holl <choll@synapticon.com>
 * 			& Frank Jeschke <jeschke@fjes.de>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Execution of this software or parts of it exclusively takes place on hardware
 *    produced by Synapticon GmbH.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the Synapticon GmbH.
 *
 */

#ifndef CTRLPROTO_M_H_
#define CTRLPROTO_M_H_

#include <stdbool.h>
#include <ecrt.h>
#include <inttypes.h>
#include <motor_define.h>
#include "bldc_motor_config_1.h"
#include "bldc_motor_config_2.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define MOTOR_PARAM_UPDATE 		1
#define VELOCITY_CTRL_UPDATE 	2
#define CSV_MOTOR_UPDATE   		3
#define POSITION_CTRL_UPDATE    4
#define PV_MOTOR_UPDATE			5
#define PP_MOTOR_UPDATE  		6
#define CST_MOTOR_UPDATE		7
#define TORQUE_CTRL_UPDATE 		8
#define TQ_MOTOR_UPDATE			9
#define QEI_CALIBRATE_UPDATE   10


#define SOMANET_ID     0x000022d2, 0x00000201
#define CAN_OD_CONTROL_WORD    	0x6040 		/* RX; 16 bit */
#define CAN_OD_STATUS_WORD      0x6041 		/* TX; 16 bit */
#define CAN_OD_MODES            0x6060 		/* RX; 8 bit */
#define CAN_OD_MODES_DISPLAY	0x6061 		/* TX; 8 bit */
#define CAN_OD_TORQUE_TARGET   	0x6071 		/* RX; 16 bit */
#define CAN_OD_TORQUE_VALUE  	0x6077 		/* TX; 16 bit */
#define CAN_OD_POSITION_TARGET	0x607A 		/* RX; 32 bit */
#define CAN_OD_POSITION_VALUE 	0x6064 		/* TX; 32 bit */
#define CAN_OD_VELOCITY_TARGET	0x60ff 		/* RX; 32 bit */
#define CAN_OD_VELOCITY_VALUE	0x606C 		/* TX; 32 bit */


/**
 * This creates the defines for a SOMANET device running CTRLPROTO
 */
#define SOMANET_C22_CTRLPROTO_CSTRUCT()\
ec_pdo_entry_info_t ctrlproto_pdo_entries[] = {\
		{CAN_OD_CONTROL_WORD, 0x00, 16},\
		{CAN_OD_MODES, 0x00, 8},\
	    {CAN_OD_TORQUE_TARGET, 0x00, 16},\
	    {CAN_OD_POSITION_TARGET, 0x00, 32},\
	    {CAN_OD_VELOCITY_TARGET, 0x00, 32},\
	    {CAN_OD_STATUS_WORD, 0x00, 16},\
	    {CAN_OD_MODES_DISPLAY, 0x00, 8},\
	    {CAN_OD_POSITION_VALUE, 0x00, 32},\
	    {CAN_OD_VELOCITY_VALUE, 0x00, 32},\
	    {CAN_OD_TORQUE_VALUE, 0x00, 16},\
};\
\
ec_pdo_info_t ctrlproto_pdos[] = {\
    {0x1600, 5, ctrlproto_pdo_entries + 0},\
    {0x1a00, 5, ctrlproto_pdo_entries + 5},\
};\
\
ec_sync_info_t ctrlproto_syncs[] = {\
    {0, EC_DIR_OUTPUT, 0, NULL, EC_WD_DISABLE},\
    {1, EC_DIR_INPUT, 0, NULL, EC_WD_DISABLE},\
    {2, EC_DIR_OUTPUT, 1, ctrlproto_pdos + 0, EC_WD_DISABLE},\
    {3, EC_DIR_INPUT, 1, ctrlproto_pdos + 1, EC_WD_DISABLE},\
    {0xff}\
};\




/**
 * This creates a entry for the domain entry register for a SOMANET device running CTRLPROTO
 * @param ALIAS The slaves alias
 * @param POSITION The position of the slave in the ethercat chain
 */
#define SOMANET_C22_CTRLPROTO_SLAVE_HANDLES_ENTRY(ALIAS, POSITION, CONFIG_NUMBER)\
{\
	{0,0,0,0,0},\
	{0,0,0,0,0},\
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},\
	ctrlproto_pdo_entries,\
	ctrlproto_pdos,\
	ctrlproto_syncs,\
	false,\
	false,\
	0,\
	ALIAS, POSITION,\
	SOMANET_ID,\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	{ 	{POLE_PAIRS(CONFIG_NUMBER), 0},\
		{GEAR_RATIO(CONFIG_NUMBER), 0},\
		{MAX_NOMINAL_SPEED(CONFIG_NUMBER), 0},\
		{MAX_NOMINAL_CURRENT(CONFIG_NUMBER), 0},\
		{MOTOR_TORQUE_CONSTANT(CONFIG_NUMBER), 0},\
		{MAX_ACCELERATION(CONFIG_NUMBER), 0},\
		{ENCODER_RESOLUTION(CONFIG_NUMBER), 0},\
		\
		{POLARITY(CONFIG_NUMBER), 0},\
		{SENSOR_SELECTION_CODE(CONFIG_NUMBER), 0},\
		\
		{(16384*VELOCITY_Kp_NUMERATOR(CONFIG_NUMBER) )/VELOCITY_Kp_DENOMINATOR(CONFIG_NUMBER), 0},\
		{(16384*VELOCITY_Ki_NUMERATOR(CONFIG_NUMBER) )/VELOCITY_Ki_DENOMINATOR(CONFIG_NUMBER), 0},\
		{(16384*VELOCITY_Kd_NUMERATOR(CONFIG_NUMBER) )/VELOCITY_Kd_DENOMINATOR(CONFIG_NUMBER), 0},\
		\
		{(16384*POSITION_Kp_NUMERATOR(CONFIG_NUMBER) )/POSITION_Kp_DENOMINATOR(CONFIG_NUMBER), 0},\
		{(16384*POSITION_Ki_NUMERATOR(CONFIG_NUMBER) )/POSITION_Ki_DENOMINATOR(CONFIG_NUMBER), 0},\
		{(16384*POSITION_Kd_NUMERATOR(CONFIG_NUMBER) )/POSITION_Kd_DENOMINATOR(CONFIG_NUMBER), 0},\
		\
		{(16384*TORQUE_Kp_NUMERATOR(CONFIG_NUMBER) )/TORQUE_Kp_DENOMINATOR(CONFIG_NUMBER), 0},\
		{(16384*TORQUE_Ki_NUMERATOR(CONFIG_NUMBER) )/TORQUE_Ki_DENOMINATOR(CONFIG_NUMBER), 0},\
		{(16384*TORQUE_Kd_NUMERATOR(CONFIG_NUMBER) )/TORQUE_Kd_DENOMINATOR(CONFIG_NUMBER), 0},\
		\
		{MIN_POSITION_LIMIT(CONFIG_NUMBER),0},\
		{MAX_POSITION_LIMIT(CONFIG_NUMBER),0},\
		\
		{MAX_TORQUE(CONFIG_NUMBER), 0},\
		{TORQUE_SLOPE(CONFIG_NUMBER), 0},\
		{MAX_PROFILE_VELOCITY(CONFIG_NUMBER), 0},\
		{PROFILE_ACCELERATION(CONFIG_NUMBER), 0},\
		{PROFILE_DECELERATION(CONFIG_NUMBER), 0},\
		{QUICK_STOP_DECELERATION(CONFIG_NUMBER), 0},\
		{PROFILE_VELOCITY(CONFIG_NUMBER), 0},\
		\
		{0, 0},\
		{0, 0},\
		{0, 0},\
		{0, 0},\
		\
		0},\
		0.0f,\
}


/**
 * This struct is for creating a slave handle for each Somanet Module
 */
typedef struct
{
	/**
	 * This links to the output variable inside the
	 * ec_pdo_entry_reg_t array for the somanet slave
	 */
	unsigned int __ecat_slave_out[5];


	/**
	 * This links to the input variable inside the
	 * ec_pdo_entry_reg_t array for the somanet slave
	 */
	unsigned int __ecat_slave_in[5];

	/**
	 * The SDO entries
	 */

	ec_sdo_request_t *__request[31];

	/**
	 * The PDO entries
	 */
	ec_pdo_entry_info_t *__pdo_entry;

	/**
	 * The PDO info
	 */
	ec_pdo_info_t *__pdo_info;

	/**
	 * The ec sync
	 */
	ec_sync_info_t *__sync_info;

	/**
	 * Internal variable for system start
	 */
	bool __start;

	/**
	 * The user variable to query for if the slave was responding
	 */
	bool is_responding;

	/**
	 * The slave config variable for each slave (see ethercat master for details)
	 */
	ec_slave_config_t *slave_config;

	/**
	 * The slave alias
	 */
	uint16_t slave_alias;

	/**
	 * The position of the slave in the chain
	 */
	uint16_t slave_pos;

	/**
	 * The VendorID
	 */
	uint32_t slave_vendorid;

	/**
	 * The ProductID
	 */
	uint32_t slave_productid;


	/**
	 * outgoing commands
	 */
	int16_t motorctrl_out;				/*only 16 bits valid*/

	/**
	 * outgoing torque (use fromFloatFunction to set it)
	 */
	int16_t torque_setpoint;			/*only 16 bits valid*/

	/**
	 * outgoing torque
	 */
	int32_t speed_setpoint;				/*only 32 bits valid*/

	/**
	 * outgoing position
	 */
	int32_t position_setpoint;			/*only 32 bits valid*/

	/**
	 * outgoing modes of operation
	 */
	int8_t operation_mode;				/*only 8 bits valid*/

	/**
	 * incoming motorctrl command (readback)
	 */
	int16_t motorctrl_status_in;		/*only 16 bits valid*/

	/**
	 * incoming torque
	 */
	int16_t torque_in;					/*only 16 bits valid*/

	/**
	 * incoming speed
	 */
	int32_t speed_in;					/*only 32 bits valid*/

	/**
	 * incoming position
	 */
	int32_t position_in;				/*only 32 bits valid*/

	/**
	 * incoming display mode of operation
	 */
	int8_t operation_mode_disp;		/*only 8 bits valid*/

	/**
	 * motor config struct
	 */
	motor_config motor_config_param; /*set via bldc_motor_config header file*/

	float factor_torq;

}ctrlproto_slv_handle;



/**
 * This creates a entry for the domain register for a SOMANET device running CTRLPROTO
 * @param ALIAS The slaves alias
 * @param POSITION The position of the slave in the ethercat chain
 * @param ARRAY_POSITION The position of the entry of the slave inside the handles array
 */
#define SOMANET_C22_CTRLPROTO_DOMAIN_REGS_ENTRIES(ALIAS, POSITION, ARRAY_POSITION)\
{ALIAS, POSITION, SOMANET_ID, CAN_OD_CONTROL_WORD, 		0, &(slv_handles[ARRAY_POSITION].__ecat_slave_out[0])},\
{ALIAS, POSITION, SOMANET_ID, CAN_OD_MODES, 			0, &(slv_handles[ARRAY_POSITION].__ecat_slave_out[1])},\
{ALIAS, POSITION, SOMANET_ID, CAN_OD_TORQUE_TARGET, 	0, &(slv_handles[ARRAY_POSITION].__ecat_slave_out[2])},\
{ALIAS, POSITION, SOMANET_ID, CAN_OD_POSITION_TARGET, 	0, &(slv_handles[ARRAY_POSITION].__ecat_slave_out[3])},\
{ALIAS, POSITION, SOMANET_ID, CAN_OD_VELOCITY_TARGET, 	0, &(slv_handles[ARRAY_POSITION].__ecat_slave_out[4])},\
{ALIAS, POSITION, SOMANET_ID, CAN_OD_STATUS_WORD, 	 	0, &(slv_handles[ARRAY_POSITION].__ecat_slave_in[0])},\
{ALIAS, POSITION, SOMANET_ID, CAN_OD_MODES_DISPLAY,  	0, &(slv_handles[ARRAY_POSITION].__ecat_slave_in[1])},\
{ALIAS, POSITION, SOMANET_ID, CAN_OD_POSITION_VALUE, 	0, &(slv_handles[ARRAY_POSITION].__ecat_slave_in[2])},\
{ALIAS, POSITION, SOMANET_ID, CAN_OD_VELOCITY_VALUE, 	0, &(slv_handles[ARRAY_POSITION].__ecat_slave_in[3])},\
{ALIAS, POSITION, SOMANET_ID, CAN_OD_TORQUE_VALUE, 	 	0, &(slv_handles[ARRAY_POSITION].__ecat_slave_in[4])}

typedef struct
{
	/**
	 * Flag for system start in handleEthercat
	 */
	bool first_run;

	/**
	 * Flag for operation (ethercat master domain checks)
	 */
	bool op_flag;

	/**
	 * Struct for ethercat master (see ethercat master for details)
	 */
	ec_master_t *master;

	/**
	 * Struct for ethercat master state (see ethercat master for details)
	 */
	ec_master_state_t master_state;

	/**
	 * Struct for ethercat domain (see ethercat master for details)
	 */
	ec_domain_t *domain;

	/**
	 * Struct for ethercat domain state (see ethercat master for details)
	 */
	ec_domain_state_t domain_state;

	/**
	 * domain_regs variable (see ethercat master for details)
	 */
	const ec_pdo_entry_reg_t *domain_regs;

	/**
	 * domain_pd variable (see ethercat master for details)
	 */
	uint8_t *domain_pd;

}master_setup_variables_t;



/**
 * Creates and initializes the master setup struct
 */
#define MASTER_SETUP_INIT()\
master_setup_variables_t master_setup={\
		false,false,NULL,{},0,{},domain_regs,NULL,\
};

/**
 * Initialises the Master and Slave communication
 *
 * \param master_setup 			A struct containing the variables for the master
 * \param slv_handles 			The handle struct for the slaves
 * \param total_no_of_slaves 	Number of connected slaves to the master
 */
void init_master(master_setup_variables_t *master_setup,
				 ctrlproto_slv_handle *slv_handles,
				 unsigned int total_no_of_slaves);

/**
 * This function handles the ethercat master communication,
 * it wraps around the master loop around the functions standing
 * below.
 *
 * \param master_setup 			A struct containing the variables for the master
 * \param slv_handles 			The handle struct for the slaves
 * \param total_no_of_slaves 	Number of connected slaves to the master
 */
void pdo_handle_ecat(master_setup_variables_t *master_setup,
        			ctrlproto_slv_handle *slv_handles,
        			unsigned int total_no_of_slaves);

/**
 * This function updates the motor parameters via ethercat
 *
 * \param master_setup 			A struct containing the variables for the master
 * \param slv_handles 			The handle array for the slaves *
 * \param update_sequence       Specify set of motor parameter to be configured
 * \param slave_number			Specify the slave number to which the motor is connected
 */
void sdo_handle_ecat(master_setup_variables_t *master_setup,
        			ctrlproto_slv_handle *slv_handles,
        			int update_sequence,
        			int slave_number);


#ifdef __cplusplus
}
#endif

#endif /* CTRLPROTO_M_H_ */
