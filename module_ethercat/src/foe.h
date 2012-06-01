/* foe.h
 *
 * header for file file over EtherCAT access.
 *
 * 2012-03-29 Frank Jeschke <jeschke@fjes.de>
 */

#include <stdint.h>

/* packet types */

#define FOE_UNUSED         0x00   /* used to indicate empty FOE reply */
#define FOE_READ           0x01   /* FoE read*/
#define FOE_WRITE          0x02   /* FoE write */
#define FOE_DATA           0x03   /* data request */
#define FOE_ACK            0x04   /* ack request */
#define FOE_ERROR          0x05   /* error request */
#define FOE_BUSY           0x06   /* busy request */

/* error codes for use in FOE_ERROR packages */

#define FOE_ERR_UNDEF      0x8000 /* Not defined */
#define FOE_ERR_NOTFOUND   0x8001 /* not found */
#define FOE_ERR_NOACCESS   0x8002 /* Access denied */
#define FOE_ERR_DISKFULL   0x8003 /* disk full */
#define FOE_ERR_ILLEGAL    0x8004 /* illegal */
#define FOE_ERR_NUMBER     0x8005 /* paket number wrong */
#define FOE_ERR_EXISTS     0x8006 /* already exists */
#define FOE_ERR_NOUSER     0x8007 /* no user */
#define FOE_ERR_BOOT       0x8008 /* bootstrap only */
#define FOE_ERR_NOBOOT     0x8009 /* not bootstrap */
#define FOE_ERR_PERM       0x800A /* no rights */
#define FOE_ERR_PERR       0x800B /* programm error */

#define FOE_STATE_IDLE     0
#define FOE_STATE_READ     1
#define FOE_STATE_WRITE    2
#define FOE_STATE_REQUEST  3
#define FOE_STATE_COMMIT   4

#define FOE_MAX_DATA_SIZE  512

#define FOE_HEADER_SIZE    6
#define FOE_MAX_MSGSIZE    122
#define FOE_DATA_SIZE      (FOE_MAX_MSGSIZE-FOE_HEADER_SIZE)

typedef struct {
	unsigned char opcode;
	union {
		uint32_t packetnumber;
		uint32_t password;
		uint32_t error;
	} a;
	union {
		char filename[FOE_DATA_SIZE];
		unsigned char data[FOE_DATA_SIZE];
		char errortext[FOE_DATA_SIZE];
	} b;
} foemsg_t;

/*
 * Internal EtherCAT API
 */

// state machine, showing expected packages
// parse packet store to file - beware of lock

int foe_init(void);
int foe_close(void);

/**
 * @brief Parse new packet
 *
 * @param msg  the message to parse
 * @return 0 no error, 1 no error, reply pending, <0 error
 */
int foe_parse_packet(chanend foe_signal, uint16_t msg[], unsigned size);

/**
 * @brief Get reply packet
 *
 * @param data  data field (16 bit)
 * @return word count of data field
 */
unsigned foe_get_reply(uint16_t data[]);

int foe_request(uint16_t data[]);

/**
 * @brief This funciton is called by ecat_handler()
 *
 * Here a channel commmunication abstraction is realized to allow applications
 * to access the file within the filesystem without the need for shared memory.
 *
 * @see foe_chan.h
 * for a list of available channel commands
 *
 * @param command  The command issued by the caller
 * @param comm     the communication channel to transfer data forth and back.
 * @param 0 on success, otherwise error.
 */
int foe_file_access(chanend comm, int command);
