
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include <platform/screen.h>
#include <platform/window.h>
#include <platform/keyboard.h>
#include <platform/mouse.h>
#include <platform/opengl_context.h>

#include <stdio.h>

#ifdef _WIN32
	#include <windows.h>
#endif

#include <GL/gl.h>

int main( int argc, char** argv )
{
	void *screen, *window, *context, *keyboard, *mouse;
	int running;

	screen = screen_construct( 0 );
	if (!screen_constructed( screen ))
		return 0;

	window = window_construct( screen );
	if (!window_constructed( window ))
	{
		screen_destruct( screen );
		return 0;
	}

	window_set_width( window, 800 );
	window_set_height( window, 600 );
	window_set_caption( window, "Window name" );

	context = opengl_context_construct_in_window( window );
	if (!opengl_context_constructed( context ))
	{
		window_destruct( window );
		screen_destruct( screen );
		return 0;
	}

	opengl_context_set_pixelformat( context, 0 );
	opengl_context_make_current( context );

	keyboard = keyboard_construct();
	window_add_input_device( window, keyboard );

	mouse = mouse_construct();
	window_add_input_device( window, mouse );

	glClearColor( 0.0f, 0.0f, 1.0f, 1.0f );

	running = 1;
	while (running)
	{
		while (window_next_event( window ))
		{
			int type = window_get_event_type( window );
			if (type == WindowEventClose)
				running = 0;
		}

		while (keyboard_next_event( keyboard ))
		{
			int type = keyboard_get_event_type( keyboard );
			int key = keyboard_get_event_key( keyboard );
			if (type == KeyboardEventKeyPress && key == KeyEscape)
				running = 0;
		}

		while (mouse_next_event( mouse ))
		{
			int type = mouse_get_event_type( mouse );
			int x = mouse_get_event_x( mouse );
			int y = mouse_get_event_y( mouse );
			if (type == MouseEventMotion)
				printf( "%d, %d\n", x, y );
		}

		glClear( GL_COLOR_BUFFER_BIT );
		opengl_context_swap_buffers( context );
	}

	keyboard_destruct( keyboard );
	opengl_context_destruct( context );
	window_destruct( window );
	screen_destruct( screen );
}

