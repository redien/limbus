
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef _X11_SCREEN_H_
#define _X11_SCREEN_H_

#include <X11/Xlib.h>

typedef struct
{
	Display* display;
	int screen;
} X11Screen;

#endif

