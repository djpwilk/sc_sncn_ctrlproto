/* foe.h

   header for file file over EtherCAT access.

   2012-03-29 Frank Jeschke <jeschke@fjes.de>

   FIXME: in the moment only one file is supported. Future versions could
   provide a reserved are in RAM to hold a simple filesystem.
 */

/* packet types */

#define FOE_READ   0x01 /* FoE read*/
#define FOE_WRITE  0x02 /* FoE write */
#define FOE_DATA   0x03 /* data request */
#define FOE_ACK    0x04 /* ack request */
#define FOE_ERROR  0x05 /* error request */
#define FOE_BUSY   0x06 /* busy request */

/* error codes for use in FOE_ERROR packages */

#define FOE_ERR_UNDEF    0x8000 /* Not defined */
#define FOE_ERR_NOTFOUND 0x8001 /* not found */
#define FOE_ERR_NOACCESS 0x8002 /* Access denied */
#define FOE_ERR_DISKFULL 0x8003 /* disk full */
#define FOE_ERR_ILLEGAL  0x8004 /* illegal */
#define FOE_ERR_NUMBER   0x8005 /* paket number wrong */
#define FOE_ERR_EXISTS   0x8006 /* already exists */
#define FOE_ERR_NOUSER   0x8007 /* no user */
#define FOE_ERR_BOOT     0x8008 /* bootstrap only */
#define FOE_ERR_NOBOOT   0x8009 /* not bootstrap */
#define FOE_ERR_PERM     0x800A /* no rights */
#define FOE_ERR_PERR     0x800B /* programm error */

/* file structure */

#define BLKSZ      10000
#define MAX_FNAME  10

typedef struct {
	int fh;
	char name[MAX_FNAME];
	unsigned int size;
	unsigned int type;
	char bytes[BLKSZ];
	int currentpos;
	//int transmit_to_master???
} foefile_t;

typedef struct {
	unsigned char opcode;
	union {
		uint32_t packetnumber;
		uint32_t password;
		uint32_t error;
	} a;
	union {
		char filename[512];
		unsigned char data[512];
		char errortext[512];
	} b;
} foemsg_t;

/*
    application interface (public)
 */

#define SEEK_SET  0
#define SEEK_CUR  1

/**
 * @brief open file
 *
 * @param filename  filename to open
 * @return file handle, <0 on error
 */
int foe_open(char filename[]);

/**
 * @brief close file handle
 *
 * @param fh  file handle to close
 * @return 0 on success, <0 otherwise
 */
int foe_close(int fh);

/** 
 * @brief read from file handle
 *
 * @param fh  file handle to read from
 * @param size  number of byes to read
 * @param b[]   buffer to store read bytes
 * @return number of bytes read, <0 on error
 */
int foe_read(int fh, int size, char b[]);

/**
 * @brief write bytes to file starting at offset
 *
 * @param fh    filehandle to write to
 * @param size  number of bytes to write
 * @param b[]   buffer holding bytes to write
 * @return number of bytes written, <0 on error
 */
int foe_write(int fh, int size, char b[]);

/**
 * @brief set file position
 *
 * @param fh   filehandle to operate on
 * @param offset  new offset of filehandle
 * @param where   either SEEK_SET for absolute positioning or SEEK_CUR for relative positioning.
 * @param 0 on success
 */
int foe_seek(int fh, int offset, int whence);

/*
    intermodule interface (private)
 */

/**
 * @brief init pseudo filesystem
 * 
 * @return 0 on success
 */
int foe_init(void);

/**
 * @brief release pseude filesystem
 *
 * @return 0 on success
 */
int foe_release(void);

/**
 * @brief format file system
 *
 * this is basically a reinitalize.
 *
 * @return 0 on success
 */
int foe_format(void);

/*
 * Internal EtherCAT API
 */

// state machine, showing expected packages
// parse packet store to file - beware of lock

