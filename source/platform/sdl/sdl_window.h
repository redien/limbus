
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef X11_WINDOW_H
#define X11_WINDOW_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "x11_screen.h"

typedef struct
{
	X11Screen* screen;
	Window window;
	Window root_window;
	void (*create_window_impl)(void*, XVisualInfo*);
	void (*destroy_window_impl)(void*);
} X11Window;

#endif
