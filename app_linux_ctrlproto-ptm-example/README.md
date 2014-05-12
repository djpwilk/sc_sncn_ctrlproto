Profile Torque Mode over EtherCAT Demo
=========================
<a href="https://github.com/synapticon/sc_sncn_motorctrl_sin/blob/master/SYNAPTICON.md">
<img align="left" src="https://s3-eu-west-1.amazonaws.com/synapticon-resources/images/logos/synapticon_fullname_blackoverwhite_280x48.png"/>
</a>
<br/>
<br/>

This application provides example Master Application for Profile Torque Mode (host side)
for a single connected node. The nodes must be running test_ethercat-motorctrl-mode from
sc_sncn_motorctrl_sin before the Master application runs.

In Profile Torque Mode, the user can set a new target torque for the node. The torque 
profiles for the target torque are generated on the node/slave side with millisecond steps.
And wait until ack and desired tolerance in torque is reached by the controller.

The motor configuration and control parameter for each node connected to the motor must be specified 
at config/motor/bldc_motor_config_N.h

Dependencies: sc_sncn_motorctrl_sin, lib_linux_ctrlproto, lib_linux_motor_drive

NOTE: The application requires EtherCAT Master for Linux from IGH to be installed on your PC. The 
node configuration must be defined in ethercat_setup.h


