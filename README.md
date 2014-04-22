Control Protocol SOFTWARE COMPONENT
===============
<img align="left" src="https://s3-eu-west-1.amazonaws.com/synapticon-resources/images/logos/synapticon_fullname_blackoverwhite_280x48.png"/>
<br/>
<br/>

Implementation of Communication protocol layer for Motor Control on SOMANET over EtherCAT.

  * See the [MANUAL](http://synapticon.github.io/sc_sncn_ctrlproto/).

<table >
<tr>
  <td width="150px" height="30px">Latest release: </td>
  <td width="300px"><a href="https://github.com/synapticon/sc_sncn_ctrlproto/releases/tag/v0.9-beta">0.9beta</a></td>
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

Known Issues
---------
  * None

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

Please see [LICENSE](http://synapticon.github.io/sc_sncn_ctrlproto/legal.html).
