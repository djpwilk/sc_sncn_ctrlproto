How to write an application for a single axis motor control over EtheCAT 
============================
<img align="left" src="https://s3-eu-west-1.amazonaws.com/synapticon-resources/images/logos/synapticon_fullname_blackoverwhite_280x48.png"/>
<br/>
<br/>

* All the Somanet slaves must be running the default [test_ethercat-mode](https://github.com/synapticon/sc_sncn_motorctrl_sin/tree/master/test_ethercat-motorctrl-mode) application. We Provide sample master application for each of the EtherCAT operation modes for single slave configuration.

* Define the motor configurations at [bldc_motor_config_1.h](https://github.com/synapticon/sc_sncn_ctrlproto/blob/master/app_linux_ctrlproto-csp-example/config/motor/bldc_motor_config_1.h)

* Define the slave configuration at [ethercat_setup.h](https://github.com/synapticon/sc_sncn_ctrlproto/blob/master/app_linux_ctrlproto-csp-example/src/ethercat_setup.h) (slave id : 0)

Now the main application for controlling the motor over EtherCAT can be written as

```
/* Initialize Ethercat Master */ 
init_master(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES); 

/* Initialize torque parameters */ 
initialize_torque(slave_number, slv_handles); 

/* Initialize all connected nodes with Mandatory Motor Configurations */ 
init_nodes(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES); 

/* Initialize the node specified with slave_number with CSP configurations */ 
set_operation_mode(CSP, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES); 

/* Enable operation of node in the mode selected */ 
enable_operation(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);
```

* The EtherCAT Operation mode for the slave can be changed with the function  *set_operation_mode*

* The command variables/sensor values of the motor can be set or accessed using set/get functions defined at [drive_function.h](https://github.com/synapticon/sc_sncn_ctrlproto/blob/master/lib_linux_motor_drive/include/drive_function.h)

* The process data update function pdo_handle_ecat must always be called once along with the set/get functions.

* The Frequency of EtherCAT update can be changed at [ctrlproto_m.h](https://github.com/synapticon/sc_sncn_ctrlproto/blob/master/lib_linux_ctrlproto/include/ctrlproto_m.h)


