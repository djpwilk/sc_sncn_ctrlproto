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
| [lib_linux_ctrlproto][lib_linux_ctrlproto]    |								|
| [lib_linux_motor_drive][lib_linux_motor_drive]|       							|
| [module_ctrlproto][module_ctrlproto]		| [app_ctrlproto-test][app_ctrlproto-test]  			| |						| [app_linux_ctrlproto-csp-example][app_linux_ctrlproto-csp-example]	|
|						| [app_linux_ctrlproto-cst-example][app_linux_ctrlproto-cst-example]   	|			
|						| [app_linux_ctrlproto-csv-example][app_linux_ctrlproto-csv-example]	|
|						| [app_linux_ctrlproto-hm-example][app_linux_ctrlproto-hm-example]	|
| 						| [app_linux_ctrlproto-ppm-example][app_linux_ctrlproto-ppm-example]	|
| 						| [app_linux_ctrlproto-ptm-example][app_linux_ctrlproto-ptm-example]  	|
| 						| [app_linux_ctrlproto-master-example][app_linux_ctrlproto-master-example]	|



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

[module_ctrlproto]: http
[lib_linux_motor_drive]: http
[lib_linux_ctrlproto]: http
[app_ctrlproto-test]: http
[app_linux_ctrlproto-csp-example]: http
[app_linux_ctrlproto-cst-example]: http	
[app_linux_ctrlproto-csv-example]: http
[app_linux_ctrlproto-hm-example]: http
[app_linux_ctrlproto-ppm-example]: http
[app_linux_ctrlproto-ptm-example]: http
[app_linux_ctrlproto-master-example]: http	

