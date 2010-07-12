
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef LB_JOYSTICK_H
#define LB_JOYSTICK_H
#ifdef __cplusplus
extern "C"
{
#endif

/* Experimental */

typedef void* LBJoystick;

enum LBJoystickEvent
{
	LBJoystickEventAxis,
	LBJoystickEventButton,
	LBJoystickEventConnected,
	LBJoystickEventDisconnected
};
typedef enum LBJoystickEvent LBJoystickEvent;

LBJoystick lb_joystick_construct( int joystick_id );
void lb_joystick_destruct( LBJoystick joystick );
int lb_joystick_constructed( LBJoystick joystick );
int lb_joystick_connected( LBJoystick joystick );

char* lb_joystick_get_product_string( LBJoystick joystick );

int lb_joystick_axes( LBJoystick joystick );
int lb_joystick_buttons( LBJoystick joystick );

void lb_joystick_set_axis_max( LBJoystick joystick, int axis, int value );
void lb_joystick_set_axis_min( LBJoystick joystick, int axis, int value );

int lb_joystick_next_event( LBJoystick joystick );
int lb_joystick_next_raw_event( LBJoystick joystick );
LBJoystickEvent lb_joystick_get_event_type( LBJoystick joystick );
int lb_joystick_get_event_number( LBJoystick joystick );
int lb_joystick_get_event_value( LBJoystick joystick );
int lb_joystick_get_event_time( LBJoystick joystick );

#ifdef __cplusplus
}
#endif
#endif

