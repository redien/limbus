
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include "../../../util/vector.h"
#include "../x11_input_device.h"

#include <limbus/mouse.h>

#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <stdlib.h>
#include <assert.h>

typedef struct MouseEventTag
{
	int type;
	int button;
	int x, y;
} MouseEvent;

typedef struct X11MouseTag
{
	X11InputDevice base;
	int added_to_window;

	Vector events;
	MouseEvent event;
	int visible;

	Cursor default_cursor;
	Cursor null_cursor;
	Pixmap null_pixmap;
} X11Mouse;

#define CAST_MOUSE()\
	X11Mouse* mouse = (X11Mouse*)m;\
	assert( mouse != NULL );

static void construct( void* m )
{
	XColor black = {0, 0, 0};
	char bitmap[] = { 0 };
	CAST_MOUSE()

	/* Create null/default cursor, used to hide/show the cursor */
	mouse->null_pixmap = XCreateBitmapFromData( mouse->base.display, mouse->base.window, bitmap, 1, 1 );
	mouse->null_cursor = XCreatePixmapCursor( mouse->base.display,
											  mouse->null_pixmap, mouse->null_pixmap,
											  &black, &black, 0, 0 );
	mouse->default_cursor = XCreateFontCursor( mouse->base.display, XC_left_ptr );

	lb_mouse_set_visibility( mouse, mouse->visible );

	mouse->added_to_window = 1;
}

#define CONVERT_BUTTON( button ) (button == 1) ? 0 :\
								((button == 3) ? 1 :\
								((button == 2) ? 2 : -1))

static void handle_x11_event( void* m, XEvent event )
{
	CAST_MOUSE()

	if (event.type == MotionNotify)
	{
		XMotionEvent* motion_event = (XMotionEvent*)&event;
		MouseEvent event;

		event.type = LBMouseEventMotion;
		event.x = motion_event->x;
		event.y = motion_event->y;
		
		vector_push_back( &mouse->events, &event );
	}

	if (event.type == ButtonPress)
	{
		XButtonPressedEvent* button_event = (XButtonPressedEvent*)&event;
		MouseEvent event;

		event.type = LBMouseEventButtonPress;
		event.button = CONVERT_BUTTON( button_event->button );
		event.x = button_event->x;
		event.y = button_event->y;

		vector_push_back( &mouse->events, &event );
	}

	if (event.type == ButtonRelease)
	{
		XButtonReleasedEvent* button_event = (XButtonReleasedEvent*)&event;
		MouseEvent event;

		event.type = LBMouseEventButtonRelease;
		event.button = CONVERT_BUTTON( button_event->button );
		event.x = button_event->x;
		event.y = button_event->y;

		vector_push_back( &mouse->events, &event );
	}
}

void* lb_mouse_construct()
{
	X11Mouse* mouse = (X11Mouse*)malloc( sizeof( X11Mouse ) );
	vector_construct( &mouse->events, sizeof( MouseEvent ) );

	mouse->added_to_window = 0;
	mouse->visible = 1;

	mouse->base.construct = &construct;
	mouse->base.handle_x11_event = &handle_x11_event;
	
	return mouse;
}

void lb_mouse_destruct( void* m )
{
	CAST_MOUSE()

	if (mouse->added_to_window)
	{
		XFreeCursor( mouse->base.display, mouse->null_cursor );
		XFreePixmap( mouse->base.display, mouse->null_pixmap );
	}

	vector_destruct( &mouse->events );
	free( mouse );
}

int lb_mouse_constructed( void* mouse )
{
	return (mouse != NULL) ? 1 : 0;
}

#define WARP_POINTER( x, y )\
	XWarpPointer( mouse->base.display,\
				  None, mouse->base.root_window,\
				  0, 0, 0, 0,\
				  x, y );

void lb_mouse_set_x( void* m, int x )
{
	CAST_MOUSE()
	WARP_POINTER( x, lb_mouse_get_y( mouse ) )
}

void lb_mouse_set_y( void* m, int y )
{
	CAST_MOUSE()
	WARP_POINTER( lb_mouse_get_x( mouse ), y )
}

#define QUERY_POINTER( x, y )\
	XQueryPointer( mouse->base.display, mouse->base.window,\
				   &window_out, &window_out,\
				   x, y,\
				   &int_out, &int_out,\
				   (unsigned int*)&int_out )

int lb_mouse_get_x( void* m )
{
	int x;
	Window window_out;
	int int_out;
	CAST_MOUSE()

	QUERY_POINTER( &x, &int_out );
	return x;
}

int lb_mouse_get_y( void* m )
{
	int y;
	Window window_out;
	int int_out;
	CAST_MOUSE()

	QUERY_POINTER( &int_out, &y );
	return y;
}

void lb_mouse_set_visibility( void* m, int state )
{
	CAST_MOUSE()

	if (mouse->added_to_window)
	{
		XDefineCursor( mouse->base.display,
					   mouse->base.window,
					   (state == 0) ? mouse->null_cursor : mouse->default_cursor );
	}
	
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

enum LBMouseEvent lb_mouse_get_event_type( void* m )
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

