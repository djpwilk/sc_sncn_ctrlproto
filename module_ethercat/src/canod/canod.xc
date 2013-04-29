/**
 * canod.xc - managing object dictionary
 *
 * Author Frank Jeschke <jeschke@fjes.de>
 *
 * Copyright 2013 Synapticon GmbH
 */

#include "canod.h"
#include "canod_datatypes.h"
#include <xs1.h>

/* static object dictionary */

/*
 * supported object dictionary indexes:
 * - 0x0000 .. 0x0037                        - default data types
 * - 0x1000, 0x1018,                         - device identity
 * - 0x1600, 0x1601,                         - PDO Mapping rx
 * - 0x1A00, 0x1A01,                         - PDO Mapping tx
 * - 0x1C00, 0x1C10, 0x1C11, 0x1C12, 0x1C13, - SyncManager settings
 * - 0x1C30, 0x1C31, 0x1C32, 0x1C33          - SyncManager synchronisation
 *
 * A future version of this module will support device profiles like EDS or similar.
 */

/* object descriptions */
static struct _sdoinfo_object_description SDO_Info_Objects[14] =  {
	{ 0x1000, DEFTYPE_UNSIGNED32, 0, CANOD_TYPE_VAR , "Device Type" },
	{ 0x1018, DEFSTRUCT_IDENTITY, 4, CANOD_TYPE_RECORD, "Identity" },
	{ 0x1C00, DEFTYPE_UNSIGNED8,  4, CANOD_TYPE_ARRAY, "Sync Manager Communication Type" },
#if CIA402
	{ 0x1600, DEFSTRUCT_PDO_MAPPING, 5, CANOD_TYPE_RECORD, "Rx PDO Mapping" },
	{ 0x1A00, DEFSTRUCT_PDO_MAPPING, 5, CANOD_TYPE_RECORD, "Tx PDO Mapping" },
#else
	{ 0x1600, DEFSTRUCT_PDO_MAPPING, 2, CANOD_TYPE_RECORD, "Rx PDO Mapping" },
	{ 0x1A00, DEFSTRUCT_PDO_MAPPING, 2, CANOD_TYPE_RECORD, "Tx PDO Mapping" },
#endif
	/* FIXME add 0x1C1x Syncmanager x PDO Assignment */
	{ 0x1C10, DEFTYPE_UNSIGNED16, 0, CANOD_TYPE_ARRAY, "SM0 PDO Assing" },
	{ 0x1C11, DEFTYPE_UNSIGNED16, 0, CANOD_TYPE_ARRAY, "SM1 PDO Assing" },
	{ 0x1C12, DEFTYPE_UNSIGNED16, 1, CANOD_TYPE_ARRAY, "SM2 PDO Assing" },
	{ 0x1C13, DEFTYPE_UNSIGNED16, 1, CANOD_TYPE_ARRAY, "SM3 PDO Assing" },
	/* assigned PDO objects */
#ifndef CIA402
	{ 0x6000, DEFTYPE_UNSIGNED16, 2, CANOD_TYPE_ARRAY, "Rx PDO Assingnment" },
	{ 0x7000, DEFTYPE_UNSIGNED16, 2, CANOD_TYPE_ARRAY, "Tx PDO Assingnment" },
#endif
#ifdef CIA402
	{ CIA402_CONTROLWORD, DEFTYPE_UNSIGNED16, 0, CANOD_TYPE_VAR, "Controlword" },
	{ CIA402_STATUSWORD, DEFTYPE_UNSIGNED16, 0, CANOD_TYPE_VAR, "Statusword" },
	{ CIA402_OP_MODES, DEFTYPE_INTEGER8, 0, CANOD_TYPE_VAR, "Modes of Operation" }, /* ??? correct type? */
	{ CIA402_OP_MODES_DISP, DEFTYPE_INTEGER8, 0, CANOD_TYPE_VAR, "Modes of Operation Display" }, /* ??? correct type? */
	{ CIA402_POSITION_VALUE, DEFTYPE_INTEGER32, 0, CANOD_TYPE_VAR, "Position Value"},
	{ CIA402_FOLLOWING_ERROR_WINDOW, DEFTYPE_UNSIGNED32, 0, CANOD_TYPE_VAR, "Following Error Window"},
	{ CIA402_FOLLOWING_ERROR_TIMEOUT, DEFTYPE_UNSIGNED16, 0, CANOD_TYPE_VAR, "Following Error Timeout"},
	{ CIA402_VELOCITY_VALUE, DEFTYPE_INTEGER32, 0, CANOD_TYPE_VAR, "Position Value"},
	{ CIA402_TARGET_TORQUE, DEFTYPE_INTEGER16, 0, CANOD_TYPE_VAR, "Target Torque"},
	{ CIA402_TORQUE_VALUE, DEFTYPE_INTEGER16, 0, CANOD_TYPE_VAR, "Torque actual Value"},
	{ CIA402_TARGET_POSITION, DEFTYPE_INTEGER32, 0, CANOD_TYPE_VAR, "Target Position" },
	{ CIA402_POSITION_RANGELIMIT, DEFTYPE_INTEGER32, 2, CANOD_TYPE_ARRAY, "Postition Range Limits"},
	{ CIA402_SOFTWARE_POSITION_LIMIT, DEFTYPE_INTEGER32, 2, CANOD_TYPE_ARRAY, "Software Postition Range Limits"},
	{ CIA402_VELOCITY_OFFSET, DEFTYPE_INTEGER32, 0, CANOD_TYPE_VAR, "Velocity Offset" },
	{ CIA402_TORQUE_OFFSET, DEFTYPE_INTEGER32, 0, CANOD_TYPE_VAR, "Torque Offset" },
	{ CIA402_INTERPOL_TIME_PERIOD, 0x80/*???*/, 2, CANOD_TYPE_RECORD, "Interpolation Time Period"},
	/* { CIA402_FOLLOWING_ERROR, , , , }, -- no object description available but recommendet in csp, csv, cst mode*/
	{ CIA402_TARGET_VELOCITY, DEFTYPE_INTEGER32, 0, CANOD_TYPE_VAR, "Target Velocity" },
	{ CIA402_SUPPORTED_DRIVE_MODES, DEFTYPE_UNSIGNED32, 0, CANOD_TYPE_VAR, "Supported drive modes" },
#endif
	{ 0, 0, 0, 0, {0}}
};

