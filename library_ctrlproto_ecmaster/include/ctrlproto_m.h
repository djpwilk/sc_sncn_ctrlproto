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


#ifdef __cplusplus
extern "C"
{
#endif

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
	uint16_t motorctrl_cmd_out;

	/**
	 * outgoing torque
	 */
	uint32_t torque_out;

	/**
	 * outgoing torque
	 */
	uint32_t speed_out;

	/**
	 * outgoing position
	 */
	uint32_t position_out;

	/**
	 * outgoing userdefined value
	 */
	uint32_t userdef_out;

	/**
	 * outgoing commands
	 */
	uint16_t motorctrl_cmd_in;

	/**
	 * incoming torque
	 */
	uint32_t torque_in;

	/**
	 * incoming speed
	 */
	uint32_t speed_in;

	/**
	 * incoming position
	 */
	uint32_t position_in;

	/**
	 * incoming userdefined value
	 */
	uint32_t userdef_in;

}ctrlproto_slv_handle;

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
void handleEcat(master_setup_variables_t *master_setup,
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
