
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
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
#elif __APPLE__
    #include <OpenGL/gl.h>
    #include <OpenGL/glext.h>
    #define APIENTRY

    void* glGetProcAddress( const char* name );
#else
	#include <GL/gl.h>
	#include <GL/glx.h>

	#define glGetProcAddress( s ) glXGetProcAddress( (const GLubyte *)s )
#endif

#ifdef __cplusplus
}
#endif
#endif

