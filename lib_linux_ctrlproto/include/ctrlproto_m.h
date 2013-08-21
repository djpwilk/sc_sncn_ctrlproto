/*
 * ctrlproto_m.h
 *
 *  Created on: 05.02.2013
 *      Author: cyborg-x1
 */

#ifndef CTRLPROTO_M_H_
#define CTRLPROTO_M_H_

#include <stdbool.h>
#include <ecrt.h>
#include <inttypes.h>

//#include <cmds.h>


#ifdef __cplusplus
extern "C"
{
#endif

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
#define SOMANET_C22_CTRLPROTO_SLAVE_HANDLES_ENTRY(ALIAS, POSITION)\
{\
	{0,0,0,0,0},\
	{0,0,0,0,0},\
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
	int motorctrl_out;				/*only 16 bits valid*/

	/**
	 * outgoing torque (use fromFloatFunction to set it)
	 */
	int torque_setpoint;			/*only 16 bits valid*/

	/**
	 * outgoing torque
	 */
	int speed_setpoint;				/*only 32 bits valid*/

	/**
	 * outgoing position
	 */
	int position_setpoint;			/*only 32 bits valid*/

	/**
	 * outgoing modes of operation
	 */
	int operation_mode;				/*only 8 bits valid*/

	/**
	 * incoming motorctrl command (readback)
	 */
	int motorctrl_status_in;		/*only 16 bits valid*/

	/**
	 * incoming torque
	 */
	int torque_in;					/*only 16 bits valid*/

	/**
	 * incoming speed
	 */
	int speed_in;					/*only 32 bits valid*/

	/**
	 * incoming position
	 */
	int position_in;				/*only 32 bits valid*/

	/**
	 * incoming display mode of operation
	 */
	int operation_mode_disp;		/*only 8 bits valid*/

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
	bool nFirstRun;

	/**
	 * Flag for operation (ethercat master domain checks)
	 */
	bool opFlag;

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


void init_master(master_setup_variables_t *master_setup,
				 ctrlproto_slv_handle *slv_handles,
				 unsigned int slave_num);

/**
 * This function handles the ethercat master communication,
 * it wraps around the master loop around the functions standing
 * below.
 * @param master_setup A struct containing the variables for the master
 * @param slv_handles The handle array for the slaves
 * @param slave_num The size of the handle array
 */
void handle_ecat(master_setup_variables_t *master_setup,
        		ctrlproto_slv_handle *slv_handles,
        		unsigned int slave_num);


/**
 * Multiplies a float value with 1000 and outputs it as 16 Bit integer.
 * It is used to transfer a float value to a slave, mainly used for torque.
 * @param value The float value
 * @return Float multiplied with 1000
 */
int32_t fromFloat(float value);

/**
 * Devides a 16 Bit integer by 1000 and returns a float value.
 * It is used receive a float value from a slave.
 * @param value Integer value from slave (torque)
 * @return Integer value
 */
float toFloat(int32_t value);

#ifdef __cplusplus
}
#endif

#endif /* CTRLPROTO_M_H_ */
