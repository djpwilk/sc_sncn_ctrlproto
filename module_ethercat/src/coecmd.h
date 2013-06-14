/**
 * Defines and definitions for applications commands to handle object
 * dictionary entries.
 *
 * 2013 Synapticon GmbH
 *
 * Author: Frank Jeschke <fjeschke@synapticon.de>
 *
 * DRAFT
 */

#ifndef COECMD_H
#define COECMD_H

/* Communication via channel between application and EtherCAT module.
 *
 * Main purpose is the exchange of data objects, optionaly object description
 * resp. object type (at least).
 */

#define CAN_GET_OBJECT    0x1
#define CAN_SET_OBJECT    0x2
#define CAN_OBJECT_TYPE   0x3
#define CAN_MAX_SUBINDEX  0x4

/* command structure:
 * app -> ecat/coe:
 * CAN_GET_OBJECT index.subindex 
 * ecat -> app
 * value
 *
 * CAN_SET_OBJECT index.subindex value
 * ecat->app: value | errorcode
 *
 * CAN_MAX_SUBINDEX index.00=subindex 
 * ecat->app: max_subindex
 */

#define CAN_OBJ_ADR(i,s)   (((unsigned)i<<8) | s)

/* Error symbols */

#define CAN_ERROR           0xff01
#define CAN_ERROR_UNKNOWN   0xff02

#endif /* COECMD_H */
