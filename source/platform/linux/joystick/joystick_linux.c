
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include <limbus/joystick.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/joystick.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_AXES 128

struct Joystick
{
	int fd;
	char name[128];
	int number_of_axes, number_of_buttons;

	int axes_max[MAX_AXES], axes_min[MAX_AXES];

	struct js_event event;
};

static int open_joystick_device( struct Joystick* joystick,
								 int joystick_id );
static void close_joystick_device( struct Joystick* joystick );
static void read_joystick_info( struct Joystick* joystick );
static void initialize_calibration( struct Joystick* joystick );

void* lb_joystick_construct( int joystick_id )
{
	struct Joystick* joystick;

	joystick = (struct Joystick*)malloc( sizeof( struct Joystick ) );
	assert( joystick != NULL );

	if (!open_joystick_device( joystick, joystick_id ))
		return 0;

	read_joystick_info( joystick );

	initialize_calibration( joystick );

	/* Ignore initial events */
	while (lb_joystick_next_raw_event( joystick )) {}

	return joystick;
}

#define DECLARE_JOYSTICK()\
	struct Joystick* joystick;\
	joystick = (struct Joystick*) j;\
	assert( joystick );

void lb_joystick_destruct( void* j )
{
	DECLARE_JOYSTICK()

	close_joystick_device( joystick );

	free( joystick );
}

int lb_joystick_constructed( void* j )
{
	return (j == NULL) ? 0 : 1;
}

int lb_joystick_connected( void* j )
{
	return (j == NULL) ? 0 : 1;
}

char* lb_joystick_get_product_string( void* j )
{
	DECLARE_JOYSTICK()
	return joystick->name;
}

int lb_joystick_axes( void* j )
{
	DECLARE_JOYSTICK()
	return joystick->number_of_axes;
}

int lb_joystick_buttons( void* j )
{
	DECLARE_JOYSTICK()
	return joystick->number_of_buttons;
}

void lb_joystick_set_axis_max( void* j, int axis, int value )
{
	DECLARE_JOYSTICK()
	assert( axis < MAX_AXES );
	joystick->axes_max[axis] = value;
}

void lb_joystick_set_axis_min( void* j, int axis, int value )
{
	DECLARE_JOYSTICK()
	assert( axis < MAX_AXES );
	joystick->axes_min[axis] = value;
}

enum LBJoystickEvent lb_joystick_get_event_type( void* j )
{
	DECLARE_JOYSTICK()
	
	joystick->event.type &= ~JS_EVENT_INIT;
	
	if (joystick->event.type == JS_EVENT_BUTTON)
		return LBJoystickEventButton;
	
	else if (joystick->event.type == JS_EVENT_AXIS)
		return LBJoystickEventAxis;

	assert( 0 && "Unknown joystick event type" );
	return -1;
}

int lb_joystick_get_event_number( void* j )
{
	DECLARE_JOYSTICK()
	return joystick->event.number;
}

int lb_joystick_get_event_value( void* j )
{
	DECLARE_JOYSTICK()
	return joystick->event.value;
}

int lb_joystick_get_event_time( void* j )
{
	DECLARE_JOYSTICK()
	return joystick->event.time;
}

char* lb_joystick_get_impl_string()
{
	return "linux";
}

int lb_joystick_next_event( void* j )
{
	DECLARE_JOYSTICK()

	if (lb_joystick_next_raw_event( joystick ))
	{
		struct js_event* event = &joystick->event;

		if (event->type == JS_EVENT_AXIS)
		{
			int range;
			assert( event->number < MAX_AXES );

			if (event->value < joystick->axes_min[event->number])
				joystick->axes_min[event->number] = event->value;

			if (event->value > joystick->axes_max[event->number])
				joystick->axes_max[event->number] = event->value;
		
			range = joystick->axes_max[event->number] -
					joystick->axes_min[event->number];

			event->value = event->value -
						  (joystick->axes_min[event->number] + (range / 2));
		
			event->value *= (double)1000 / (range / 2);
		}
		
		return 1;
	}
	else
	{
		return 0;
	}
}

int lb_joystick_next_raw_event( void* j )
{
	DECLARE_JOYSTICK()

	if (read( joystick->fd,
			  &joystick->event,
			  sizeof(struct js_event) ) > 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int open_joystick_device( struct Joystick* joystick, int joystick_id )
{
	char device[256];
	/* TODO: Fix possible buffer overrun */
	sprintf( device, "/dev/input/js%d", joystick_id );
	
	joystick->fd = open( device, O_RDONLY | O_NONBLOCK );
	if (joystick->fd < 0)
		return 0;

	return 1;
}

void close_joystick_device( struct Joystick* joystick )
{
	close( joystick->fd );
}

void read_joystick_info( struct Joystick* joystick )
{
	joystick->number_of_axes = 0;
	ioctl( joystick->fd, JSIOCGAXES, &joystick->number_of_axes );
	joystick->number_of_buttons = 0;
	ioctl( joystick->fd, JSIOCGBUTTONS, &joystick->number_of_buttons );

	strcpy( joystick->name, "Unknown" );
	ioctl( joystick->fd,
		   JSIOCGNAME( sizeof( joystick->name ) ),
		   joystick->name );
}

void initialize_calibration( struct Joystick* joystick )
{
	int i;
	for (i = 0; i < MAX_AXES; i++)
	{
		/* These initial values works when the joystick is properly
		   calibrated */
		joystick->axes_max[i] = 32767;
		joystick->axes_min[i] = -32767;
	}
}

