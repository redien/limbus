
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include <limbus/filesystem.h>

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_CWD_PATH 256

typedef struct WinAPIFilesystemTag
{
	HANDLE find_handle;
	WIN32_FIND_DATA find_data;
	int first_find;
	char cwd[MAX_CWD_PATH];
} WinAPIFilesystem;

void* lb_filesystem_construct( void )
{
	return malloc( sizeof( WinAPIFilesystem ) );
}

void lb_filesystem_destruct( void* filesystem )
{
	free( filesystem );
}

int lb_filesystem_constructed( void* filesystem )
{
	return (filesystem) ? 1 : 0;
}

int lb_filesystem_path_is_file( void* filesystem, const char* path )
{
    return !lb_filesystem_path_is_directory( filesystem, path );
}

unsigned int lb_filesystem_file_size( void* filesystem, const char* path )
{
	WinAPIFilesystem* fs = (WinAPIFilesystem*)filesystem;
	FindFirstFile( path, &fs->find_data );
	return fs->find_data.nFileSizeLow;
}

int lb_filesystem_path_is_directory( void* filesystem, const char* path )
{
    return (GetFileAttributesA( path ) & FILE_ATTRIBUTE_DIRECTORY) ? 1 : 0;
}

LBFilesystemError lb_filesystem_create_directory( LBFilesystem filesystem, const char* path )
{
	DWORD error;

	if (CreateDirectoryA(path, NULL))
		return LBFilesystemNoError;

	error = GetLastError();
	if (error == ERROR_ALREADY_EXISTS)
		return LBFilesystemDirectoryAlreadyExists;
	else if (error == ERROR_PATH_NOT_FOUND)
		return LBFilesystemPathNotFound;
	else
		return LBFilesystemUnknownError;
}

LBFilesystemError lb_filesystem_remove_directory( LBFilesystem filesystem, const char* path )
{
	DWORD error;

	if (RemoveDirectoryA(path))
		return LBFilesystemNoError;

	error = GetLastError();
	if (error == ERROR_PATH_NOT_FOUND)
		return LBFilesystemPathNotFound;
	else
		return LBFilesystemUnknownError;
}

int lb_filesystem_directory_list( void* filesystem, const char* path )
{
	WinAPIFilesystem* fs = (WinAPIFilesystem*)filesystem;
	char dir[MAX_PATH];

	if (strlen( path ) >= (MAX_PATH - 3))
		return 0;

	sprintf( dir, "%s/*", path );
	fs->find_handle = FindFirstFile( dir, &fs->find_data );

	if (fs->find_handle == INVALID_HANDLE_VALUE)
		return 0;

	fs->first_find = 1;
	return 1;
}

int lb_filesystem_directory_next_entry( void* filesystem )
{
	WinAPIFilesystem* fs = (WinAPIFilesystem*)filesystem;

	if (fs->first_find)
	{
		fs->first_find = 0;
		if (strcmp( fs->find_data.cFileName, "." ) == 0)
			return lb_filesystem_directory_next_entry( filesystem );
	}
	else
	{
		int result = FindNextFile( fs->find_handle, &fs->find_data );
		if (result == 0)
			return 0;

		if (strcmp( fs->find_data.cFileName, ".." ) == 0)
			return lb_filesystem_directory_next_entry( filesystem );
	}

	return 1;
}

const char* lb_filesystem_directory_get_entry( void* filesystem )
{
	WinAPIFilesystem* fs = (WinAPIFilesystem*)filesystem;
	return fs->find_data.cFileName;
}

const char* lb_filesystem_get_working_directory( void* filesystem )
{
	WinAPIFilesystem* fs = (WinAPIFilesystem*)filesystem;
	GetCurrentDirectoryA( MAX_CWD_PATH, fs->cwd );
    return fs->cwd;
}
