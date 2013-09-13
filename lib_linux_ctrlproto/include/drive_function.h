#include <drive_config.h>
#include "ctrlproto_m.h"
#include "profile.h"

/*External functions*/
int set_operation_mode(int operation_mode, int slave_number, master_setup_variables_t *master_setup, ctrlproto_slv_handle *slv_handles, int total_no_of_slaves);

int enable_operation(int slave_number, master_setup_variables_t *master_setup, ctrlproto_slv_handle *slv_handles, int total_no_of_slaves);

int shutdown_operation(int operation_mode, int slave_number, master_setup_variables_t *master_setup, ctrlproto_slv_handle *slv_handles, int total_no_of_slaves);


void set_position_deg(int target_position, int slave_number, ctrlproto_slv_handle *slv_handles);

void set_profile_position_deg(float target_position, int slave_number, ctrlproto_slv_handle *slv_handles);

float get_position_actual_deg(int slave_number, ctrlproto_slv_handle *slv_handles);

int quick_stop_position(int slave_number, master_setup_variables_t *master_setup, ctrlproto_slv_handle *slv_handles, int total_no_of_slaves);

int renable_ctrl_quick_stop(int operation_mode, int slave_number, master_setup_variables_t *master_setup, ctrlproto_slv_handle *slv_handles, int total_no_of_slaves);

int position_set_flag(int slave_number, ctrlproto_slv_handle *slv_handles);

int target_position_reached(int slave_number, float target_position, float tolerance, ctrlproto_slv_handle *slv_handles);

int init_position_profile_params(float target_position, float actual_position, int velocity, int acceleration, int deceleration);


void set_velocity(int target_velocity, int slave_number, ctrlproto_slv_handle *slv_handles);

int get_velocity_actual(int slave_number, ctrlproto_slv_handle *slv_handles);

int quick_stop_velocity(int slave_number, master_setup_variables_t *master_setup, ctrlproto_slv_handle *slv_handles, int total_no_of_slaves);

int target_velocity_reached(int slave_number, int target_velocity, int tolerance, ctrlproto_slv_handle *slv_handles);

int velocity_set_flag(int slave_number, ctrlproto_slv_handle *slv_handles);