#define PDOMAPING(idx,sub,bit)    ( (uint32_t)(idx<<16)|(sub<<8)|bit )

/* static list of od entries description and value */
struct _sdoinfo_entry_description SDO_Info_Entries[] = {
#ifndef CIA402
	{ 0x1000, 0, 0, DEFTYPE_UNSIGNED32, 32, 0x0203, 0x00000001, "Device Type" },
#else
	/* device type value: Mode bits (8bits) | type (8bits) | device profile number (16bits)
	                      *                 | 0x02 (Servo) | 0x0192 */
	{ 0x1000, 0, 0, DEFTYPE_UNSIGNED32, 32, 0x0203, 0x00020192, "Device Type" },
#endif
	/* identity object */
	{ 0x1018, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0207, 4, "Identity" },
	{ 0x1018, 1, 0, DEFTYPE_UNSIGNED32, 32, 0x0207, 0x000022d2, "Vendor ID" }, /* Vendor ID (by ETG) */
	{ 0x1018, 2, 0, DEFTYPE_UNSIGNED32, 32, 0x0207, 0x00000201, "Product Code" }, /* Product Code */
	{ 0x1018, 3, 0, DEFTYPE_UNSIGNED32, 32, 0x0207, 0x0a000002, "Revision Number" }, /* Revision Number */
	{ 0x1018, 4, 0, DEFTYPE_UNSIGNED32, 32, 0x0207, 0x000000dd, "Serial Number" }, /* Serial Number */
	/* FIXME special index 0xff: { 0x1018, 0xff, 0, DEFTYPE_UNSIGNED32, ..., ..., ...} */
#ifdef CIA402
	/* RxPDO Mapping */
	{ 0x1600, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0207, 5, "Rx PDO Mapping" }, /* input */
	{ 0x1600, 1, 0, DEFTYPE_UNSIGNED8, 8, 0x0207, PDOMAPING(CIA402_CONTROLWORD,0,8), "Rx PDO Mapping Controlword" },
	{ 0x1600, 2, 0, DEFTYPE_UNSIGNED8, 8, 0x0207, PDOMAPING(CIA402_OP_MODES,0,8), "Rx PDO Mapping Opmode" },
	{ 0x1600, 3, 0, DEFTYPE_UNSIGNED16, 16, 0x0207, PDOMAPING(CIA402_TARGET_TORQUE,0,16), "Rx PDO Mapping Target Torque" },
	{ 0x1600, 4, 0, DEFTYPE_UNSIGNED32, 32, 0x0207, PDOMAPING(CIA402_TARGET_POSITION,0,32), "Rx PDO Mapping Target Position" },
	{ 0x1600, 5, 0, DEFTYPE_UNSIGNED32, 32, 0x0207, PDOMAPING(CIA402_TARGET_VELOCITY,0,32), "Rx PDO Mapping Target Velocity" },
	/* TxPDO Mapping */
	{ 0x1A00, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0207, 5, "Tx PDO Mapping" }, /* output */
	{ 0x1A00, 1, 0, DEFTYPE_UNSIGNED8, 8, 0x0207, PDOMAPING(CIA402_STATUSWORD,0,8), "Tx PDO Mapping Statusword" },
	{ 0x1A00, 2, 0, DEFTYPE_UNSIGNED8, 8, 0x0207, PDOMAPING(CIA402_OP_MODES_DISP,0,8), "Tx PDO Mapping Modes Display" },
	{ 0x1A00, 3, 0, DEFTYPE_UNSIGNED32, 32, 0x0207, PDOMAPING(CIA402_POSITION_VALUE,0,32), "Tx PDO Mapping Position Value" },
	{ 0x1A00, 4, 0, DEFTYPE_UNSIGNED32, 32, 0x0207, PDOMAPING(CIA402_VELOCITY_VALUE,0,32), "Tx PDO Mapping Velocity Value" },
	{ 0x1A00, 5, 0, DEFTYPE_UNSIGNED16, 16, 0x0207, PDOMAPING(CIA402_TORQUE_VALUE,0,16), "Tx PDO Mapping Torque Value" },
#else
	/* RxPDO Mapping */
	{ 0x1600, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0207, 2, "Rx PDO Mapping" }, /* input */
	{ 0x1600, 1, 0, DEFTYPE_UNSIGNED32, 32, 0x0207, 0x60000120, "Rx PDO Mapping" }, /* see comment on PDO Mapping value below */
	{ 0x1600, 2, 0, DEFTYPE_UNSIGNED32, 32, 0x0207, 0x60000220, "Rx PDO Mapping" }, /* see comment on PDO Mapping value below */
	/* TxPDO Mapping */
	{ 0x1A00, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0207, 2, "Tx PDO Mapping" }, /* output */
	{ 0x1A00, 1, 0, DEFTYPE_UNSIGNED32, 32, 0x0207, 0x70000120, "Tx PDO Mapping" }, /* see comment on PDO Mapping value below */
	{ 0x1A00, 2, 0, DEFTYPE_UNSIGNED32, 32, 0x0207, 0x70000220, "Tx PDO Mapping" }, /* see comment on PDO Mapping value below */
#endif
	/* SyncManager Communication Type */
	{ 0x1C00, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0207, 4, "SyncManager Comm" },
	{ 0x1C00, 1, 0, DEFTYPE_UNSIGNED8, 8, 0x0207, 0x01, "SyncManager Comm" }, /* mailbox receive */
	{ 0x1C00, 2, 0, DEFTYPE_UNSIGNED8, 8, 0x0207, 0x02, "SyncManager Comm" }, /* mailbox send */
	{ 0x1C00, 3, 0, DEFTYPE_UNSIGNED8, 8, 0x0207, 0x03, "SyncManager Comm" }, /* PDO in (bufferd mode) */
	{ 0x1C00, 4, 0, DEFTYPE_UNSIGNED8, 8, 0x0207, 0x04, "SyncManager Comm" }, /* PDO output (bufferd mode) */
	/* Tx PDO and Rx PDO assignments */
	{ 0x1C10, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0207, 0, "SyncMan 0 assignment"}, /* assignment of SyncMan 0 */
	{ 0x1C11, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0207, 0, "SyncMan 1 assignment"}, /* assignment of SyncMan 1 */
	{ 0x1C12, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0207, 1, "SyncMan 2 assignment"}, /* assignment of SyncMan 2 */
	{ 0x1C12, 1, 0, DEFTYPE_UNSIGNED16, 16, 0x0207, 0x1600, "SyncMan 2 assignment" },
	{ 0x1C13, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0207, 1, "SyncMan 3 assignment"}, /* assignment of SyncMan 3 */
	{ 0x1C13, 1, 0, DEFTYPE_UNSIGNED16, 16, 0x0207, 0x1A00, "SyncMan 3 assignment" },
#ifndef CIA402
	/* objects describing RxPDOs */
	{ 0x6000, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0207, 2, "Rx PDOs" },
	{ 0x6000, 1, 0, DEFTYPE_UNSIGNED16, 16, 0x0247, 0x0001, "Rx PDOs" }, /* the values are stored in application */
	{ 0x6000, 2, 0, DEFTYPE_UNSIGNED16, 16, 0x0247, 0x0002, "Rx PDOs" }, /* the values are stored in application */
	/* objects describing TxPDOs */
	{ 0x7000, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0207, 2, "Tx PDOs" },
	{ 0x7000, 1, 0, DEFTYPE_UNSIGNED16, 16, 0x0287, 0x0010, "Tx PDOs" }, /* the values are stored in application */
	{ 0x7000, 2, 0, DEFTYPE_UNSIGNED16, 16, 0x0287, 0x0020, "Tx PDOs" }, /* the values are stored in application */
#endif
	/* CiA objects */
	/* index, sub, value info, datatype, bitlength, object access, value, name */
	{ CIA402_CONTROLWORD, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0207 |COD_RXPDO_MAPABLE|COD_WR_OP_STATE, 0, "CiA402 Control Word" }, /* map to PDO */
	{ CIA402_STATUSWORD, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0207|COD_TXPDO_MAPABLE, 0, "CiA402 Status Word" },  /* map to PDO */
	{ CIA402_SUPPORTED_DRIVE_MODES, 0, 0, DEFTYPE_UNSIGNED32, 32, 0x0207, 0x0280 /* csv, csp, cst */, "Supported drive modes" },
	{ CIA402_OP_MODES, 0, 0, DEFTYPE_INTEGER8, 8, 0x0207|COD_RXPDO_MAPABLE|COD_WR_OP_STATE/* writeable? */, CIA402_OP_MODE_CSP, "Operating mode" },
	{ CIA402_OP_MODES_DISP, 0, 0, DEFTYPE_INTEGER8, 8, 0x0207|COD_TXPDO_MAPABLE, CIA402_OP_MODE_CSP, "Operating mode" },
	{ CIA402_POSITION_VALUE, 0, 0,  DEFTYPE_INTEGER32, 32, 0x0207|COD_TXPDO_MAPABLE, 0, "Position Value" }, /* FIXME PDO */
	{ CIA402_FOLLOWING_ERROR_WINDOW, 0, 0, DEFTYPE_UNSIGNED32, 32, 0x0207, 0, "Following Error Window"},
	{ CIA402_FOLLOWING_ERROR_TIMEOUT, 0, 0, DEFTYPE_UNSIGNED16, 16, 0x207, 0, "Following Error Timeout"},
	{ CIA402_VELOCITY_VALUE, 0, 0, DEFTYPE_INTEGER32, 32, 0x0207|COD_TXPDO_MAPABLE, 0, "Velocity Value"},
	{ CIA402_TARGET_TORQUE, 0, 0, DEFTYPE_INTEGER16, 16, 0x0207|COD_RXPDO_MAPABLE|COD_WR_OP_STATE, 0, "Target Torque"},
	{ CIA402_TORQUE_VALUE, 0, 0, DEFTYPE_INTEGER16, 16, 0x0207|COD_TXPDO_MAPABLE, 0, "Torque actual Value"},
	{ CIA402_TARGET_POSITION, 0, 0, DEFTYPE_INTEGER32, 32, 0x0207|COD_RXPDO_MAPABLE|COD_WR_OP_STATE, 0, "Target Position" },
	{ CIA402_POSITION_RANGELIMIT, 0, 0, DEFTYPE_INTEGER32, 32, 0x0207, 2, "Postition Range Limits"},
	{ CIA402_POSITION_RANGELIMIT, 1, 0, DEFTYPE_INTEGER32, 32, 0x0207, 0, "Min Postition Range Limit"},
	{ CIA402_POSITION_RANGELIMIT, 2, 0, DEFTYPE_INTEGER32, 32, 0x0207, 0, "Max Postition Range Limit"},
	{ CIA402_SOFTWARE_POSITION_LIMIT, 0, 0,  DEFTYPE_INTEGER32, 32, 0x0207, 2, "Software Postition Range Limits"},
	{ CIA402_SOFTWARE_POSITION_LIMIT, 1, 0,  DEFTYPE_INTEGER32, 32, 0x0207, 0, "Min Software Postition Range Limit"},
	{ CIA402_SOFTWARE_POSITION_LIMIT, 2, 0,  DEFTYPE_INTEGER32, 32, 0x0207, 0, "Max Software Postition Range Limit"},
	{ CIA402_VELOCITY_OFFSET, 0, 0, DEFTYPE_INTEGER32, 32, 0x0207, 0, "Velocity Offset" },
	{ CIA402_TORQUE_OFFSET, 0, 0, DEFTYPE_INTEGER32, 32, 0x0207, 0, "Torque Offset" },
	{ CIA402_INTERPOL_TIME_PERIOD, 0, 0, DEFTYPE_INTEGER32, 32, 0x0207, 2, "Interpolation Time Period"},
	{ CIA402_INTERPOL_TIME_PERIOD, 1, 0, DEFTYPE_INTEGER32, 32, 0x0207, 1, "Interpolation Time Unit"}, /* value range: 1..255msec */
	{ CIA402_INTERPOL_TIME_PERIOD, 2, 0, DEFTYPE_INTEGER32, 32, 0x0207, -3, "Interpolation Time Index"}, /* value range: -3, -4 (check!)*/
	{ CIA402_TARGET_VELOCITY, 0, 0,  DEFTYPE_INTEGER32, 32, 0x0207|COD_RXPDO_MAPABLE|COD_WR_OP_STATE, 0, "Target Velocity" },
	{ 0, 0, 0, 0, 0, 0, 0, "\0" }
};

