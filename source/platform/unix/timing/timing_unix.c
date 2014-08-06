
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include <limbus/timing.h>

#define _XOPEN_SOURCE 500
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

double lb_time_count_in_seconds( void )
{
	struct timeval time_now;
	gettimeofday( &time_now, NULL );
	
	return time_now.tv_sec +
		   time_now.tv_usec / 1000000.0;
}

unsigned long lb_time_count_in_milliseconds( void )
{
	struct timeval time_now;
	gettimeofday( &time_now, NULL );
	
	return time_now.tv_sec * 1000 +
		   time_now.tv_usec / 1000;
}

/* TODO: Come up with a better way to support this under ANSI strict */
/*int usleep( unsigned long usec );*/

void lb_msleep( unsigned int msec )
{
    usleep( msec * 1000L );
}

void lb_usleep( unsigned int usec )
{
    usleep( usec );
}

