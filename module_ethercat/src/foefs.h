/* foefs.h
 *
 * API for simple and specialized filesystem for use within the EtherCAT module.
 *
 * 2012-03-29 Frank Jeschke <jeschke@fjes.de>
 *
 * FIXME: in the moment only one file is supported. Future versions could
 * provide a reserved are in RAM to hold a simple filesystem.
 */

#ifndef FOEFS_H
#define FOEFS_H

#define BLKSZ      10000
#define MAX_FNAME  10
#define SEEK_SET   0
#define SEEK_CUR   1
#define MODE_RO    0
#define MODE_RW    1

#define REQUEST_FILE   1    ///< request file from ethercat master
#define COMMIT_FILE    2   ///< push file to ehtercat master

/**
 * @brief Filesystem entry
 *
 * This is a file representation for a single file within the
 * pseude filesystem.
 */
typedef struct {
	int fh;                ///< Filehandle to access the file
	char name[MAX_FNAME];  ///< filename
	unsigned int size;     ///< file size in bytes
	unsigned int type;     ///< file type
	char bytes[BLKSZ];     ///< File content itself
	int currentpos;        ///< Current position within the file where the next r/w access is done
	int mode;              ///< Access to file: MODE_RW, MODE_RO
	//int transmit_to_master???
} foefile_t;

/**
 * @brief open file
 *
 * @param filename  filename to open
 * @return file handle, <0 on error
 */
int foefs_open(char filename[], int mode);

/**
 * @brief close file handle
 *
 * @param fh  file handle to close
 * @return 0 on success, <0 otherwise
 */
int foefs_close(int fh);

/** 
 * @brief read from file handle
 *
 * @param fh  file handle to read from
 * @param size  number of byes to read
 * @param b[]   buffer to store read bytes
 * @return number of bytes read, <0 on error
 */
int foefs_read(int fh, int size, char b[]);

/**
 * @brief write bytes to file starting at offset
 *
 * @param fh    filehandle to write to
 * @param size  number of bytes to write
 * @param b[]   buffer holding bytes to write
 * @return number of bytes written, <0 on error
 */
int foefs_write(int fh, int size, char b[]);

/**
 * @brief set file position
 *
 * @param fh   filehandle to operate on
 * @param offset  new offset of filehandle
 * @param where   either SEEK_SET for absolute positioning or SEEK_CUR for relative positioning.
 * @param 0 on success
 */
int foefs_seek(int fh, int offset, int whence);

/*
    intermodule interface (private)
 */

/**
 * @brief init pseudo filesystem
 * 
 * @return 0 on success
 */
int foefs_init(void);

/**
 * @brief release pseude filesystem
 *
 * @return 0 on success
 */
int foefs_release(void);

/**
 * @brief format file system
 *
 * this is basically a reinitalize.
 *
 * @return 0 on success
 */
int foefs_format(void);

/**
 * @brief Request the number of free bytes.
 *
 * @return Number of available bytes.
 */
int foefs_free(void);

#endif /* FOEFS_H */