/* local */

static int get_minvalue(unsigned datatype)
{
	switch (datatype) {
	case DEFTYPE_BOOLEAN:
		return 0;
	case DEFTYPE_INTEGER8:
		return 0xff;
	case DEFTYPE_INTEGER16:
		return 0xffff;
	case DEFTYPE_INTEGER32:
		return 0xffffffff;
	case DEFTYPE_UNSIGNED8:
		return 0;
	case DEFTYPE_UNSIGNED16:
		return 0;
	case DEFTYPE_UNSIGNED32:
		return 0;
	default:
		return 0;
	}

	return 0;
}

static int get_maxvalue(unsigned datatype)
{
	switch (datatype) {
	case DEFTYPE_BOOLEAN:
		return 1;
	case DEFTYPE_INTEGER8:
		return 0x7f;
	case DEFTYPE_INTEGER16:
		return 0x7fff;
	case DEFTYPE_INTEGER32:
		return 0x7fffffff;
	case DEFTYPE_UNSIGNED8:
		return 0xff;
	case DEFTYPE_UNSIGNED16:
		return 0xffff;
	case DEFTYPE_UNSIGNED32:
		return 0xffffffff;
	default:
		return 0;
	}

	return 0;
}



/* API implementation */

int canod_get_all_list_length(unsigned length[])
{
	/* FIXME correct length of all subsections */
	length[0] = sizeof(SDO_Info_Objects)/sizeof(SDO_Info_Objects[0]);
	length[1] = 0;
	length[2] = 0;
	length[3] = 0;
	length[4] = 0;

	return 0;
}

