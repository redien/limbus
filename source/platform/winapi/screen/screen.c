
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include <limbus/screen.h>
#include "../win_screen.h"

#include <windows.h>

/* Not implemented yet... */

/* TODO: Implement */

void* lb_screen_construct( int screen_num )
{
	return (void*)1;
}

void* lb_screen_destruct( void* screen )
{
	return NULL;
}

int lb_screen_constructed( void* screen )
{
	if (screen != NULL)
		return 1;
	else
		return 0;
}

int lb_screen_get_width( void* screen )
{
	return GetSystemMetrics( SM_CXSCREEN );
}

int lb_screen_get_height( void* screen )
{
	return GetSystemMetrics( SM_CYSCREEN );
}

int lb_screen_modes( void* screen )
{
    return 0;
}

int lb_screen_get_mode_width( void* screen, int mode )
{
    return 0;
}

int lb_screen_get_mode_height( void* screen, int mode )
{
    return 0;
}

int lb_screen_change_mode( void* screen, int mode )
{
    return 0;
}

