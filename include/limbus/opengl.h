
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef LB_OPENGL_H
#define LB_OPENGL_H
#ifdef __cplusplus
extern "C"
{
#endif

#ifdef _WIN32
	#include <windows.h>
	#include <GL/gl.h>

	#define glGetProcAddress wglGetProcAddress
#else
	#include <GL/gl.h>

	#define glGetProcAddress( s ) glXGetProcAddress( (const GLubyte *)s )
#endif

#ifdef __cplusplus
}
#endif
#endif
