
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef _WGL_OPENGL_CONTEXT_H_
#define _WGL_OPENGL_CONTEXT_H_

#include <windows.h>

typedef struct
{
	PIXELFORMATDESCRIPTOR pfd;
	HDC device_context;
	HGLRC render_context;
	void* impl_data;
} wglContext;

#endif

