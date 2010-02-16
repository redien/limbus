
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include "../../timing.h"

#include <windows.h>

double time_count_in_seconds( void )
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
