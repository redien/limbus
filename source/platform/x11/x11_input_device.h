
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
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

