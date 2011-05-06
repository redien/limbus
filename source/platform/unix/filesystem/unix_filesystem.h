
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
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

