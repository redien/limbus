
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include <limbus/filesystem.h>

#include "unix_filesystem.h"

/* TODO: Come up with a better way to support this under ANSI strict */
/*size_t readlink( const char* path, char* buf, size_t size );*/

void* lb_unix_filesystem_construct()
{
	UnixFilesystem* filesystem;
	void* data;

	data = malloc( sizeof *filesystem );
	filesystem = (UnixFilesystem*)data;	
	assert( filesystem );
	
	filesystem->dir = NULL;

	{
    	int len = readlink( "/proc/self/cwd", filesystem->cwd, MAX_PATH - 1 );
    	filesystem->cwd[len] = '\0';
	}

	return filesystem;
}

void lb_unix_filesystem_destruct( void* fs )
{
	UnixFilesystem* filesystem = (UnixFilesystem*)fs;

	if (filesystem->dir)
		closedir( filesystem->dir );

	free( filesystem );
}

LBFilesystemError lb_filesystem_constructed( void* filesystem )
{
	return (filesystem) ? LBFilesystemNoError : LBFilesystemAllocationFailure;
}

int lb_filesystem_path_is_directory( void* fs, const char* path )
{
	struct stat info;
	if (stat( path, &info ) == 0)
		return (S_ISDIR( info.st_mode )) ? 1 : 0;
	else
		return 0;
}

int lb_filesystem_path_is_file( void* fs, const char* path )
{
	return (lb_filesystem_path_is_directory( fs, path ) == 1) ? 0 : 1;
}

int lb_filesystem_directory_list( void* fs, const char* path )
{
	UnixFilesystem* filesystem = (UnixFilesystem*)fs;

	if (filesystem->dir)
		closedir( filesystem->dir );

	filesystem->dir = opendir( path );
	return (filesystem->dir) ? 1 : 0;
}

int lb_filesystem_directory_next_entry( void* fs )
{
	UnixFilesystem* filesystem = (UnixFilesystem*)fs;
	filesystem->dir_entry = readdir( filesystem->dir );
	
	if (filesystem->dir_entry)
	{
		if (strncmp( filesystem->dir_entry->d_name, ".", 1 ) == 0
		||  strncmp( filesystem->dir_entry->d_name, "..", 2 ) == 0)
			return lb_filesystem_directory_next_entry( fs );
	}

	return (filesystem->dir_entry) ? 1 : 0;
}

const char* lb_filesystem_directory_get_entry( void* fs )
{
	UnixFilesystem* filesystem = (UnixFilesystem*)fs;
	return filesystem->dir_entry->d_name;
}

unsigned int lb_filesystem_file_size( void* fs, const char* path )
{
	struct stat info;
	if (stat( path, &info ) == 0)
		return info.st_size;
	else
		return 0;
}

const char* lb_filesystem_get_working_directory( void* fs )
{
	UnixFilesystem* filesystem = (UnixFilesystem*)fs;
	return filesystem->cwd;
}

