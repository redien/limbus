
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include <platform/screen.h>
#include <platform/window.h>
#include <platform/opengl_context.h>
#include <platform/tablet.h>

#include <stdio.h>

#ifdef _WIN32
	#include <windows.h>
#endif

#include <GL/gl.h>

int main( int argc, char** argv )
{
	void *screen, *window, *context, *tablet;
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

	context = opengl_context_construct_in_window( window );
	if (!opengl_context_constructed( context ))
	{
		window_destruct( window );
		screen_destruct( screen );
		return 0;
	}

	opengl_context_set_pixelformat( context, 0 );
	opengl_context_make_current( context );

	tablet = tablet_construct();
	window_add_input_device( window, tablet );

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

	running = 1;
	while (running)
	{
		while (window_next_event( window ))
		{
			int type = window_get_event_type( window );
			if (type == WindowEventClose)
				running = 0;
		}

		while (tablet_next_event( tablet ))
		{
			int type = tablet_get_event_type( tablet );
			
			if (type == TabletEventProximityIn)
				printf( "Proximity in\n" );
			
			if (type == TabletEventProximityOut)
				printf( "Proximity out\n" );
			
			if (type == TabletEventMotion)
			{
				int x = tablet_get_event_x( tablet );
				int y = tablet_get_event_y( tablet );
				int pressure = tablet_get_event_pressure( tablet );
				
				printf( "%d, %d, %d\n", x, y, pressure );
			}
		}

		glClear( GL_COLOR_BUFFER_BIT );
		opengl_context_swap_buffers( context );
	}

	tablet_destruct( tablet );
	opengl_context_destruct( context );
	window_destruct( window );
	screen_destruct( screen );
}

