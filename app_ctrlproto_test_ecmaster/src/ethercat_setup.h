#ifndef ETHERCAT_SETUP_H_
#define ETHERCAT_SETUP_H_


#define NUM_SLAVES 1

//definitions for the bus connections and expected slave positions
#define SOMANET_0_Pos 0,0

//Vendor ID, Product Code  (see Output of ethercat cstruct)
#define SOMANET_ID 0x000022d2, 0x00000201


/* Master 0, Slave 0, "Synapticon-ECAT"
 * Vendor ID:       0x000022d2
 * Product code:    0x00000201
 * Revision number: 0x0a000002
 */

ec_pdo_entry_info_t slave_0_pdo_entries[] = {
    {0x6000, 0x01, 16}, /*  */
    {0x6000, 0x02, 16}, /*  */
    {0x7000, 0x01, 16}, /*  */
    {0x7000, 0x02, 16}, /*  */
};

ec_pdo_info_t slave_0_pdos[] = {
    {0x1600, 2, slave_0_pdo_entries + 0}, /* Rx PDO Mapping */
    {0x1a00, 2, slave_0_pdo_entries + 2}, /* Tx PDO Mapping */
};

ec_sync_info_t slave_0_syncs[] = {
    {0, EC_DIR_OUTPUT, 0, NULL, EC_WD_DISABLE},
    {1, EC_DIR_INPUT, 0, NULL, EC_WD_DISABLE},
    {2, EC_DIR_OUTPUT, 1, slave_0_pdos + 0, EC_WD_DISABLE},
    {3, EC_DIR_INPUT, 1, slave_0_pdos + 1, EC_WD_DISABLE},
    {0xff}
};

static unsigned int somanet_0_o_offset;

//Slave Handles Array
static ctrlproto_slv_handle slv_handles[]=
{
		//Slave 0
		{
			0,
			0,
			0,
			0,
			slave_0_pdo_entries,
			slave_0_pdos,
			slave_0_syncs,
			0,
			false,
			0,
			false,
			0,
			0x0,//Alias
			0x0,//Slave Position
			SOMANET_ID,
			{0,0},
			{0,0}
		},
};


/*
 * Use the starting addresses for the digital I/O (see Output of ethercat -> pdo_entries[] )
 */
const static ec_pdo_entry_reg_t domain_regs[] = {
{SOMANET_0_Pos, SOMANET_ID, 0x6000, 1, &slv_handles[0].__ecat_slave_out_1},
{SOMANET_0_Pos, SOMANET_ID, 0x6000, 2, &slv_handles[0].__ecat_slave_out_2},
{SOMANET_0_Pos, SOMANET_ID, 0x7000, 1, &slv_handles[0].__ecat_slave_in_1},
{SOMANET_0_Pos, SOMANET_ID, 0x7000, 2, &slv_handles[0].__ecat_slave_in_2},
{}
};

//##################################################//
//##################################################//
//##################################################//
//##################################################//
//##################################################//

//This keeps the variables needed for the master
master_setup_variables_t master_setup={
		false,false,NULL,{},0,{},domain_regs,NULL,
};






#endif /* ETHERCAT_SETUP_H_ */