/* FIXME except for all the list length returns length 0 */
int canod_get_list_length(unsigned listtype)
{
	int length = 0;

	switch (listtype) {
	case CANOD_LIST_ALL:
		length = sizeof(SDO_Info_Objects)/sizeof(SDO_Info_Objects[0]);
		break;

	case CANOD_LIST_RXPDO_MAP:
		break;

	case CANOD_LIST_TXPDO_MAP:
		break;

	case CANOD_LIST_REPLACE:
		break;

	case CANOD_LIST_STARTUP:
		break;
	
	default:
		return 0;
	};

	return length;
}

/* FIXME implement and check other list lengths. */
int canod_get_list(unsigned list[], unsigned size, unsigned listtype)
{
	int length, i;

	switch (listtype) {
	case CANOD_LIST_ALL:
		length = sizeof(SDO_Info_Objects)/sizeof(SDO_Info_Objects[0])-1;

		for (i=0; i<length && i<size; i++) {
			list[i] = SDO_Info_Objects[i].index;
		}

		break;

	case CANOD_LIST_RXPDO_MAP:
		break;

	case CANOD_LIST_TXPDO_MAP:
		break;

	case CANOD_LIST_REPLACE:
		break;

	case CANOD_LIST_STARTUP:
		break;
	
	default:
		return 0;
	};

	return length;
}

