#ifndef ETHERCAT_SETUP_H_
#define ETHERCAT_SETUP_H_

/**
 * Number of SOMANET Slaves
 */
#define NUM_SLAVES 1

SOMANET_C22_CTRLPROTO_CSTRUCT();

//Slave Handles Array
static ctrlproto_slv_handle slv_handles[]=
{												//ALIAS / POSITION / CONFIG_NUMBER
		SOMANET_C22_CTRLPROTO_SLAVE_HANDLES_ENTRY(0,             0, 	1)
};

//Domain entries for the pdos
const static ec_pdo_entry_reg_t domain_regs[] = {
												//ALIAS / POSITION / ARRAY POSITION inside SLV_HANDLES
		SOMANET_C22_CTRLPROTO_DOMAIN_REGS_ENTRIES(0,		0,			0),
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
