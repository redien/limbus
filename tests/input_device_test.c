
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include <limbus/screen.h>
#include <limbus/window.h>
#include <limbus/opengl_context.h>
#include <limbus/keyboard.h>
#include <limbus/mouse.h>
#include <limbus/tablet.h>

#include <stdio.h>
#include <assert.h>
#include <string.h>

#ifdef _WIN32
	#include <windows.h>
#endif

#include <GL/gl.h>

int main( int argc, char** argv )
{
	void *screen, *window, *context, *keyboard, *mouse, *tablet;
	int running;

	screen = lb_screen_construct( LBScreenDefault );
	assert( lb_screen_constructed( screen ) == 1 );

	window = lb_window_construct( screen );
	assert( lb_window_constructed( window ) == 1 );

	lb_window_set_width( window, 800 );
	lb_window_set_height( window, 600 );
	lb_window_set_caption( window, "Window1" );

	context = lb_opengl_context_construct_in_window( window, LBOpenglContextCreateNew );
	assert( lb_opengl_context_constructed( context ) == 1 );

	lb_opengl_context_set_pixelformat( context, LBOpenglDefaultPixelformat );

	keyboard = lb_keyboard_construct();
	if (lb_keyboard_constructed( keyboard ) == 1)
		lb_window_add_input_device( window, keyboard );

	mouse = lb_mouse_construct();
	if (lb_mouse_constructed( mouse ) == 1)
		lb_window_add_input_device( window, mouse );

	tablet = lb_tablet_construct();
	if (lb_tablet_constructed( tablet ) == 1)
		lb_window_add_input_device( window, tablet );

	lb_opengl_context_make_current( context );
	glClearColor( 0.390625f, 0.58203125f, 0.92578125f, 1.0f );

	running = 1;
	while (running)
	{
		while (lb_window_next_event( window ) == 1)
		{
			int type = lb_window_get_event_type( window );
			if (type == LBWindowEventClose)
				running = 0;
		}

		while (klb_eyboard_constructed( keyboard ) == 1 && lb_keyboard_next_event( keyboard ) == 1)
		{
			int type = lb_keyboard_get_event_type( keyboard );
			if (type == LBKeyboardEventKeyPress)
			{
				int key = lb_keyboard_get_event_key( keyboard );
				printf( "KeyboardEventKeyPress: %d\n", key );

				if (key == KeyEscape)
					running = 0;
			}
			else if (type == LBKeyboardEventKeyRelease)
			{
				int key = lb_keyboard_get_event_key( keyboard );
				printf( "KeyboardEventKeyRelease: %d\n", key );
			}
			else if (type == LBKeyboardEventUnicode)
			{
				int code_point = lb_keyboard_get_event_unicode( keyboard );
				printf( "KeyboardEventUnicode: %d\n", code_point );
			}
		}

		while (lb_mouse_constructed( mouse ) == 1 && lb_mouse_next_event( mouse ) == 1)
		{
			int type = lb_mouse_get_event_type( mouse );
			if (type == LBMouseEventMotion)
			{
				int x = lb_mouse_get_event_x( mouse );
				int y = lb_mouse_get_event_y( mouse );
				printf( "MouseEventMotion: %d, %d\n", x, y );
			}
			else if (type == LBMouseEventButtonPress)
			{
				int button = lb_mouse_get_event_button( mouse );
				int x = lb_mouse_get_event_x( mouse );
				int y = lb_mouse_get_event_y( mouse );
				printf( "MouseEventButtonPress: button:%d, x:%d, y:%d\n", button, x, y );
			}
			else if (type == LBMouseEventButtonRelease)
			{
				int button = lb_mouse_get_event_button( mouse );
				int x = lb_mouse_get_event_x( mouse );
				int y = lb_mouse_get_event_y( mouse );
				printf( "MouseEventButtonRelease: button:%d, x:%d, y:%d\n", button, x, y );
			}
		}

		while (lb_tablet_constructed( tablet ) == 1 && lb_tablet_next_event( tablet ) == 1)
		{
			int type = lb_tablet_get_event_type( tablet );
			if (type == LBTabletEventMotion)
			{
				int x = lb_tablet_get_event_x( tablet );
				int y = lb_tablet_get_event_y( tablet );
				printf( "TabletEventMotion: x:%d, y:%d\n", x, y );
			}
			else if (type == LBTabletEventButtonPress)
			{
				int button = lb_tablet_get_event_button( tablet );
				printf( "TabletEventButtonPress: button:%d\n", button );
			}
			else if (type == LBTabletEventButtonRelease)
			{
				int button = lb_tablet_get_event_button( tablet );
				printf( "TabletEventButtonRelease: button:%d\n", button );
			}
			else if (type == LBTabletEventProximityIn)
			{
				printf( "TabletEventProximityIn\n" );
			}
			else if (type == LBTabletEventProximityOut)
			{
				printf( "TabletEventProximityOut\n" );
			}
		}

		glClear( GL_COLOR_BUFFER_BIT );
		lb_opengl_context_swap_buffers( context );
	}

	if (lb_tablet_constructed( tablet ) == 1)
		lb_tablet_destruct( tablet );
	if (lb_keyboard_constructed( keyboard ) == 1)
		lb_keyboard_destruct( keyboard );
	if (lb_mouse_constructed( mouse ) == 1)
		lb_mouse_destruct( mouse );
	
	context = lb_opengl_context_destruct( context );
	assert( lb_opengl_context_constructed( context ) == 0 );

	window = lb_window_destruct( window );
	assert( lb_window_constructed( window ) == 0 );

	screen = lb_screen_destruct( screen );
	assert( lb_screen_constructed( screen ) == 0 );
}
