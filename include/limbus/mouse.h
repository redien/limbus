
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MOUSE_H
#define MOUSE_H
#ifdef __cplusplus
extern "C"
{
#endif

typedef void* LBMouse;

enum LBMouseEvent
{
	LBMouseEventMotion,
	LBMouseEventButtonPress,
	LBMouseEventButtonRelease
};
typedef enum LBMouseEvent LBMouseEvent;

LBMouse lb_mouse_construct( void );
void lb_mouse_destruct( LBMouse mouse );
int lb_mouse_constructed( LBMouse mouse );

void lb_mouse_set_x( LBMouse mouse, int x );
void lb_mouse_set_y( LBMouse mouse, int y );
int lb_mouse_get_x( LBMouse mouse );
int lb_mouse_get_y( LBMouse mouse );

void lb_mouse_cursor_show( LBMouse mouse, int state );

int lb_mouse_next_event( LBMouse mouse );
LBMouseEvent lb_mouse_get_event_type( LBMouse mouse );
int lb_mouse_get_event_button( LBMouse mouse );
int lb_mouse_get_event_x( LBMouse mouse );
int lb_mouse_get_event_y( LBMouse mouse );

#ifdef __cplusplus
}
#endif
#endif

