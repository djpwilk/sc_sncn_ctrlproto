/**
 * canod.xc - managing object dictionary
 *
 * Author Frank Jeschke <jeschke@fjes.de>
 *
 * Copyright 2013 Synapticon GmbH
 */

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

#include "canod.xc"
#include <xc1.h>

/* static object dictionary */

/* object descriptions */
static struct _sdoinfo_object_description SDO_Info_Objects[] =  {
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
	{ 0, 0, 0, 0, nil}
};


/* static list of od entries description and value */
struct _sdoinfo_entry_description SDO_Info_Entries[] = {
	{ 0x1000, 0, 0, DEFTYPE_UNSIGNED32, 32, 0x0203, 0x00000000 },
	/* identity object */
	{ 0x1018, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 4 },
	{ 0x1018, 1, 0, DEFTYPE_UNSIGNED32, 32, 0x0203, 0xbeef }, /* Vendor ID (by ETG) */
	{ 0x1018, 2, 0, DEFTYPE_UNSIGNED32, 32, 0x0203, 0x1 }, /* Product Code */
	{ 0x1018, 3, 0, DEFTYPE_UNSIGNED32, 32, 0x0203, 0x2 }, /* Revision Number */
	{ 0x1018, 4, 0, DEFTYPE_UNSIGNED32, 32, 0x0203, 0 }, /* Serial Number */
	/* FIXME special index 0xff: { 0x1018, 0xff, 0, DEFTYPE_UNSIGNED32, ..., ..., ...} */
	/* FIXME check PDO Mapping RX and TX */
	{ 0x1600, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 1 },
	{ 0x1600, 1, 0, DEFTYPE_UNSIGNED32, 32, 0x0203, 0x20000116 }, /* see comment on PDO Mapping value below */
	{ 0x1601, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 1 },
	{ 0x1601, 1, 0, DEFTYPE_UNSIGNED32, 32, 0x0203, 0x20000216 }, /* see comment on PDO Mapping value below */
	{ 0x1A00, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 1 },
	{ 0x1A00, 1, 0, DEFTYPE_UNSIGNED32, 32, 0x0203, 0x20010116 }, /* see comment on PDO Mapping value below */
	{ 0x1A01, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 1 },
	{ 0x1A01, 1, 0, DEFTYPE_UNSIGNED32, 32, 0x0203, 0x20000216 }, /* see comment on PDO Mapping value below */
	/* SyncManager Communication Type - FIXME check assignment */
	{ 0x1C00, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 4 },
	{ 0x1C00, 1, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 0x01 }, /* mailbox receive */
	{ 0x1C00, 2, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 0x02 }, /* mailbox send */
	{ 0x1C00, 3, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 0x03 oder 0x04 }, /* PDO in or output */
	{ 0x1C00, 3, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 0x03 oder 0x04 }, /* PDO in or output */
	/* Tx PDO and Rx PDO assignments */
	{ 0x1C10, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 0}, /* assignment of SyncMan 0 */
	{ 0x1C11, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 0}, /* assignment of SyncMan 1 */
	{ 0x1C12, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 2}, /* assignment of SyncMan 2 */
	{ 0x1C12, 1, 0, DEFTUPE_UNSIGNED16, 16, 0x0203, 0x1600 /* either 0x1600 or 0x1A00 */ },
	{ 0x1C12, 2, 0, DEFTUPE_UNSIGNED16, 16, 0x0203, 0x1601 /* either 0x1601 or 0x1A01 */ ..},
	{ 0x1C13, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 2}, /* assignment of SyncMan 3 */
	{ 0x1C13, 1, 0, DEFTUPE_UNSIGNED16, 16, 0x0203, 0x1A00 /* either 0x1600 or 0x1A00 */ },
	{ 0x1C13, 2, 0, DEFTUPE_UNSIGNED16, 16, 0x0203, 0x1A01 /* either 0x1601 or 0x1A01 */ ..},
	/* FIXME check - objects describing RxPDOs */
	{ 0x2000, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 2 },
	{ 0x2000, 1, 0, DEFTYPE_UNSIGNED16, 16, 0x0203, 0 }, /* the values are elsewhere !!! */
	{ 0x2000, 2, 0, DEFTYPE_UNSIGNED16, 16, 0x0203, 0 }, /* the values are elsewhere !!! */
	/* FIXME check - objects describing RxPDOs */
	{ 0x2001, 0, 0, DEFTYPE_UNSIGNED8, 8, 0x0203, 2 },
	{ 0x2001, 1, 0, DEFTYPE_UNSIGNED16, 16, 0x0203, 0 }, /* the values are elsewhere !!! */
	{ 0x2001, 2, 0, DEFTYPE_UNSIGNED16, 16, 0x0203, 0 }, /* the values are elsewhere !!! */
	{ 0, 0, 0, 0, 0, 0 }
};



/* API implementation */

int canod_get_list_length(unsigned length[])
{
	return 0;
}

int canod_get_list(unsigned list[], unsigned cathegory)
{
	return 0;
}
int canod_get_entry_description(struct _sdoinfo_service_data obj, unsigned index, unsigned subindex)
{
	return 0;
}

int canod_get_entry(unsigned index, unsigned subindex, char values[])
{
	/* index 0x0001 - 0x0FFF  data type area */
	/* index 0x1000 - 0x1FFF  CoE communication profile area */
	/* index 0x2000 - 0x5FFF  manufacturer specific area - UNUSED */
	/* index 0x6000 - 0xFFFF  profile area - UNUSED */

	if (index > 0x1000 && index < 0x1ffff) {
		switch (index) {
		case 0x1000:
			!!! Device type;
			break;

		case 0x1018:
			switch (subindex) {
			case 0:
				identity.objcount;
				break;

int canod_get_entry(unsigned index, unsigned subindex, unsigned &value, unsigned &bitlength)
{
	int i;

	/* FIXME handle special subindex 0xff to request object type -> see also CiA 301 */

	for (int i=0; SDO_Info_Entries[i].index != 0x0; i++) {
		if (SDO_Info_Entries[i].index == index
		    && SDO_Info_Entries[i].subindex == subindex) {
			value = SDO_Info_Entries[i].value;
			bitlength = SDO_Info_Entries[i].bitLength; /* alternative bitLength */

			return 0;
		}
	}

	return 1; /* not found */
}

int canod_set_entry(unsigned index, unsigned subindex, char values[])
{
	return -1; /* currently unsupported! */
}

