
#ifndef ALSTATE_H
#define ALSTATE_H

/* AL State and control register */
#define AL_REG_CONTROL                 0x0120
#define AL_REG_STATUS                  0x0130
#define AL_REG_STATUS_CODE             0x0134
#define AL_REG_EVENT_REQUEST_LOW       0x0220  /* AL Control Event */
#define AL_REG_EVENT_REQUEST_HIGH      0x0222  /* SyncM Events: bits 7:0; bits 15:8 unsued */

/* AL States */
#define AL_STATE_NOOP                  0x0000
#define AL_STATE_INIT                  0x0001
#define AL_STATE_PREOP                 0x0002
#define AL_STATE_BOOTSTRAP             0x0003
#define AL_STATE_SAFEOP                0x0004
#define AL_STATE_OP                    0x0008

#define AL_STATE_NOERROR               0x0000
#define AL_STATE_ERRORBIT              0x0010

#define AL_CONTROL_EVENT               0x0001

/* AL States Error Codes */
#define AL_NO_ERROR                    0x0000    /* any -> current state */
#define AL_ERROR                       0x0001    /* any -> current state + E */
#define AL_NO_MEMORY                   0x0002    /* any -> current state + E */
#define AL_INVALID_REQUEST             0x0011
#define AL_UNKNOWN_REQUEST             0x0012
#define AL_BOOTSTRAP_NOT_SUPPORTED     0x0013
#define AL_NO_VALID_FIRMWARE           0x0014
#define AL_INVALID_MAILBOX_CONF_1      0x0015
#define AL_INVALID_MAILBOX_CONF_2      0x0016
#define AL_INVALID_SYNCM_CONF          0x0017
#define AL_INVALID_INPUTS              0x0018
#define AL_INVALID_OUTPUTS             0x0019
#define AL_SYNC_ERROR                  0x001A
#define AL_SYNCM_WATCHDOG              0x001B
#define AL_INVALID_SYNCM_TYPES         0x001C
#define AL_INVALID_OUTPUT_CONF         0x001D
#define AL_INVALID_INPUT_CONF          0x001E
#define AL_INVALID_WATCHDOG_CONF       0x001F
#define AL_SLAVE_NEEDS_COLD_START      0x0020
#define AL_SLAVE_NEEDS_INIT            0x0021
#define AL_SLAVE_NEEDS_PREOP           0x0022
#define AL_SLAVE_NEEDS_SAFEOP          0x0023
#define AL_INVALID_INPUT_MAPPING       0x0024
#define AL_INVALID_OUTPUT_MAPPING      0x0025
#define AL_INCONSISTENT_SETTINGS       0x0026
#define AL_FREERUN_NOT_SUPPORTED       0x0027
#define AL_SYNC_NOT_SUPPORTED          0x0028
#define AL_FREERUN_NEEDS_BUFFER_MODE   0x0029
#define AL_BACKGROUND_WATCHDOG         0x002A
#define AL_NO_VALID_INPUT_OUTPUT       0x002B
#define AL_FATAL_SYNC_ERROR            0x002C
#define AL_NOSYNC_ERROR                0x002D
#define AL_INVALID_DC_SYNCH_CONF       0x0030
#define AL_INVALID_DC_LATCH_CONF       0x0031
#define AL_PLL_ERROR                   0x0032
#define AL_INVALID_DC_IO_ERROR         0x0033
#define AL_INVALID_DC_TIMEOUT_ERROR    0x0034
#define AL_DC_INVALID_SYNC_CYCLE_TIME  0x0035
#define AL_DC_SYNC0_CYCLE_TIME         0x0036
#define AL_DC_SYNC1_CYCLE_TIME         0x0037
#define AL_MBX_AOE                     0x0041
#define AL_MBX_EOE                     0x0042
#define AL_MBX_COE                     0x0043
#define AL_MBX_FOE                     0x0044
#define AL_MBX_SOE                     0x0045
#define AL_MBX_VOE                     0x004F
#define AL_EEPROM_NO_ACCESS            0x0050
#define AL_EEPROM_ERROR                0x0051
#define AL_SLAVE_RESTARTED_LOCALLY     0x0060

/* Vendor specific error codes 0x8000 - 0xffff */

#endif /* ALSTATE_H */
