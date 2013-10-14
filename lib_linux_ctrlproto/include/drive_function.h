#include <drive_config.h>
#include "ctrlproto_m.h"
#include "profile.h"

/*External functions*/

/**
 * \brief Sets operation mode via defines for modes of operation in drive_config.h
 *
 * \param operation_mode        Specify the operation mode for the slave
 * \param slave_number			Specify the slave number to which the motor is connected
 * \param master_setup 			A struct containing the variables for the master
 * \param slv_handles 			The handle struct for the slaves
 * \param total_no_of_slaves 	Number of connected slaves to the master
 */
int set_operation_mode(int operation_mode, int slave_number, master_setup_variables_t *master_setup, ctrlproto_slv_handle *slv_handles, int total_no_of_slaves);


/**
 * \brief Enables the operation mode specified
 *
 * \param slave_number			Specify the slave number to which the motor is connected
 * \param master_setup 			A struct containing the variables for the master
 * \param slv_handles 			The handle struct for the slaves
 * \param total_no_of_slaves 	Number of connected slaves to the master
 */
int enable_operation(int slave_number, master_setup_variables_t *master_setup, ctrlproto_slv_handle *slv_handles, int total_no_of_slaves);


/**
 * \brief Shuts down the operation mode selected
 *
 * \param operation_mode        Specify the operation mode selected for the slave
 * \param slave_number			Specify the slave number to which the motor is connected
 * \param master_setup 			A struct containing the variables for the master
 * \param slv_handles 			The handle struct for the slaves
 * \param total_no_of_slaves 	Number of connected slaves to the master
 */
int shutdown_operation(int operation_mode, int slave_number, master_setup_variables_t *master_setup, ctrlproto_slv_handle *slv_handles, int total_no_of_slaves);


/**
 * \brief Sets target position cyclically for Cyclic Synchronous Position(CSP) mode only
 *
 * \param target_position 		Specify the target position to follow (in degree)
 * \param slave_number			Specify the slave number to which the motor is connected
 * \param slv_handles 			The handle struct for the slaves
 */
void set_position_deg(int target_position, int slave_number, ctrlproto_slv_handle *slv_handles);


/**
 * \brief Sets target position for Profile Position mode(PPM) only
 *
 * \param target_position		Specify the target position to follow (in degree)
 * \param slave_number			Specify the slave number to which the motor is connected
 * \param slv_handles 			The handle struct for the slaves
 */
void set_profile_position_deg(float target_position, int slave_number, ctrlproto_slv_handle *slv_handles);


/**
 * \brief Gets actual position
 *
 * \param slave_number			Specify the slave number to which the motor is connected
 * \param slv_handles 			The handle struct for the slaves
 *
 * \return actual_position from the slave number specified
 */
float get_position_actual_deg(int slave_number, ctrlproto_slv_handle *slv_handles);


/**
 * \brief Quick Stop Position
 *
 * \param slave_number			Specify the slave number to which the motor is connected
 * \param master_setup 			A struct containing the variables for the master
 * \param slv_handles 			The handle struct for the slaves
 * \param total_no_of_slaves 	Number of connected slaves to the master
 */
int quick_stop_position(int slave_number, master_setup_variables_t *master_setup, ctrlproto_slv_handle *slv_handles, int total_no_of_slaves);


/**
 * \param operation_mode        Specify the operation mode selected for the slave
 * \param slave_number			Specify the slave number to which the motor is connected
 * \param master_setup 			A struct containing the variables for the master
 * \param slv_handles 			The handle struct for the slaves
 * \param total_no_of_slaves 	Number of connected slaves to the master
 */
int renable_ctrl_quick_stop(int operation_mode, int slave_number, master_setup_variables_t *master_setup, ctrlproto_slv_handle *slv_handles, int total_no_of_slaves);


/**
 * \param slave_number			Specify the slave number to which the motor is connected
 * \param slv_handles 			The handle struct for the slaves
 */
int position_set_flag(int slave_number, ctrlproto_slv_handle *slv_handles);


/**
 * \param slave_number			Specify the slave number to which the motor is connected
 * \param target_position		Specify the target position set (in degree)
 * \param tolerance 			Specify the tolerance for target position
 * \param slv_handles 			The handle struct for the slaves
 */
int target_position_reached(int slave_number, float target_position, float tolerance, ctrlproto_slv_handle *slv_handles);


int init_position_profile_params(float target_position, float actual_position, int velocity, int acceleration, int deceleration);


/**
 * \brief Sets target velocity for Profile Velocity mode(PPM) & Cyclic Synchronous Velocity(CSV) mode
 *
 * \param target_velocity		Specify the target velocity to follow (in rpm)
 * \param slave_number			Specify the slave number to which the motor is connected
 * \param slv_handles 			The handle struct for the slaves
 */
void set_velocity(int target_velocity, int slave_number, ctrlproto_slv_handle *slv_handles);


/**
 * \brief Gets actual velocity
 *
 * \param slave_number			Specify the slave number to which the motor is connected
 * \param slv_handles 			The handle struct for the slaves
 *
 * \return actual_velocity in rpm from the slave number specified
 */
int get_velocity_actual(int slave_number, ctrlproto_slv_handle *slv_handles);


/**
 * \brief Quick Stop Velocity
 *
 * \param slave_number			Specify the slave number to which the motor is connected
 * \param master_setup 			A struct containing the variables for the master
 * \param slv_handles 			The handle struct for the slaves
 * \param total_no_of_slaves 	Number of connected slaves to the master
 */
int quick_stop_velocity(int slave_number, master_setup_variables_t *master_setup, ctrlproto_slv_handle *slv_handles, int total_no_of_slaves);


/**
 * \param slave_number			Specify the slave number to which the motor is connected
 * \param target_velocity		Specify the target velocity set (in rpm)
 * \param tolerance 			Specify the tolerance for target velocity
 * \param slv_handles 			The handle struct for the slaves
 */
int target_velocity_reached(int slave_number, int target_velocity, int tolerance, ctrlproto_slv_handle *slv_handles);


/**
 * \param slave_number			Specify the slave number to which the motor is connected
 * \param slv_handles 			The handle struct for the slaves
 */
int velocity_set_flag(int slave_number, ctrlproto_slv_handle *slv_handles);






