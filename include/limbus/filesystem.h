
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef LB_FILESYSTEM_H
#define LB_FILESYSTEM_H
#ifdef __cplusplus
extern "C"
{
#endif

/* Experimental */

/* An enumeration of the API-specific errors. */
enum LBFilesystemError
{
    /* No error occured */
    LBFilesystemNoError,
    /* Failed to allocate memory */
    LBFilesystemAllocationFailure,
	/* A path that does not exist was passed to a function */
	LBFilesystemPathNotFound,
	/* A directory with the specified path already exists */
	LBFilesystemDirectoryAlreadyExists,
	/* An error occured but the type of error could not be determined */
	LBFilesystemUnknownError
};
typedef enum LBFilesystemError LBFilesystemError;

typedef void* LBFilesystem;

enum LBFilesystemWatchEvent
{
    LBFilesystemWatchEventModified
};
typedef enum LBFilesystemWatchEvent LBFilesystemWatchEvent;

LBFilesystem lb_filesystem_construct( void );
void lb_filesystem_destruct( LBFilesystem filesystem );
LBFilesystemError lb_filesystem_constructed( LBFilesystem filesystem );

int lb_filesystem_path_is_file( LBFilesystem filesystem, const char* path );
unsigned int lb_filesystem_file_size( LBFilesystem filesystem, const char* path );

int lb_filesystem_path_is_directory( LBFilesystem filesystem, const char* path );
LBFilesystemError lb_filesystem_create_directory( LBFilesystem filesystem, const char* path );
LBFilesystemError lb_filesystem_remove_directory( LBFilesystem filesystem, const char* path );
const char* lb_filesystem_get_working_directory( LBFilesystem filesystem );
int lb_filesystem_directory_list( LBFilesystem filesystem, const char* path );
int lb_filesystem_directory_next_entry( LBFilesystem filesystem );
const char* lb_filesystem_directory_get_entry( LBFilesystem filesystem );

int lb_filesystem_watch_path( LBFilesystem filesystem, char* path );
void lb_filesystem_remove_watch( LBFilesystem filesystem, int id );
int lb_filesystem_next_watch_event( LBFilesystem filesystem );
int lb_filesystem_get_watch_event_id( LBFilesystem filesystem );
const char* lb_filesystem_get_watch_event_file( LBFilesystem filesystem );
LBFilesystemWatchEvent lb_filesystem_get_watch_event_type( LBFilesystem filesystem );

#ifdef __cplusplus
}
#endif
#endif

