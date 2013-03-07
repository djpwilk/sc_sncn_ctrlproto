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
#include <ctrlproto_command.h>

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
	unsigned int __ecat_slave_out_1;
	unsigned int __ecat_slave_out_2;

	/**
	 * This links to the input variable inside the
	 * ec_pdo_entry_reg_t array for the somanet slave
	 */
	unsigned int __ecat_slave_in_1;
	unsigned int __ecat_slave_in_2;

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
	 * This variable counts lost packages for each slave,
	 * when output heartbeat did not change between the last iteration
	 */
	uint64_t __lost_packages_cnt;

	/**
	 * Internal variable for system start
	 */
	bool __start;

	/**
	 * The heartbeat value from last iteration
	 */
	uint8_t __last_heartbeat_value;

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
	 * Variable for data received from the slave (2 times 16 Bit)
	 */
	uint16_t out[2];

	/**
	 * Variable for outgoing data
	 */
	uint16_t in[2];
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

/**
 * This function handles the ethercat master communication,
 * it wraps around the master loop around the functions standing
 * below.
 * @param master_setup A struct containing the variables for the master
 * @param slv_handles The handle array for the slaves
 * @param slave_num The size of the handle array
 * @param pause_betw_loops The variable defining the sleep time between each loop
 */
void handleEcat(master_setup_variables_t *master_setup,
		        ctrlproto_slv_handle *slv_handles,
		        unsigned int slave_num,
		        unsigned int pause_betw_loops);

/**
 * Multiplies a float value with 1000 and outputs it as 16 Bit integer.
 * It is used to transfer a float value to a slave, mainly used for torque.
 * @param value The float value
 * @return Float multiplied with 1000
 */
int16_t fromFloat(float value);

/**
 * Devides a 16 Bit integer by 1000 and returns a float value.
 * It is used receive a float value from a slave.
 * @param value Integer value from slave (torque)
 * @return Integer value
 */
float toFloat(int16_t value);

/**
 * Sends a command to a slave.
 *
 * @param slave_no Number of the slave handle inside the array (NO SIZE CHECK!).
 * @param cmd a command defined in the given struct
 * @param value The value to be sent with the command
 * @param force_write This will write the value, even if the slave is not responding
 * @param slv_handles The array for the slave handles
 */
bool setSlave(unsigned int slave_no, ctrl_proto_xmos_cmd_t cmd, int16_t value, bool force_write, ctrlproto_slv_handle *slv_handles);

/**
 * @param slave_no Number of the slave handle inside the array (NO SIZE CHECK!).
 * @param cmd a command defined in the given struct, only will contain commands beginning with GET
 * @param value The read value
 * @param slv_handles The array for the slave handles
 */
bool getSlave(unsigned int slave_no, ctrl_proto_xmos_cmd_t *what, int16_t *value, ctrlproto_slv_handle *slv_handles);


#ifdef __cplusplus
}
#endif

#endif /* CTRLPROTO_M_H_ */
