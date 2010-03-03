
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef _LB_OPENGL_CONTEXT_H_
#define _LB_OPENGL_CONTEXT_H_
#ifdef __cplusplus
extern "C"
{
#endif

/* TODO: Change prefix from LBOpengl to LBOpenglContext for consistency */
/* TODO: Change attribute accessor from using enumerations to standalone
         functions to avoid crazy names as well as for consistency */

enum LBOpenglAttribute
{
	LBOpenglAttributeDoubleBuffer,
	LBOpenglAttributeOpenglSupport,
	LBOpenglAttributeRGBA,
	LBOpenglAttributeDepthSize
};

#define LBOpenglDefaultPixelformat 0

#define LBOpenglContextCreateNew 0
#define LBOpenglContextUseCurrent 1

void* lb_opengl_context_construct_in_window( void* window, int use_current );
void* lb_opengl_context_destruct( void* context );
void lb_opengl_context_release( void* context );
int lb_opengl_context_constructed( void* context );

void lb_opengl_context_swap_buffers( void* context );

int lb_opengl_context_pixelformats( void* context );
int lb_opengl_context_get_attribute( void* context,
                                     int pixelformat,
                                     enum LBOpenglAttribute attribute );
void lb_opengl_context_set_pixelformat( void* context, int pixelformat );

void lb_opengl_context_make_current( void* context );
void lb_opengl_context_release_current( void* context );

#ifdef __cplusplus
}
#endif
#endif

