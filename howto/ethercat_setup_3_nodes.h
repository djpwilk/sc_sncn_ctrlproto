
/**
* \file ethercat_setup.h
* \brief Ethercat Node Setup: Please define your the node structure and configuration for each node.
* \author Christian Holl <choll@synapticon.com>
* \version 1.0
* \date 10/04/2014
*/

#ifndef ETHERCAT_SETUP_H_
#define ETHERCAT_SETUP_H_

/**
* Number of SOMANET Slaves
*/
#define TOTAL_NUM_OF_SLAVES 3

SOMANET_C22_CTRLPROTO_CSTRUCT();

/* Slave Handles Array for single node
Note: Each array entry defines handle struct for a node */
static ctrlproto_slv_handle slv_handles[]=
{						//ALIAS / POSITION / CONFIG_NUMBER
	SOMANET_C22_CTRLPROTO_SLAVE_HANDLES_ENTRY(0, 		0, 		1),
	SOMANET_C22_CTRLPROTO_SLAVE_HANDLES_ENTRY(0,            1, 		2),
	SOMANET_C22_CTRLPROTO_SLAVE_HANDLES_ENTRY(0,            2, 		3),
};

/* Domain entries for the pdos
* Note: Each array entry define pdos for a node
*/
const static ec_pdo_entry_reg_t domain_regs[] = {
						//ALIAS / POSITION / ARRAY POSITION inside SLV_HANDLES
	SOMANET_C22_CTRLPROTO_DOMAIN_REGS_ENTRIES(0,		 0,		0),
	SOMANET_C22_CTRLPROTO_DOMAIN_REGS_ENTRIES(0,		 1,		1),
	SOMANET_C22_CTRLPROTO_DOMAIN_REGS_ENTRIES(0,		 2,		2),
{0}
};

/*
* Master setup struct
*/
MASTER_SETUP_INIT();


/**
* Increase priority of the master process
* !! YOU WILL NEED TO RUN THIS AS ROOT OTHERWISE THE PRIORITY WILL NOT CHANGE!!
*/
#define PRIORITY

#endif /* ETHERCAT_SETUP_H_ */
