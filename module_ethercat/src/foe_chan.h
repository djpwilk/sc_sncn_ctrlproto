#ifndef FOE_CHAN_H
#define FOE_CHAN_H

/* defines for file access over channel */

/* commands from the caller */
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

#endif /* FOE_CHAN_H */
