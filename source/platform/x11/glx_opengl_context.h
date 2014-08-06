
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
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

