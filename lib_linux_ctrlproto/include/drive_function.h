#include <drive_config.h>
#include "ctrlproto_m.h"

/*External functions*/
int set_operation_mode(int operation_mode, int slave_number, master_setup_variables_t *master_setup, ctrlproto_slv_handle *slv_handles, int total_no_of_slaves);

int enable_operation(int slave_number, master_setup_variables_t *master_setup, ctrlproto_slv_handle *slv_handles, int total_no_of_slaves);

int quick_stop_position(int slave_number, master_setup_variables_t *master_setup, ctrlproto_slv_handle *slv_handles, int total_no_of_slaves);

int renable_ctrl(int operation_mode, int slave_number, master_setup_variables_t *master_setup, ctrlproto_slv_handle *slv_handles, int total_no_of_slaves);

int shutdown_operation(int operation_mode, int slave_number, master_setup_variables_t *master_setup, ctrlproto_slv_handle *slv_handles, int total_no_of_slaves);

int quick_stop_velocity(int slave_number, master_setup_variables_t *master_setup, ctrlproto_slv_handle *slv_handles, int total_no_of_slaves);

void set_velocity(int target_velocity, int slave_number, ctrlproto_slv_handle *slv_handles);

int get_velocity_actual(int slave_number, ctrlproto_slv_handle *slv_handles);

int get_position_actual_deg(int slave_number, ctrlproto_slv_handle *slv_handles);

void set_position_deg(int target_position, int slave_number, ctrlproto_slv_handle *slv_handles);


/*Internal functions*/
int check_ready(int status_word);

int check_switch_enable(int status_word);

int check_switch_on(int status_word);

int check_op_enable(int status_word);

int check_quick_stop_active(int status_word);

int check_target_reached(int status_word);

void set_controlword(int controlword, int slave_number, ctrlproto_slv_handle *slv_handles);

int read_statusword(int slave_number, ctrlproto_slv_handle *slv_handles);




