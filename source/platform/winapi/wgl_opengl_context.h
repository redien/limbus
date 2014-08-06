
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef WGL_OPENGL_CONTEXT_H
#define WGL_OPENGL_CONTEXT_H

#include <windows.h>

typedef struct
{
	PIXELFORMATDESCRIPTOR pfd;
	HDC device_context;
	HGLRC render_context;
} wglContext;

#endif

