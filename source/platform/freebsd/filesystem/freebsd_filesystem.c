
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include <limbus/filesystem.h>
#include "../../unix/filesystem/unix_filesystem.h"

void* lb_filesystem_construct()
{
	return lb_unix_filesystem_construct();
}

void* lb_filesystem_destruct( void* fs )
{
	return lb_unix_filesystem_destruct( fs );
}

int lb_filesystem_watch_path( void* fs, char* path )
{
    return 0;
}

void lb_filesystem_remove_watch( void* fs, int id )
{
}

int lb_filesystem_next_watch_event( void* fs )
{
    return 0;
}

int lb_filesystem_get_watch_event_id( void* fs )
{
	return 0;
}

enum LBFilesystemWatchEvent lb_filesystem_get_watch_event_type( void* fs )
{
	return 0;
}

const char* lb_filesystem_get_watch_event_file( void* fs )
{
	return 0;
}

