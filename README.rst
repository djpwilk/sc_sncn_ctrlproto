SOFTWARE COMPONENT Control Protocol
...................................

:Latest release: 0.9beta
:Maintainer: support@synapticon.com
:Description: Implementation of Control Protocol for Motor Control on SOMANET over EtherCAT. 	


Key Features
============

   * Cyclic Synchronous Position Control
   * Cyclic Synchronous Velocity Control
   * Cyclic Synchronous Torque Control
   * Profile Position Control 
   * Profile Velocity Control
   * Profile Torque Control
   * Support motor and controller parameter update
   * Support complex node topology


Known Issues
============

   * Profile Torque Control not yet supported for IFM DC100 
   * Cyclic Synchronous Torque Control not yet supported for IFM DC100

Support
=======

Issues may be submitted via email at support@synapticon.com

Required software (dependencies)
================================

  * sc_somanet-base 
  * sc_pwm
  * sc_sncn_ethercat 
  * sc_sncn_motorctrl_sin