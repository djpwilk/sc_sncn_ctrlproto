How to write an application for a multi axis motor control over EtheCAT
============================
<img align="left" src="https://s3-eu-west-1.amazonaws.com/synapticon-resources/images/logos/synapticon_fullname_blackoverwhite_280x48.png"/>
<br/>
<br/>

* Define the slave configuration at [ethercat_setup.h](https://github.com/synapticon/sc_sncn_ctrlproto/blob/master/howto/ethercat_setup_3_nodes.h): for example with 3 slaves 
* Define all the motor configurations at [bldc_motor_config_N.h](https://github.com/synapticon/sc_sncn_ctrlproto/tree/master/app_linux_ctrlproto-csp-example/config/motor) (N: 1, 2, 3)

> If all motor are the same you can have a single configuration file.

Now the main application for controlling the motor over EtherCAT can be written in the similar way as for a [single motor axis](https://github.com/synapticon/sc_sncn_ctrlproto/blob/master/howto/HOW_TO_SINGLE_AXIS.md) except that we need to define modes, set/get command/sensor variables for all connected slaves.


