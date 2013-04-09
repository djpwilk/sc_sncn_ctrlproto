/*
 * cmds.h
 *
 *  Created on: 15.03.2013
 *      Author: choll
 */

#ifndef CMDS_H_
#define CMDS_H_

	typedef enum
	{
		/**
		 * Stops the motor
		 */
		CTRL_STOP=0x0,

		/**
		 * Enable position control flag
		 */
		CTRL_F_POSITION=0x1,

		/**
		 * Enable speed control flag
		 */
		CTRL_F_SPEED=0x2,

		/**
		 * Enable torque control flag
		 */
		CTRL_F_TORQUE=0x4,
	}ctrlproto_cmds;

#endif /* CMDS_H_ */
