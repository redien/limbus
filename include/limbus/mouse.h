
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef _MOUSE_H_
#define _MOUSE_H_
#ifdef __cplusplus
extern "C"
{
#endif

enum LBMouseEvent
{
	LBMouseEventMotion,
	LBMouseEventButtonPress,
	LBMouseEventButtonRelease
};

void* lb_mouse_construct( void );
void lb_mouse_destruct( void* mouse );
int lb_mouse_constructed( void* mouse );

void lb_mouse_set_x( void* mouse, int x );
void lb_mouse_set_y( void* mouse, int y );
int lb_mouse_get_x( void* mouse );
int lb_mouse_get_y( void* mouse );

void lb_mouse_cursor_show( void* mouse, int state );

int lb_mouse_next_event( void* mouse );
enum LBMouseEvent lb_mouse_get_event_type( void* mouse );
int lb_mouse_get_event_button( void* mouse );
int lb_mouse_get_event_x( void* mouse );
int lb_mouse_get_event_y( void* mouse );

#ifdef __cplusplus
}
#endif
#endif

