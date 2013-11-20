
/**
 *
 * \file ethercat_setup.h
 *
 * \brief Ethercat Node Setup
 *
 *	Please define your the node structure and configuration for each node.
 *
 * Copyright (c) 2013, Synapticon GmbH
 * All rights reserved.
 * Author: Christian Holl <choll@synapticon.com>
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

#ifndef ETHERCAT_SETUP_H_
#define ETHERCAT_SETUP_H_

/**
 * Number of SOMANET Slaves
 */
#define TOTAL_NUM_OF_SLAVES 1

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
