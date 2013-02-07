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
	/* FIXME add 0x160x record Rx PDO Mapping */
	{ 0x1600, DEFSTRUCT_PDO_MAPPING, 1, CANOD_TYPE_RECORD, "Rx PDO Mapping" },
	{ 0x1601, DEFSTRUCT_PDO_MAPPING, 1, CANOD_TYPE_RECORD, "Rx PDO Mapping" }, /* ??? */
	/* FIXME add 0x1A0x record Tx PDO Mapping */
	{ 0x1A00, DEFSTRUCT_PDO_MAPPING, 1, CANOD_TYPE_RECORD, "Tx PDO Mapping" },
	{ 0x1A01, DEFSTRUCT_PDO_MAPPING, 1, CANOD_TYPE_RECORD, "Tx PDO Mapping" }, /* ??? */
	/* FIXME add 0x1C1x Syncmanager x PDO Assignment */
	{ 0x1C10, DEFTYPE_UNSIGNED16, 0, CANOD_TYPE_ARRAY, "SM0 PDO Assing" },
	{ 0x1C11, DEFTYPE_UNSIGNED16, 0, CANOD_TYPE_ARRAY, "SM1 PDO Assing" },
	{ 0x1C12, DEFTYPE_UNSIGNED16, 2, CANOD_TYPE_ARRAY, "SM2 PDO Assing" },
	{ 0x1C13, DEFTYPE_UNSIGNED16, 2, CANOD_TYPE_ARRAY, "SM3 PDO Assing" },
	/* assigned PDO objects */
	{ 0x2000, DEFTYPE_UNSIGNED16, 2, CANOD_TYPE_ARRAY, "Rx PDO Assingnment" },
	{ 0x2001, DEFTYPE_UNSIGNED16, 2, CANOD_TYPE_ARRAY, "Tx PDO Assingnment" },
	{ 0, 0, 0, 0, {0}}
};


/* static list of od entries description and value */
struct _sdoinfo_entry_description SDO_Info_Entries[] = {
	{ 0x1000, 0, 0, DEFTYPE_UNSIGNED32, 32, 0x0203, 0x00000001, "Device Type" },
	/* identity object */
	{ 0x1018, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 4, "Identity" },
	{ 0x1018, 1, 0, DEFTYPE_UNSIGNED32, 32, 0x0203, 0x000022d2, "Vendor ID" }, /* Vendor ID (by ETG) */
	{ 0x1018, 2, 0, DEFTYPE_UNSIGNED32, 32, 0x0203, 0x00000201, "Product Code" }, /* Product Code */
	{ 0x1018, 3, 0, DEFTYPE_UNSIGNED32, 32, 0x0203, 0x0a000002, "Revision Number" }, /* Revision Number */
	{ 0x1018, 4, 0, DEFTYPE_UNSIGNED32, 32, 0x0203, 0xdeefdeef, "Serial Number" }, /* Serial Number */
	/* FIXME special index 0xff: { 0x1018, 0xff, 0, DEFTYPE_UNSIGNED32, ..., ..., ...} */
	/* FIXME check PDO Mapping RX and TX */
	{ 0x1600, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 1, "PDO Mapping" },
	{ 0x1600, 1, 0, DEFTYPE_UNSIGNED32, 32, 0x0203, 0x20000116, "PDO Mapping" }, /* see comment on PDO Mapping value below */
	{ 0x1601, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 1, "PDO Mapping" },
	{ 0x1601, 1, 0, DEFTYPE_UNSIGNED32, 32, 0x0203, 0x20000216, "PDO Mapping" }, /* see comment on PDO Mapping value below */
	{ 0x1A00, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 1, "PDO Mapping" },
	{ 0x1A00, 1, 0, DEFTYPE_UNSIGNED32, 32, 0x0203, 0x20010116, "PDO Mapping" }, /* see comment on PDO Mapping value below */
	{ 0x1A01, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 1, "PDO Mapping" },
	{ 0x1A01, 1, 0, DEFTYPE_UNSIGNED32, 32, 0x0203, 0x20000216, "PDO Mapping" }, /* see comment on PDO Mapping value below */
	/* SyncManager Communication Type - FIXME check assignment */
	{ 0x1C00, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 4, "SyncManager Comm" },
	{ 0x1C00, 1, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 0x01, "SyncManager Comm" }, /* mailbox receive */
	{ 0x1C00, 2, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 0x02, "SyncManager Comm" }, /* mailbox send */
	{ 0x1C00, 3, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 0x03, "SyncManager Comm" }, /* PDO in or output */
	{ 0x1C00, 3, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 0x04, "SyncManager Comm" }, /* PDO in or output */
	/* Tx PDO and Rx PDO assignments */
	{ 0x1C10, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 0, "SyncMan 0 assignment"}, /* assignment of SyncMan 0 */
	{ 0x1C11, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 0, "SyncMan 0 assignment"}, /* assignment of SyncMan 1 */
	{ 0x1C12, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 2, "SyncMan 0 assignment"}, /* assignment of SyncMan 2 */
	{ 0x1C12, 1, 0, DEFTYPE_UNSIGNED16, 16, 0x0203, 0x1600 /* either 0x1600 */, "SyncMan 0 assignment" },
	{ 0x1C12, 2, 0, DEFTYPE_UNSIGNED16, 16, 0x0203, 0x1601 /* either 0x1601 */, "SyncMan 0 assignment" },
	{ 0x1C13, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 2, "SyncMan 0 assignment"}, /* assignment of SyncMan 3 */
	{ 0x1C13, 1, 0, DEFTYPE_UNSIGNED16, 16, 0x0203, 0x1A00 /* either 0x1A00 */, "SyncMan 0 assignment" },
	{ 0x1C13, 2, 0, DEFTYPE_UNSIGNED16, 16, 0x0203, 0x1A01 /* either 0x1A01 */, "SyncMan 0 assignment" },
	/* FIXME check - objects describing RxPDOs */
	{ 0x2000, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 2, "Rx PDOs" },
	{ 0x2000, 1, 0, DEFTYPE_UNSIGNED16, 16, 0x0203, 0, "Rx PDOs" }, /* the values are elsewhere !!! */
	{ 0x2000, 2, 0, DEFTYPE_UNSIGNED16, 16, 0x0203, 0, "Rx PDOs" }, /* the values are elsewhere !!! */
	/* FIXME check - objects describing RxPDOs */
	{ 0x2001, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 2, "Tx PDOs" },
	{ 0x2001, 1, 0, DEFTYPE_UNSIGNED16, 16, 0x0203, 0, "Tx PDOs" }, /* the values are elsewhere !!! */
	{ 0x2001, 2, 0, DEFTYPE_UNSIGNED16, 16, 0x0203, 0, "Tx PDOs" }, /* the values are elsewhere !!! */
	{ 0, 0, 0, 0, 0, 0 }
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
	int i;

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
	return -1; /* currently unsupported! */
}

