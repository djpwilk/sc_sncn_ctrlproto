#ifndef COMMAND_H_
#define COMMAND_H_

#ifdef __cplusplus
extern "C"
{
#endif

	/**
	 * This enum holds the protocol commands (first PDO)
	 */
	typedef enum
	{
		CTRL_STOP=0x0000,		//!< Stops the Motor (do not use any other flags)
		CTRL_SPEED=0x0001,		//!< Enables speed control (if used with position, speed setting is the maximum speed value)
		CTRL_POSITION=0x0002,	//!< Enables position based control
		CTRL_TORQUE=0x0004,	//!< Enables torque control (if used with position, torque setting is the maximum torque value)
		__ENUM_MC_END=CTRL_SPEED+CTRL_POSITION+CTRL_TORQUE, //!< __ENUM_MC_END

		SET_SETPOINT_SPEED,		//!< Sets the speed value for control
		SET_SETPOINT_TORQUE,	//!< Sets the torque value for control
		SET_SETPOINT_POSITION,	//!< Sets the position value for control
		__ENUM_SET_END,//!< __ENUM_SET_END

		GET_POSITION,			//!< Sets the output to current position
		GET_TORQUE,				//!< Sets the output to current torque
		GET_SPEED,				//!< Sets the output to current speed
		GET_SETPOINT_POSITION,	//!< Sets the output to the set point of position
		GET_SETPOINT_TORQUE, 	//!< Sets the output to the set point of torque
		GET_SETPOINT_SPEED,		//!< Sets the output to the set point of speed
		GET_CUSTOM,				//!< Sets the output to the custom value
		__ENUM_OUT_END,//!< __ENUM_OUT_END

		__USER_CUSTOM_COMMANDS	//!< This can be used to create a user based command set in another enum, every command will be placed in customcommand and the value in customvalue
	}ctrl_proto_xmos_cmd_t;

#ifdef __cplusplus
}
#endif

#endif /* COMMAND_H_ */
