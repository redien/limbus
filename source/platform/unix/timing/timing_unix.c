
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include <limbus/timing.h>

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

