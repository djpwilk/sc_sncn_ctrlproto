#ifndef DRIVE_CONFIG_H_
#define DRIVE_CONFIG_H_
#pragma once

/**
 * \brief Modes of Operation (CiA402)
 *
 * 	M - Mandatory, C - Conditional, R - Recommended, O - optional , FG - Function Group
 */

#define	PP		1 	/* Profile Position mode 									O*/
#define VL 		2	/* Velocity mode (frequency converter) 						O*/
#define PV 		3   /* Profile velocity mode 									O*/
#define TQ 		4   /* Torque profile mode 										O*/
#define HM 		6	/* Homing mode 												O*/
#define IP 		7	/* Interpolated position mode 								O*/
#define CSP 	8	/* Cyclic synchronous position mode 						C*/
#define CSV 	9	/* Cyclic synchronous velocity mode 						C*/
#define CST 	10	/* Cyclic synchronous torque mode 							C*/
#define CSTCA 	11	/* Cyclic synchronous torque mode with commutation angle 	O*/

/* Manufacturer specific mode -128...-1 optional */

/* Controlword */

//Common for all Modes of Operation (CiA402)

#define SHUTDOWN				0x0006
#define SWITCH_ON				0x000F
#define QUICK_STOP  			0x000B
#define CLEAR_FAULT 			0x0080

//Operation Mode specific control words (complies with CiA402)

/* Homing mode */
#define START_HOMING 			0x001F
#define HALT_HOMING  			0x011F

/* Profile Position Mode */
#define ABSOLUTE_POSITIONING 	0x001F	 	// not supported yet
#define RELATIVE_POSITIONING 	0x005F   	// supported currently
#define STOP_POSITIONING		0x010F

/*Profile Velocity Mode*/
#define HALT_PROFILE_VELOCITY	0x010F

/* Statusword */
//state defined is ORed with current state

#define TARGET_REACHED 			0x0400

/* Homing Mode */
#define HOMING_ATTAINED			0x1000

/* Profile Position Mode */
#define SET_POSITION_ACK	  	0x1000

/* Profile Velocity Mode */
#define TARGET_VELOCITY_REACHED 0x0400

/*Controlword Bits*/
#define SWITCH_ON_CONTROL					0x1
#define ENABLE_VOLTAGE_CONTROL				0x2
#define QUICK_STOP_CONTROL					0x4
#define ENABLE_OPERATION_CONTROL			0x8
#define OPERATION_MODES_SPECIFIC_CONTROL	0x70  /*3 bits*/
#define FAULT_RESET_CONTROL					0x80
#define HALT_CONTROL						0x100
#define OPERATION_MODE_SPECIFIC_CONTROL		0x200
#define RESERVED_CONTROL					0x400
#define MANUFACTURER_SPECIFIC_CONTROL		0xf800

/*Statusword Bits*/
#define READY_TO_SWITCH_ON_STATE	  		0X1
#define SWITCHED_ON_STATE					0X2
#define OPERATION_ENABLED_STATE				0X4
#define FAULT_STATE							0X8
#define VOLTAGE_ENABLED_STATE				0X10
#define QUICK_STOP_STATE					0X20
#define SWITCH_ON_DISABLED_STATE			0X40
#define WARNING_STATE						0X80
#define MANUFACTURER_SPECIFIC_STATE			0X100
#define REMOTE_STATE						0X200
#define TARGET_REACHED_OR_RESERVED_STATE	0X400
#define INTERNAL_LIMIT_ACTIVE_STATE			0X800
#define OPERATION_MODE_SPECIFIC_STATE		0X1000	// 12 CSP/CSV/CST  13
#define MANUFACTURER_SPECIFIC_STATES   		0XC000	// 14-15

extern int init_state(void);

extern int update_statusword(int current_status, int state_reached);

extern int get_next_values(int in_state, int check_init, int ctrl_input, int fault);

extern int read_controlword_switch_on(int control_word);

extern int read_controlword_quick_stop(int control_word);

extern int read_controlword_enable_op(int control_word);

extern int read_controlword_fault_reset(int control_word);

#endif /* DRIVE_CONFIG_H_*/

