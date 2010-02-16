
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef _LB_FILESYSTEM_H_
#define _LB_FILESYSTEM_H_
#ifdef __cplusplus
extern "C"
{
#endif

/* Experimental */

enum LBFilesystemWatchEvent
{
    LBFilesystemWatchEventModified
};

void* lb_filesystem_construct( void );
void* lb_filesystem_destruct( void* filesystem );
int lb_filesystem_constructed( void* filesystem );

int lb_filesystem_path_is_directory( void* filesystem, const char* path );
int lb_filesystem_path_is_file( void* filesystem, const char* path );

int lb_filesystem_directory_list( void* filesystem, const char* directory );
int lb_filesystem_directory_next_entry( void* filesystem );
const char* lb_filesystem_directory_get_entry( void* filesystem );

const char* lb_filesystem_get_working_directory( void* filesystem );

unsigned int lb_filesystem_file_size( void* filesystem, const char* path );

int lb_filesystem_watch_path( void* filesystem, char* path );
void lb_filesystem_remove_watch( void* filesystem, int id );
int lb_filesystem_next_watch_event( void* filesystem );
int lb_filesystem_get_watch_event_id( void* filesystem );
const char* lb_filesystem_get_watch_event_file( void* filesystem );
enum LBFilesystemWatchEvent lb_filesystem_get_watch_event_type( void* filesystem );

#ifdef __cplusplus
}
#endif
#endif

