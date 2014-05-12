Control Protocol SOFTWARE COMPONENT
===============
<img align="left" src="https://s3-eu-west-1.amazonaws.com/synapticon-resources/images/logos/synapticon_fullname_blackoverwhite_280x48.png"/>
<br/>
<br/>

Implementation of Communication protocol layer for Motor Control on SOMANET over EtherCAT.

* [How to?](https://github.com/synapticon/sc_sncn_ctrlproto/tree/master/howto)

<table >
<tr>
  <td width="150px" height="30px">Latest release: </td>
  <td width="300px"><a href="https://github.com/synapticon/sc_sncn_ctrlproto/releases/tag/v1.0">1.0</a></td>
</tr>
<tr>
  <td height="30px">Maintainer:</td>
  <td><a href="mailto:support@synapticon.com">support@synapticon.com</a></td>
</tr>
</table> 


Key Features
---------
  * Cyclic Synchronous Position Control
  * Cyclic Synchronous Velocity Control
  * Cyclic Synchronous Torque Control
  * Profile Position Control
  * Profile Velocity Control
  * Profile Torque Control
  * Homing mode 
  * Precise position control based on position sensor ticks
  * Support motor and controller parameter update
  * Support complex node topology
  * Profile Generation for multinode configurations

Content
---------
| Module        				| Demo          						|
| :-------------: 				|:-------------							|
| [module_adc][module_adc]      		| [test_adc_external_input][test_adc_external_input] 		|
| [module_blocks][module_blocks] 		|       							|
| [module_comm][module_comm]	 		|     								|
| [module_common][module_common]		|     								|
| [module_commutation][module_commutation]	|								|
| [module_ctrl_loops][module_ctrl_loops]	| [test_position-ctrl][test_position-ctrl] [test_velocity-ctrl][test_velocity-ctrl] [test_torque-ctrl][test_torque-ctrl]	|
| [module_ecat_drive][module_ecat_drive]	| [test_ethercat-motorctrl-mode][test_ethercat-motorctrl-mode]	|
| [module_gpio][module_gpio]			| [test_gpio_digital][test_gpio_digital] [test_homing][test_homing] 	|
| [module_hall][module_hall]			| [test_hall][test_hall]					|
| [module_profile][module_profile]		|								|
| [module_qei][module_qei]			| [test_qei][test_qei]						|
| [module_sm][module_sm]			|								|


Required software (dependencies)
---------
  * [sc_somanet-base](https://github.com/synapticon/sc_somanet-base) 
  * [sc_pwm](https://github.com/synapticon/sc_pwm)
  * [sc_sncn_ethercat](https://github.com/synapticon/sc_sncn_ethercat) 
  * [sc_sncn_motorctrl_sin](https://github.com/synapticon/sc_sncn_motorctrl_sin)

Changelog
---------
  * [1.0](https://github.com/synapticon/sc_sncn_ctrlproto/releases/tag/v1.0) (2014-04-17)
	* Homing Mode
	* Profile Generation for multinode configurations
	* Precise position control based on position sensor ticks
  * [0.9beta](https://github.com/synapticon/sc_sncn_ctrlproto/releases/tag/v0.9-beta) (2013-01-24)

License
---------
Please see [LICENSE](https://github.com/synapticon/sc_sncn_ctrlproto/blob/master/LICENSE.md).
