#ifndef FOE_CHAN_H
#define FOE_CHAN_H

/* defines for file access over channel */

/**
 * commands from the caller
 *
 * The following commands are recognized:
 * FOE_FILE_OPEN <filename>
 * open specified file for reading, replies FOE_FILE_OK on success or FOE_FILE_ERROR on error.
 *
 * FOE_FILE_CLOSE
 * Finish file access operation
 *
 * FOE_FILE_READ <size>
 * read <size> bytes from previously opened file
 *
 * FOE_WRITE <size> <data>
 *** currently unsupported ***
 *
 * FOE_FILE_SEEK <pos>
 * set filepointer to (absolute) position <pos>, the next read/write operation will start from there.
 * With FOE_FILE_SEEK 0 the file pointer is rewind to the beginning of the file.
 *
 * FOE_FILE_FREE
 * This command will erase the file (or files), so further file access is possible.
 */
#define FOE_FILE_OPEN      10
#define FOE_FILE_READ      11
#define FOE_FILE_WRITE     12
#define FOE_FILE_CLOSE     13
#define FOE_FILE_SEEK      14
#define FOE_FILE_FREE      15

/* replies to the caller */
#define FOE_FILE_ACK       20
#define FOE_FILE_ERROR     21
#define FOE_FILE_DATA      22

/* control commands */
#define FOE_FILE_READY     30

#endif /* FOE_CHAN_H */
