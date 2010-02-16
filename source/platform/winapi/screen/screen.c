
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include "../../screen.h"
#include "../win_screen.h"

#include <windows.h>

/* Not implemented yet */

void* screen_construct( int screen_num )
{
	return (void*)1;
}

void* screen_destruct( void* screen )
{
	return NULL;
}

int screen_constructed( void* screen )
{
	if (screen != NULL)
		return 1;
	else
		return 0;
}

int screen_get_width( void* screen )
{
	return GetSystemMetrics( SM_CXSCREEN );
}

int screen_get_height( void* screen )
{
	return GetSystemMetrics( SM_CYSCREEN );
}
