/**
 * canod.h
 *
 * Handle CAN object dictionary.
 */

/* Roadmap: this object dictionary should be more dynamically and more general */

#ifndef CANOD_H
#define CANOD_H

/* SDO Information operation code */
#define CANOD_OP_

/* list of dictionary lists identifiers */
#define CANOD_GET_NUMBER_OF_OBJECTS   0x00
#define CANOD_ALL_OBJECTS             0x01
#define CANOD_RXPDO_MAPABLE           0x02
#define CANOD_TXPDO_MAPABLE           0x03
#define CANOD_DEVICE_REPLACEMENT      0x04
#define CANOD_STARTUP_PARAMETER       0x05

/* possible object types of dictionary objects */
#define CANOD_TYPE_DOMAIN     0x0
#define CANOD_TYPE_DEFTYPE    0x5
#define CANOD_TYPE_DEFSTRUCT  0x6
#define CANOD_TYPE_VAR        0x7
#define CANOD_TYPE_ARRAY      0x8
#define CANOD_TYPE_RECORD     0x9

/* list types */
#define CANOD_LIST_ALL        0x01  ///< all objects
#define CANOD_LIST_RXPDO_MAP  0x02  ///< only objects which are mappable in a RxPDO
#define CANOD_LIST_TXPDO_MAP  0x03  ///< only objects which are mappable in a TxPDO
#define CANOD_LIST_REPLACE    0x04  ///< objects which has to stored for a device replacement ???
#define CANOD_LIST_STARTUP    0x05  ///< objects which can be used as startup parameter


struct _sdoinfo_service {
	unsigned opcode;                   ///< OD operation code
	unsigned incomplete;               ///< 0 - last fragment, 1 - more fragments follow
	unsigned fragments;                ///< number of fragments which follow
	unsigned char data[SDO_MAX_DATA];  ///< SDO data field
};


/* sdo information data structure - FIXME may move to canod.h */

/* FIXME: add objects which describe the mapped PDO data.
 * the best matching OD area would be at index 0x200-0x5fff (manufacturer specific profile area
 */

/** object description structure */
struct _sdoinfo_object_description {
	unsigned index; ///< 16 bit int should be sufficient
	unsigned dataType; ///< 16 bit int should be sufficient
	unsigned char maxSubindex;
	unsigned char objectCode;
	unsigned char name[COE_MAX_MSG_SIZE-12];
};

/** entry description structure */
struct _sdoinfo_entry_description {
	unsigned index; ///< 16 bit int should be sufficient
	unsigned subindex; ///< 16 bit int should be sufficient
	unsigned char valueInfo; /* depends on SDO Info: get entry description request */
	unsigned char dataType;
	unsigned char bitLength;
	unsigned objectAccess;
	unsigned value; ///< real data type is defined by .dataType
};

/* ad valueInfo (BYTE):
 * Bit 0 - 2: reserved
 * Bit 3: unit type
 * Bit 4: default value
 * Bit 5: minimum value
 * Bit 6: maximum value
 */

/* ad objectAccess (WORD):
 * Bit 0: read access in Pre-Operational state
 * Bit 1: read access in Safe-Operational state
 * Bit 2: read access in Operational state
 * Bit 3: write access in Pre-Operational state
 * Bit 4: write access in Safe-Operational state
 * Bit 5: write access in Operational state
 * Bit 6: object is mappable in a RxPDO
 * Bit 7: object is mappable in a TxPDO
 * Bit 8: object can be used for backup
 * Bit 9: object can be used for settings
 * Bit 10 - 15: reserved
 */

/* ad PDO Mapping value (at index 0x200[01]):
 * bit 0-7: length of the mapped objects in bits
 * bit 8-15: subindex of the mapped object
 * bit 16-32: index of the mapped object
 */


/**
 * Return the length of all five cathegories
 */
int canod_get_all_list_length(unsigned length[]);

/**
 * return the length of list of type listtype
 */
int canod_get_list_length(unsigned listtype);

/**
 * Get list of objects in the specified cathegory
 */
int canod_get_list(unsigned list[], unsigned size; unsigned listtype);

/**
 * Get description of object at index and subindex.
 */
int canod_get_object_description(struct _sdoinfo_object_description &obj, unsigned index);

/**
 * Get/Set OD entry values
 *
 * @param index
 * @param subindex
 * @param &value     read/write the values from this array.
 * @param &type      the type of &value
 * @return 0 on success
 */
int canod_get_entry(unsigned index, unsigned subindex, unsigned &value, unsigned &type);

/**
 * Get/Set OD entry values
 *
 * @note This function is currently unused.
 *
 * @param index
 * @param subindex
 * @param &value     read/write the values from this array.
 * @param &type      the type of &value
 * @return 0 on success
 */
int canod_set_entry(unsigned index, unsigned subindex, unsigned value, unsigned type);

#endif /* CANOD_H */

