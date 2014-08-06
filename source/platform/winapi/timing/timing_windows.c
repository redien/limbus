
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

#include <windows.h>

double lb_time_count_in_seconds( void )
{
	LARGE_INTEGER frequency;

	if (QueryPerformanceFrequency( &frequency ))
	{
		LARGE_INTEGER time;
		QueryPerformanceCounter( &time );
		return (double)time.QuadPart / (double)frequency.QuadPart;
	}
	else
	{
		return (double)GetTickCount() / 1000.0;
	}
}

unsigned long lb_time_count_in_milliseconds( void )
{
	LARGE_INTEGER frequency;

	if (QueryPerformanceFrequency( &frequency ))
	{
		LARGE_INTEGER time;
		QueryPerformanceCounter( &time );
		return (unsigned long)time.QuadPart / (unsigned long)(frequency.QuadPart / 1000);
	}
	else
	{
		return GetTickCount();
	}
}

void lb_msleep( unsigned int msec )
{
	Sleep( msec );
}

void lb_usleep( unsigned int usec )
{
	Sleep( usec / 1000 );
}
