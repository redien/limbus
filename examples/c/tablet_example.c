
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include <limbus/screen.h>
#include <limbus/window.h>
#include <limbus/opengl_context.h>
#include <limbus/tablet.h>

#include <stdio.h>

#ifdef _WIN32
	#include <windows.h>
#endif

#include <GL/gl.h>

int main( int argc, char** argv )
{
	void *screen, *window, *context, *tablet;
	int running;

	screen = lb_screen_construct( LBScreenDefault );
	if (!lb_screen_constructed( screen ))
		return -1;

	window = lb_window_construct( screen );
	if (!lb_window_constructed( window ))
	{
		lb_screen_destruct( screen );
		return -1;
	}

	context = lb_opengl_context_construct_in_window( window,
	                                                 LBOpenglContextCreateNew );
	if (!lb_opengl_context_constructed( context ))
	{
		lb_window_destruct( window );
		lb_screen_destruct( screen );
		return -1;
	}

	lb_opengl_context_set_pixelformat( context, LBOpenglDefaultPixelformat );
	lb_opengl_context_make_current( context );

	tablet = lb_tablet_construct();
	lb_window_add_input_device( window, tablet );

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

	running = 1;
	while (running)
	{
		while (lb_window_next_event( window ))
		{
			int type = lb_window_get_event_type( window );
			if (type == LBWindowEventClose)
				running = 0;
		}

		while (lb_tablet_next_event( tablet ))
		{
			int type = lb_tablet_get_event_type( tablet );

			if (type == LBTabletEventProximityIn)
				printf( "Proximity in\n" );

			if (type == LBTabletEventProximityOut)
				printf( "Proximity out\n" );

			if (type == LBTabletEventMotion)
			{
				int x = lb_tablet_get_event_x( tablet );
				int y = lb_tablet_get_event_y( tablet );
				int pressure = lb_tablet_get_event_pressure( tablet );

				printf( "%d, %d, %d\n", x, y, pressure );
			}
		}

		glClear( GL_COLOR_BUFFER_BIT );
		lb_opengl_context_swap_buffers( context );
	}

	lb_tablet_destruct( tablet );
	lb_opengl_context_destruct( context );
	lb_window_destruct( window );
	lb_screen_destruct( screen );
	
	return 0;
}

