
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include <limbus/joystick.h>

#include <windows.h>
#include <assert.h>

typedef struct
{
	int type,
		number,
		value;
} JoystickEvent;

typedef struct
{
	int joystick_num;
	JOYINFOEX info;
	JOYCAPS caps;
	JoystickEvent event;
	int connected;
} Joystick;

void* lb_joystick_construct( int joystick_num )
{
	MMRESULT result;
	Joystick* joystick;

	joystick = (Joystick*)malloc( sizeof( Joystick ) );
	if (!joystick)
		return 0;

	joystick->joystick_num = joystick_num;

	joystick->info.dwSize = sizeof( JOYINFOEX );
	joystick->info.dwFlags = JOY_RETURNALL;
	result = joyGetPosEx( joystick_num, &joystick->info );
	if (result != JOYERR_NOERROR)
	{
		joystick->connected = 0;
	}
	else
	{
		joystick->connected = 1;
		joyGetDevCaps( joystick_num, &joystick->caps, sizeof( JOYCAPS ) );
	}
	
	return joystick;
}

void lb_joystick_destruct( void* joystick )
{
	assert( joystick );
	free( joystick );
}

int lb_joystick_constructed( void* joystick )
{
	return (joystick != NULL) ? 1 : 0;
}

int lb_joystick_connected( void* joystick )
{
	return ((Joystick*)joystick)->connected;
}

char* lb_joystick_get_product_string( void* joystick )
{
	assert( joystick );
	return ((Joystick*)joystick)->caps.szPname;
}

void lb_joystick_set_axis_max( void* joystick, int axis, int value )
{
}

void lb_joystick_set_axis_min( void* joystick, int axis, int value )
{
}

int lb_joystick_axes( void* joystick )
{
	assert( joystick );
	return ((Joystick*)joystick)->caps.wNumAxes;
}

int lb_joystick_buttons( void* joystick )
{
	assert( joystick );
	return ((Joystick*)joystick)->caps.wNumButtons;
}

int lb_joystick_next_event( void* js )
{
	return lb_joystick_next_raw_event( js );
}

int lb_joystick_next_raw_event( void* js )
{
	JOYINFOEX info;
	MMRESULT result;
	Joystick* joystick;
	int i;

	joystick = (Joystick*)js;
	assert( joystick );
	
	info.dwSize = sizeof( JOYINFOEX );
	info.dwFlags = JOY_RETURNALL;
	result = joyGetPosEx( joystick->joystick_num, &info );
	if (result == JOYERR_NOERROR && !joystick->connected)
	{
		joyGetDevCaps( joystick->joystick_num, &joystick->caps, sizeof( JOYCAPS ) );
		joystick->connected = 1;
		joystick->event.type = LBJoystickEventConnected;
		return 1;
	}

	if (result != JOYERR_NOERROR && joystick->connected)
	{
		joystick->connected = 0;
		joystick->event.type = LBJoystickEventDisconnected;
		return 1;
	}

	if (result != JOYERR_NOERROR && !joystick->connected)
	{
		return 0;
	}

#define READ_AXIS_EVENT( new_value, old_value, axis_num )\
	if (new_value != old_value)\
	{\
		joystick->event.type = LBJoystickEventAxis;\
		joystick->event.number = axis_num;\
		joystick->event.value = new_value;\
		old_value = new_value;\
		return 1;\
	}

	READ_AXIS_EVENT( info.dwXpos, joystick->info.dwXpos, 0 )
	READ_AXIS_EVENT( info.dwYpos, joystick->info.dwYpos, 1 )
	READ_AXIS_EVENT( info.dwZpos, joystick->info.dwZpos, 2 )
	READ_AXIS_EVENT( info.dwRpos, joystick->info.dwRpos, 3 )
	READ_AXIS_EVENT( info.dwUpos, joystick->info.dwUpos, 4 )
	READ_AXIS_EVENT( info.dwVpos, joystick->info.dwVpos, 5 )
	READ_AXIS_EVENT( info.dwPOV, joystick->info.dwPOV, 6 )

	for (i = 0; i < 32; i++)
	{
		DWORD mask = (1 << i);
		if ((info.dwButtons & mask) != (joystick->info.dwButtons & mask))
		{
			joystick->event.type = LBJoystickEventButton;
			joystick->event.number = i;
			if (info.dwButtons & mask)
				joystick->event.value = 1;
			else
				joystick->event.value = 0;
			joystick->info.dwButtons ^= mask;
			return 1;
		}
	}

	return 0;
}

enum LBJoystickEvent lb_joystick_get_event_type( void* joystick )
{
	assert( joystick );
	return ((Joystick*)joystick)->event.type;
}

int lb_joystick_get_event_number( void* joystick )
{
	assert( joystick );
	return ((Joystick*)joystick)->event.number;
}

int lb_joystick_get_event_value( void* joystick )
{
	assert( joystick );
	return ((Joystick*)joystick)->event.value;
}

int lb_joystick_get_event_time( void* joystick )
{
	return timeGetTime();
}
