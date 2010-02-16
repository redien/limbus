
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include "../../../util/vector.h"
#include "../../mouse.h"
#include "../winapi_input_device.h"

#include <windows.h>
#include <assert.h>

typedef struct MouseEventTag
{
	int type;
	int button;
	int x, y;
} MouseEvent;

typedef struct WinAPIMouseTag
{
	WinAPIInputDevice base;

	Vector events;
	MouseEvent event;
} WinAPIMouse;

#define CAST_MOUSE()\
	WinAPIMouse* mouse = (WinAPIMouse*)m;\
	assert( mouse != NULL );

static void construct( void* m )
{
	CAST_MOUSE()
}

static int handle_winapi_message( void* m, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* result )
{
	MouseEvent event;
	CAST_MOUSE()

	if (uMsg == WM_LBUTTONDOWN)
	{
		event.type = MouseEventButtonPress;
		event.button = 0;
		event.x = LOWORD( lParam );
		event.y = HIWORD( lParam );
		vector_push_back( &mouse->events, &event );
	}
	else if (uMsg == WM_RBUTTONDOWN)
	{
		event.type = MouseEventButtonPress;
		event.button = 1;
		event.x = LOWORD( lParam );
		event.y = HIWORD( lParam );
		vector_push_back( &mouse->events, &event );
	}
	else if (uMsg == WM_LBUTTONUP)
	{
		event.type = MouseEventButtonRelease;
		event.button = 0;
		event.x = LOWORD( lParam );
		event.y = HIWORD( lParam );
		vector_push_back( &mouse->events, &event );
	}
	else if (uMsg == WM_RBUTTONUP)
	{
		event.type = MouseEventButtonRelease;
		event.button = 1;
		event.x = LOWORD( lParam );
		event.y = HIWORD( lParam );
		vector_push_back( &mouse->events, &event );
	}
	else if (uMsg == WM_MOUSEMOVE)
	{
		event.type = MouseEventMotion;
		event.x = LOWORD( lParam );
		event.y = HIWORD( lParam );
		vector_push_back( &mouse->events, &event );
	}

	return 0;
}

void* mouse_construct()
{
	WinAPIMouse* mouse = (WinAPIMouse*)malloc( sizeof( WinAPIMouse ) );
	vector_construct( &mouse->events, sizeof( MouseEvent ), 3 );

	mouse->base.construct = &construct;
	mouse->base.handle_winapi_message = &handle_winapi_message;

	return mouse;
}

void mouse_destruct( void* m )
{
	CAST_MOUSE()
	vector_destruct( &mouse->events );
	free( mouse );
}

int mouse_constructed( void* mouse )
{
	return (mouse != NULL) ? 1 : 0;
}

void mouse_set_x( void* mouse, int x )
{
	POINT p;
	BOOL result;
	result = GetCursorPos( &p );
	assert( result );
	result = SetCursorPos( x, p.y );
	assert( result );
}

void mouse_set_y( void* mouse, int y )
{
	POINT p;
	BOOL result;
	result = GetCursorPos( &p );
	assert( result );
	result = SetCursorPos( p.x, y );
	assert( result );
}

int mouse_get_x( void* mouse )
{
	POINT p;
	BOOL result;
	result = GetCursorPos( &p );
	assert( result );
	return p.x;
}

int mouse_get_y( void* mouse )
{
	POINT p;
	BOOL result;
	result = GetCursorPos( &p );
	assert( result );
	return p.y;
}

void mouse_cursor_show( void* mouse, int state )
{
	ShowCursor( (state == 1) ? TRUE : FALSE );
}


int mouse_next_event( void* m )
{
	MouseEvent* iter;
	CAST_MOUSE()

	iter = (MouseEvent*)vector_next( &mouse->events, NULL );
	if (iter != NULL)
	{
		mouse->event = *iter;
		vector_erase( &mouse->events, iter );
		return 1;
	}
	else
	{
		return 0;
	}
}

enum MouseEvent mouse_get_event_type( void* m )
{
	CAST_MOUSE()
	return mouse->event.type;
}

int mouse_get_event_button( void* m )
{
	CAST_MOUSE()
	return mouse->event.button;
}

int mouse_get_event_x( void* m )
{
	CAST_MOUSE()
	return mouse->event.x;
}

int mouse_get_event_y( void* m )
{
	CAST_MOUSE()
	return mouse->event.y;
}
