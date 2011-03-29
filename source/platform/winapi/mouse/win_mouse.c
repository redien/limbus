
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include "../../../util/vector.h"
#include <limbus/mouse.h>
#include "../winapi_input_device.h"

#include <windows.h>
#include <assert.h>

typedef struct MouseEventTag
{
	LBMouseEvent type;
	LBMouseButton button;
	int x, y;
} MouseEvent;

typedef struct WinAPIMouseTag
{
	WinAPIInputDevice base;

	Vector events;
	MouseEvent event;
	int visible;
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
		event.type = LBMouseEventButtonPress;
		event.button = 0;
		event.x = LOWORD( lParam );
		event.y = HIWORD( lParam );
		vector_push_back( &mouse->events, &event );
		*result = 0;
		return 1;
	}
	else if (uMsg == WM_RBUTTONDOWN)
	{
		event.type = LBMouseEventButtonPress;
		event.button = 1;
		event.x = LOWORD( lParam );
		event.y = HIWORD( lParam );
		vector_push_back( &mouse->events, &event );
		*result = 0;
		return 1;
	}
	else if (uMsg == WM_MBUTTONDOWN)
	{
		event.type = LBMouseEventButtonPress;
		event.button = 2;
		event.x = LOWORD( lParam );
		event.y = HIWORD( lParam );
		vector_push_back( &mouse->events, &event );
		*result = 0;
		return 1;
	}
	else if (uMsg == WM_LBUTTONUP)
	{
		event.type = LBMouseEventButtonRelease;
		event.button = 0;
		event.x = LOWORD( lParam );
		event.y = HIWORD( lParam );
		vector_push_back( &mouse->events, &event );
		*result = 0;
		return 1;
	}
	else if (uMsg == WM_RBUTTONUP)
	{
		event.type = LBMouseEventButtonRelease;
		event.button = 1;
		event.x = LOWORD( lParam );
		event.y = HIWORD( lParam );
		vector_push_back( &mouse->events, &event );
		*result = 0;
		return 1;
	}
	else if (uMsg == WM_MBUTTONUP)
	{
		event.type = LBMouseEventButtonRelease;
		event.button = 2;
		event.x = LOWORD( lParam );
		event.y = HIWORD( lParam );
		vector_push_back( &mouse->events, &event );
		*result = 0;
		return 1;
	}
	else if (uMsg == WM_MOUSEMOVE)
	{
		event.type = LBMouseEventMotion;
		event.x = LOWORD( lParam );
		event.y = HIWORD( lParam );
		vector_push_back( &mouse->events, &event );
		*result = 0;
		return 1;
	}

	return 0;
}

void* lb_mouse_construct()
{
	WinAPIMouse* mouse = (WinAPIMouse*)malloc( sizeof( WinAPIMouse ) );
	vector_construct( &mouse->events, sizeof( MouseEvent ) );

	mouse->base.construct = &construct;
	mouse->base.handle_winapi_message = &handle_winapi_message;
	mouse->visible = 1;

	return mouse;
}

void lb_mouse_destruct( void* m )
{
	CAST_MOUSE()
	vector_destruct( &mouse->events );
	free( mouse );
}

int lb_mouse_constructed( void* mouse )
{
	return (mouse != NULL) ? 1 : 0;
}

void lb_mouse_set_x( void* mouse, int x )
{
	POINT p;
	BOOL result;
	result = GetCursorPos( &p );
	assert( result );
	result = SetCursorPos( x, p.y );
	assert( result );
}

void lb_mouse_set_y( void* mouse, int y )
{
	POINT p;
	BOOL result;
	result = GetCursorPos( &p );
	assert( result );
	result = SetCursorPos( p.x, y );
	assert( result );
}

int lb_mouse_get_x( void* mouse )
{
	POINT p;
	BOOL result;
	result = GetCursorPos( &p );
	assert( result );
	return p.x;
}

int lb_mouse_get_y( void* mouse )
{
	POINT p;
	BOOL result;
	result = GetCursorPos( &p );
	assert( result );
	return p.y;
}

void lb_mouse_set_visibility( void* m, int state )
{
	CAST_MOUSE()
	ShowCursor( (state == 1) ? TRUE : FALSE );
	mouse->visible = state;
}

int lb_mouse_get_visibility( void* m )
{
	CAST_MOUSE()
	return mouse->visible;
}

int lb_mouse_next_event( void* m )
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

LBMouseEvent lb_mouse_get_event_type( void* m )
{
	CAST_MOUSE()
	return mouse->event.type;
}

LBMouseButton lb_mouse_get_event_button( void* m )
{
	CAST_MOUSE()
	return mouse->event.button;
}

int lb_mouse_get_event_x( void* m )
{
	CAST_MOUSE()
	return mouse->event.x;
}

int lb_mouse_get_event_y( void* m )
{
	CAST_MOUSE()
	return mouse->event.y;
}

