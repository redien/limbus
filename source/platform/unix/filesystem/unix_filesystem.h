
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define MAX_FILENAME 64
#define MAX_PATH 256

typedef struct UnixFilesystemTag
{
	DIR* dir;
	struct dirent* dir_entry;
	char cwd[MAX_PATH];
} UnixFilesystem;

void* lb_unix_filesystem_construct( void );
void lb_unix_filesystem_destruct( void* fs );

