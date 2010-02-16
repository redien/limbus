
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef _LB_WINDOW_H_
#define _LB_WINDOW_H_
#ifdef __cplusplus
extern "C"
{
#endif

enum LBWindowEvent
{
	LBWindowEventClose,
	LBWindowEventFileDrop
};

void* lb_window_construct( void* screen );
void* lb_window_destruct( void* window );
int lb_window_constructed( void* window );

void lb_window_clear_decoration( void* window );

void lb_window_set_caption( void* window, const char* caption );
void lb_window_set_width( void* window, int width );
void lb_window_set_height( void* window, int height );
void lb_window_set_x( void* window, int x );
void lb_window_set_y( void* window, int y );

const char* lb_window_get_caption( void* window );
int lb_window_get_width( void* window );
int lb_window_get_height( void* window );
int lb_window_get_x( void* window );
int lb_window_get_y( void* window );

int lb_window_next_event( void* window );
enum LBWindowEvent lb_window_get_event_type( void* window );
int lb_window_get_event_x( void* window );
int lb_window_get_event_y( void* window );
int lb_window_get_event_files( void* window );
const char* lb_window_get_event_file( void* window, int i );

void lb_window_add_input_device( void* window, void* device );

#ifdef __cplusplus
}
#endif
#endif

