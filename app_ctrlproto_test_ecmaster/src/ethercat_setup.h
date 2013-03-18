#ifndef ETHERCAT_SETUP_H_
#define ETHERCAT_SETUP_H_

/**
 * Number of SOMANET Slaves
 */
#define NUM_SLAVES 1

SOMANET_C22_CTRLPROTO_CSTRUCT();

//Slave Handles Array
static ctrlproto_slv_handle slv_handles[]=
{												//ALIAS / POSITION
		SOMANET_C22_CTRLPROTO_SLAVE_HANDLES_ENTRY(0,             0)
};

//Domain entries for the pdos
const static ec_pdo_entry_reg_t domain_regs[] = {
		 	 	 	 	 	 	 	 	 	 	 //ALIAS / POSITION
		SOMANET_C22_CTRLPROTO_DOMAIN_REGS_ENTRIES(0, 			  0),
{}
};

//This keeps the variables needed for the master
master_setup_variables_t master_setup={
		false,false,NULL,{},0,{},domain_regs,NULL,
};

#endif /* ETHERCAT_SETUP_H_ */