int canod_get_object_description(struct _sdoinfo_object_description &obj, unsigned index)
{
	int i = 0, k;

	for (i=0; i<sizeof(SDO_Info_Objects)/sizeof(SDO_Info_Objects[0]); i++) {
		if (SDO_Info_Objects[i].index == index) {
			obj.index = SDO_Info_Objects[i].index;
			obj.dataType = SDO_Info_Objects[i].dataType;
			obj.maxSubindex = SDO_Info_Objects[i].maxSubindex;
			obj.objectCode = SDO_Info_Objects[i].objectCode;
			for (k=0; k<50; k++) { /* FIXME set a define for max string length */
				obj.name[k] = SDO_Info_Objects[i].name[k];
			}
			break;
		}

		if (SDO_Info_Objects[i].index == 0x0) {
			return 1; /* object not found */
		}
	}

	return 0;
}

int canod_get_entry_description(unsigned index, unsigned subindex, unsigned valueinfo, struct _sdoinfo_entry_description &desc)
{
	struct _sdoinfo_entry_description entry;
	int i,k;

	for (i=0; i<SDO_Info_Entries[i].index != 0x0; i++) {
		if ((SDO_Info_Entries[i].index == index) && (SDO_Info_Entries[i].subindex == subindex))
			break;
	}

	if (SDO_Info_Entries[i].index == 0x0)
		return -1; /* Entry object not found */

	/* FIXME implement entry_description */
	desc.index = index;
	desc.subindex = subindex;
	desc.valueInfo = valueinfo;

	desc.dataType = SDO_Info_Entries[i].dataType;
	desc.bitLength = SDO_Info_Entries[i].bitLength;
	desc.objectAccess = SDO_Info_Entries[i].objectAccess;

#if 1
	desc.value = SDO_Info_Entries[i].value;
#else /* wrong assumption of packet content? */
	switch (valueinfo) {
	case CANOD_VALUEINFO_UNIT:
		desc.value = 0; /* unit type currently unsupported */
		break;

	case CANOD_VALUEINFO_DEFAULT:
		desc.value = SDO_Info_Entries[i].value;
		break;
	case CANOD_VALUEINFO_MIN:
		desc.value = get_minvalue(desc.dataType);
		break;

	case CANOD_VALUEINFO_MAX:
		desc.value = get_maxvalue(desc.dataType);
		break;
	default:
		/* empty response */
		desc.value = 0;
		break;
	}
#endif

	/* copy name */
	for (k=0; k<50 && SDO_Info_Entries[i].name[k] != '\0'; k++) {
		desc.name[k] = SDO_Info_Entries[i].name[k];
	}
	return 0;
}

int canod_get_entry(unsigned index, unsigned subindex, unsigned &value, unsigned &bitlength)
{
	int i;

	/* FIXME handle special subindex 0xff to request object type -> see also CiA 301 */

	for (i=0; SDO_Info_Entries[i].index != 0x0; i++) {
		if (SDO_Info_Entries[i].index == index
		    && SDO_Info_Entries[i].subindex == subindex) {
			value = SDO_Info_Entries[i].value;
			bitlength = SDO_Info_Entries[i].bitLength; /* alternative bitLength */

			return 0;
		}
	}

	return 1; /* not found */
}

int canod_set_entry(unsigned index, unsigned subindex, unsigned value, unsigned type)
{
	for (int i; SDO_Info_Entries[i].index != 0x0; i++) {
		if (SDO_Info_Entries[i].index == index
				&& SDO_Info_Entries[i].subindex == subindex) {
			SDO_Info_Entries[i].value = value;
			return 0;
		}
	}

	return 1; /* cannot set value */
}

