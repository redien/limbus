
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef LB_OPENGL_CONTEXT_H
#define LB_OPENGL_CONTEXT_H
#ifdef __cplusplus
extern "C"
{
#endif

#define LBOpenglContextDefaultPixelformat 0

#define LBOpenglContextCreateNew 0
#define LBOpenglContextUseCurrent 1

typedef void* LBOpenglContext;

LBOpenglContext lb_opengl_context_construct_in_window( LBOpenglContext window, int use_current );
LBOpenglContext lb_opengl_context_destruct( LBOpenglContext context );
void lb_opengl_context_release_ownership( LBOpenglContext context );
int lb_opengl_context_constructed( LBOpenglContext context );

void lb_opengl_context_swap_buffers( LBOpenglContext context );

int lb_opengl_context_pixelformats( LBOpenglContext context );
int lb_opengl_context_get_pixelformat_double_buffer( LBOpenglContext context,
                                                     int pixelformat );
int lb_opengl_context_get_pixelformat_support_opengl( LBOpenglContext context,
                                                     int pixelformat );
int lb_opengl_context_get_pixelformat_rgba( LBOpenglContext context,
                                            int pixelformat );
int lb_opengl_context_get_pixelformat_depth_size( LBOpenglContext context,
                                                  int pixelformat );
void lb_opengl_context_set_pixelformat( LBOpenglContext context, int pixelformat );

void lb_opengl_context_make_current( LBOpenglContext context );
void lb_opengl_context_release_current( LBOpenglContext context );

int lb_opengl_context_set_swap_interval( LBOpenglContext context, int interval );

#ifdef __cplusplus
}
#endif
#endif

