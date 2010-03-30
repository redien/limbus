
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

enum LBJoystickEvent
{
	LBJoystickEventAxis,
	LBJoystickEventButton,
	LBJoystickEventConnected,
	LBJoystickEventDisconnected
};

void* lb_joystick_construct( int joystick_id );
void lb_joystick_destruct( void* joystick );
int lb_joystick_constructed( void* joystick );
int lb_joystick_connected( void* joystick );

char* lb_joystick_get_product_string( void* joystick );

int lb_joystick_axes( void* joystick );
int lb_joystick_buttons( void* joystick );

void lb_joystick_set_axis_max( void* joystick, int axis, int value );
void lb_joystick_set_axis_min( void* joystick, int axis, int value );

int lb_joystick_next_event( void* joystick );
int lb_joystick_next_raw_event( void* joystick );
enum LBJoystickEvent lb_joystick_get_event_type( void* joystick );
int lb_joystick_get_event_number( void* joystick );
int lb_joystick_get_event_value( void* joystick );
int lb_joystick_get_event_time( void* joystick );

#ifdef __cplusplus
}
#endif
#endif
