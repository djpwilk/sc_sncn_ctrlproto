/* foe.c

   2012-03-29 Frank jeschke <jeschke@fjes.de>
 */

#include <xs1.h>
#include <string.h>

#include "foe.h"

/* FIXME currently only one file in the file system is allowed */
foefile_t filesystem;

/* application interface (public) */

int foe_open(char filename[])
{
	if (strncmp(filename, filesystem.name, MAX_FNAME) != 0) {
		return -FOE_ERR_NOTFOUND;
	}

	if (filesystem.fh != 0) { /* file already open, and/or currently in use */
		return -FOE_ERR_PERM;
	}

	filesystem.fh++;
	filesystem.currentpos = 0; /* rewind */

	return filesystem.fh;
}

int foe_close(int fh)
{
	filesystem.fh = 0;
	return filesystem.fh;
}

int foe_read(int fh, int size, char b[])
{
	int readcount = 0;
	int i;

	if (fh != filesystem.fh) {
		return -FOE_ERR_NOACCESS;
	}

	if (filesystem.size < filesystem.currentpos + size) {
		return -FOE_ERR_ILLEGAL;
	}

	for (i=filesystem.currentpos; i<size; i++, readcount++) {
		b[readcount] = filesystem.bytes[i];
	}

	return readcount;
}

/*
FIXME: what happens if offset is larger than file.size at the beginning of write cycle?
*/
int foe_write(int fh, int size, char b[])
{
	int writecount = 0;
	int i;

	if (fh != filesystem.fh) {
		return -FOE_ERR_NOACCESS;
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

int foe_seek(int fh, int offset, int whence)
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

int foe_init()
{
	filesystem.fh = 0;
	filesystem.name[0] = 0; /* empty name */
	filesystem.size = 0;
	return 0;
}

/* placeholder */
int foe_release()
{
	return 0;
}

int foe_format()
{
	return foe_init();
}

