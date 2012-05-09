/* foefs.c
 *
 * 2012-03-29 Frank jeschke <jeschke@fjes.de>
 */

#include <xs1.h>
#include <platform.h>
#include <string.h>

#include "foe.h"
#include "foefs.h"

/* FIXME currently only one file in the file system is allowed */
foefile_t filesystem;
unsigned freespace;

/* application interface (public) */

/* FIXME check if file exists, than check mode */
int foefs_open(char filename[], int mode)
{
	if (freespace > 0) { /* new file */
		freespace--;
	}

	if ((mode == MODE_RO) && (strncmp(filename, filesystem.name, MAX_FNAME) != 0)) {
		return -FOE_ERR_NOTFOUND;
	}

	/* is file already open, and/or currently in use */
	if (filesystem.fh != 0) {
		return -FOE_ERR_PERM;
	}

	filesystem.fh++;
	filesystem.currentpos = 0; /* rewind */
	filesystem.mode = mode;

	return filesystem.fh;
}

int foefs_close(int fh)
{
	filesystem.fh = 0;
	return filesystem.fh;
}

int foefs_read(int fh, int size, char b[])
{
	int readcount = 0;
	int i;

	if (fh != filesystem.fh) {
		return -FOE_ERR_NOACCESS;
	}

	for (i=filesystem.currentpos; i<size && i<filesystem.size; i++, readcount++) {
		b[readcount] = filesystem.bytes[i];
	}

	return readcount;
}

/*
FIXME: what happens if offset is larger than file.size at the beginning of write cycle?
*/
int foefs_write(int fh, int size, char b[])
{
	int writecount = 0;
	int i;

	if (fh != filesystem.fh) {
		return -FOE_ERR_NOACCESS;
	}

	if (filesystem.mode == MODE_RO) {
		return -FOE_ERR_PERM;
	}

	if (BLKSZ < filesystem.currentpos + size) {
		return -FOE_ERR_ILLEGAL;
	}

	for (i=filesystem.currentpos; i<size; i++, writecount++) {
		filesystem.bytes[i] = b[writecount];
	}

	filesystem.size += writecount;

	return writecount;
}

int foefs_seek(int fh, int offset, int whence)
{
	if (fh != filesystem.fh) {
		return -FOE_ERR_NOACCESS;
	}

	switch (whence) {
	case SEEK_SET:
		filesystem.currentpos = offset;
		break;
	case SEEK_CUR:
		filesystem.currentpos += offset;
		break;
	}

	return filesystem.currentpos;
}

/* intermodule interface (private) */

int foefs_init()
{
	filesystem.fh = 0;
	filesystem.name[0] = 0; /* empty name */
	filesystem.size = 0;
	freespace = 1;

	return 0;
}

/* placeholder */
int foefs_release()
{
	return 0;
}

int foefs_format()
{
	return foefs_init();
}

int foefs_remove(int fh)
{
	int i=0;

	if (freespace > 0) {
		freespace--;
	}

	filesystem.fh = 0;
	filesystem.size = 0;
	filesystem.currentpos = 0;
	freespace++;

	for (i=0; i<MAX_FNAME; i++) {
		filesystem.name[i] = 0;
	}

	return 0;
}

int foefs_free(void)
{
	return freespace;
}
