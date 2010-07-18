
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef _X11_WINDOW_H_
#define _X11_WINDOW_H_

#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef struct
{
	Display* display;
	int screen;
	Window window;
	Window root_window;
	void (*create_window_impl)(void*, XVisualInfo*);
	void (*destroy_window_impl)(void*);
} X11Window;

#endif

