Control Protocol SOFTWARE COMPONENT
...................................
.. image:: https://s3-eu-west-1.amazonaws.com/synapticon-resources/images/logos/synapticon_fullname_blackoverwhite_280x48.png

Implementation of Communication protocol layer for Motor Control on SOMANET over EtherCAT.

: 0.9beta
:Maintainer: support@synapticon.com

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

Further information
=======

   sc_sncn_motorctrl_sin `DOCUMENTATION`_.

Known Issues
============

   None

Required software (dependencies)
================================

  * `sc_somanet-base`_ 
  * `sc_pwm`_
  * `sc_sncn_ethercat`_ 
  * `sc_sncn_motorctrl_sin`_

Changelog
=======

  * `0.9beta`_ (2013-01-24)

License
=======

Please see `LICENSE`_.

.. _sc_somanet-base: https://github.com/synapticon/sc_somanet-base
.. _sc_pwm: https://github.com/synapticon/sc_pwm
.. _sc_sncn_ethercat: https://github.com/synapticon/sc_sncn_ethercat
.. _sc_sncn_motorctrl_sin: https://github.com/synapticon/sc_sncn_motorctrl_sin

.. _DOCUMENTATION: http://synapticon.github.io/sc_sncn_ctrlproto/
.. _0.9beta: None
.. _LICENSE: https://github.com/synapticon/sc_sncn_ctrlproto/blob/master/LICENSE.dox
