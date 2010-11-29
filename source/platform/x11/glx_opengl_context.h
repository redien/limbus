
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef GLX_OPENGL_CONTEXT_H
#define GLX_OPENGL_CONTEXT_H

#include <X11/Xlib.h>
#include <GL/glx.h>

typedef struct
{
	Display* display;
	int screen;
	GLXContext context;
	Window window;
	void* impl_data;
} glXContext;

#endif

