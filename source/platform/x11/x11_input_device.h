
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef X11_INPUT_DEVICE_H
#define X11_INPUT_DEVICE_H

#include <X11/Xlib.h>

typedef struct X11InputDeviceTag
{
	Display* display;
	Window window;
	Window root_window;
	XIC input_context;

	void (*construct)( void* device );
	void (*handle_x11_event)( void* device, XEvent event );
	/* TODO: Add destruct callback */
} X11InputDevice;

#endif

