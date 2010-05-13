
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

void* lb_opengl_context_construct_in_window( void* window, int use_current );
void* lb_opengl_context_destruct( void* context );
void lb_opengl_context_release_ownership( void* context );
int lb_opengl_context_constructed( void* context );

void lb_opengl_context_swap_buffers( void* context );

int lb_opengl_context_pixelformats( void* context );
int lb_opengl_context_get_pixelformat_double_buffer( void* context,
                                                     int pixelformat );
int lb_opengl_context_get_pixelformat_support_opengl( void* context,
                                                     int pixelformat );
int lb_opengl_context_get_pixelformat_rgba( void* context,
                                            int pixelformat );
int lb_opengl_context_get_pixelformat_depth_size( void* context,
                                                  int pixelformat );
void lb_opengl_context_set_pixelformat( void* context, int pixelformat );

void lb_opengl_context_make_current( void* context );
void lb_opengl_context_release_current( void* context );

void lb_opengl_context_set_swap_interval( void* context, int interval );

#ifdef __cplusplus
}
#endif
#endif
