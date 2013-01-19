/**
 * canod_datatypes.h - definition of datatypes used in the object dictionary
 */

#ifndef CANOD_DATATYPES_H
#define CANOD_DATATYPES_H

/* Basic Data Type Area */
#define DEFTYPE_BOOLEAN          0x0001
#define DEFTYPE_INTEGER8         0x0002
#define DEFTYPE_INTEGER16        0x0003
#define DEFTYPE_INTEGER32        0x0004
#define DEFTYPE_UNSIGNED8        0x0005
#define DEFTYPE_UNSIGNED16       0x0006
#define DEFTYPE_UNSIGNED32       0x0007
#define DEFTYPE_REAL32           0x0008
#define DEFTYPE_VISIBLE_STRING   0x0009
#define DEFTYPE_OCTET_STRING     0x000A
#define DEFTYPE_UNICODE_STRING   0x000B
#define DEFTYPE_TIME_OF_DAY      0x000C
#define DEFTYPE_TIME_DIFFERENCE  0x000D

#define DEFTYPE_DOMAIN           0x000F

#define DEFSTRUCT_PDO_MAPPING    0x0021
#define DEFSTRUCT_IDENTITY       0x0023

#endif /* CANOD_DATATYPES_H */
