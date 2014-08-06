
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

