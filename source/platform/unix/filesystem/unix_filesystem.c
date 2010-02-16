
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include <limbus/filesystem.h>

#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

/* TODO: Break out linux specifics into it's own file */
#ifndef linux
    #define linux
#endif

#ifdef linux
    #include <sys/inotify.h>
#endif

#include "../../../util/vector.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define MAX_FILENAME 64
#define MAX_PATH 256

#ifdef linux
typedef struct _WatchEvent
{
    int id;
    enum LBFilesystemWatchEvent type;
    char name[MAX_FILENAME];
} WatchEvent;
#endif

typedef struct UnixFilesystemTag
{
	DIR* dir;
	struct dirent* dir_entry;
	char cwd[MAX_PATH];
#ifdef linux
    int inotify_fd;
    Vector watch_events;
    WatchEvent watch_event;
#endif
} UnixFilesystem;

/* TODO: Come up with a better way to support this under ANSI strict */
size_t readlink( const char* path, char* buf, size_t size );

void* lb_filesystem_construct()
{
	UnixFilesystem* filesystem;
	void* data;

	data = malloc( sizeof *filesystem );
	filesystem = (UnixFilesystem*)data;	
	assert( filesystem );
	
	filesystem->dir = NULL;

	#ifdef linux
	filesystem->inotify_fd = inotify_init();
	assert( filesystem->inotify_fd >= 0 );
	vector_construct( &filesystem->watch_events, sizeof( WatchEvent ) );

	{
    	int len = readlink( "/proc/self/cwd", filesystem->cwd, MAX_PATH - 1 );
    	filesystem->cwd[len] = '\0';
	}
	#endif

	return filesystem;
}

void* lb_filesystem_destruct( void* fs )
{
	UnixFilesystem* filesystem = (UnixFilesystem*)fs;

	#ifdef linux
	vector_destruct( &filesystem->watch_events );
    close( filesystem->inotify_fd );
    #endif

	if (filesystem->dir)
		closedir( filesystem->dir );

	free( filesystem );
	return NULL;
}

int lb_filesystem_constructed( void* filesystem )
{
	return (filesystem) ? 1 : 0;
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


#ifdef linux
int lb_filesystem_watch_path( void* fs, char* path )
{
	UnixFilesystem* filesystem = (UnixFilesystem*)fs;
    return inotify_add_watch( filesystem->inotify_fd,
                              path,
                              IN_ALL_EVENTS );
}

void lb_filesystem_remove_watch( void* fs, int id )
{
	UnixFilesystem* filesystem = (UnixFilesystem*)fs;
    inotify_rm_watch( filesystem->inotify_fd, id );
}

int lb_filesystem_next_watch_event( void* fs )
{
	UnixFilesystem* filesystem = (UnixFilesystem*)fs;

    if (vector_size( &filesystem->watch_events ) > 0)
    {
        WatchEvent* event = vector_next( &filesystem->watch_events, NULL );
        filesystem->watch_event = *event;
        vector_erase( &filesystem->watch_events, event );
        return 1;
    }
    else
    {
        struct timeval time = {0, 0};
        fd_set fds;
        int result;
        
        FD_ZERO( &fds );
        FD_SET( filesystem->inotify_fd, &fds );
        
        result = select( filesystem->inotify_fd + 1, &fds, NULL, NULL, &time );

        if (result < 0)
            return 0;
        
        if (FD_ISSET( filesystem->inotify_fd, &fds ))
        {
            #define EVENT_SIZE (sizeof( struct inotify_event ))
            #define BUFFER_LENGTH (1024 * (EVENT_SIZE + 16))
            
            char buffer[BUFFER_LENGTH];
            int result_size, offset;
            
            result_size = read( filesystem->inotify_fd, buffer, BUFFER_LENGTH );
            
            if (result_size <= 0)
                return 0;
            
            offset = 0;
            while (offset < result_size)
            {
                WatchEvent new_event;
                int printf( char*, ... );
                struct inotify_event* event = (struct inotify_event*)&buffer[offset];
                
                if (event->len)
                {
                    strncpy( new_event.name, event->name, MAX_FILENAME - 1 );
                    new_event.name[MAX_FILENAME - 1] = '\0';
                }
                else
                {
                    new_event.name[0] = '\0';
                }

                if (event->mask & IN_MODIFY
                ||  event->mask & IN_IGNORED
                ||  event->mask & IN_MOVED_FROM)
                {
                    new_event.id = event->wd;
                    new_event.type = LBFilesystemWatchEventModified;
                    vector_push_back( &filesystem->watch_events, &new_event );
                }

/*
                #define debug_type( type )\
                    if(event->mask & type)\
                        printf("    " #type "\n");
                
                debug_type( IN_ACCESS )
                debug_type( IN_ATTRIB )
                debug_type( IN_CLOSE_WRITE )
                debug_type( IN_CLOSE_NOWRITE )
                debug_type( IN_CREATE )
                debug_type( IN_DELETE )
                debug_type( IN_DELETE_SELF )
                debug_type( IN_MODIFY )
                debug_type( IN_MOVE_SELF )
                debug_type( IN_MOVED_FROM )
                debug_type( IN_MOVED_TO )
                debug_type( IN_OPEN )
                debug_type( IN_UNMOUNT )
                debug_type( IN_Q_OVERFLOW )
                debug_type( IN_IGNORED )
                debug_type( IN_ISDIR )
*/

                offset += EVENT_SIZE + event->len;
            }

            if (vector_size( &filesystem->watch_events ) > 0)
                return lb_filesystem_next_watch_event( filesystem );
            else
                return 0;
        }
    }

    return 0;
}

int lb_filesystem_get_watch_event_id( void* fs )
{
	UnixFilesystem* filesystem = (UnixFilesystem*)fs;
	return filesystem->watch_event.id;
}

enum LBFilesystemWatchEvent lb_filesystem_get_watch_event_type( void* fs )
{
	UnixFilesystem* filesystem = (UnixFilesystem*)fs;
	return filesystem->watch_event.type;
}

const char* lb_filesystem_get_watch_event_file( void* fs )
{
	UnixFilesystem* filesystem = (UnixFilesystem*)fs;
	return filesystem->watch_event.name;
}

#endif /* linux */
