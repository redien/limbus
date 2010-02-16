
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include <limbus/timing.h>
#include <limbus/screen.h>
#include <limbus/window.h>
#include <limbus/opengl_context.h>

#include <stdio.h>
#include <assert.h>
#include <string.h>

#ifdef _WIN32
	#include <windows.h>
#endif

#include <GL/gl.h>

int main( int argc, char** argv )
{
	void *screen, *window, *context;
	void *window2, *context2;
	int running;
	double time;

	screen = lb_screen_construct( LBScreenDefault );
	assert( lb_screen_constructed( screen ) == 1 );


	window = lb_window_construct( screen );
	assert( lb_window_constructed( window ) == 1 );

	lb_window_set_width( window, 201 );
	lb_window_set_height( window, 202 );
	lb_window_set_x( window, 101 );
	lb_window_set_y( window, 102 );
	lb_window_set_caption( window, "No name" );

	context = lb_opengl_context_construct_in_window( window, LBOpenglContextCreateNew );
	assert( lb_opengl_context_constructed( context ) == 1 );

	lb_opengl_context_set_pixelformat( context, LBOpenglDefaultPixelformat );

	assert( lb_window_get_width( window ) == 201 );
	assert( lb_window_get_height( window ) == 202 );
	assert( lb_window_get_x( window ) == 101 );
	assert( lb_window_get_y( window ) == 102 );
	assert( strcmp( lb_window_get_caption( window ), "No name" ) == 0 );

	lb_window_set_width( window, 800 );
	lb_window_set_height( window, 600 );
	lb_window_set_x( window, 130 );
	lb_window_set_y( window, 120 );
	lb_window_set_caption( window, "Window1" );

	assert( lb_window_get_width( window ) == 800 );
	assert( lb_window_get_height( window ) == 600 );

	assert( lb_window_get_x( window ) == 130 );
	assert( lb_window_get_y( window ) == 120 );
	assert( strcmp( lb_window_get_caption( window ), "Window1" ) == 0 );


	window2 = lb_window_construct( screen );
	assert( lb_window_constructed( window2 ) == 1 );

	context2 = lb_opengl_context_construct_in_window( window2, LBOpenglContextCreateNew );
	assert( lb_opengl_context_constructed( context2 ) == 1 );

	lb_opengl_context_set_pixelformat( context2, LBOpenglDefaultPixelformat );
	
	assert( lb_window_get_width( window2 ) == 640 );
	assert( lb_window_get_height( window2 ) == 480 );

	lb_window_set_width( window2, 800 );
	lb_window_set_height( window2, 600 );
	assert( lb_window_get_width( window2 ) == 800 );
	assert( lb_window_get_height( window2 ) == 600 );

	time = lb_time_count_in_seconds();
	running = 1;
	while (running)
	{
		while (lb_window_constructed( window ) == 1 && lb_window_next_event( window ) == 1)
		{
			int type = lb_window_get_event_type( window );
			if (type == LBWindowEventClose)
			{
				context = lb_opengl_context_destruct( context );
				window = lb_window_destruct( window );
			}
		}

		while (lb_window_constructed( window2 ) == 1 && lb_window_next_event( window2 ) == 1)
		{
			int type = lb_window_get_event_type( window2 );
			if (type == LBWindowEventClose)
			{
				context2 = lb_opengl_context_destruct( context2 );
				window2 = lb_window_destruct( window2 );
			}
		}

		if (lb_window_constructed( window ) == 0 && lb_window_constructed( window2 ) == 0)
			running = 0;

		if (lb_opengl_context_constructed( context ) == 1)
		{
			lb_opengl_context_make_current( context );
			glClearColor( 0.390625f, 0.58203125f, 0.92578125f, 1.0f );
			glClear( GL_COLOR_BUFFER_BIT );
			lb_opengl_context_swap_buffers( context );
		}
		if (lb_opengl_context_constructed( context2 ) == 1)
		{
			lb_opengl_context_make_current( context2 );
			glClearColor( 0.390625f, 0.58203125f, 0.92578125f, 1.0f );
			glClear( GL_COLOR_BUFFER_BIT );
			lb_opengl_context_swap_buffers( context2 );
		}
		
		if (lb_time_count_in_seconds() > time + 1.0)
			running = 0;
	}

	if (lb_opengl_context_constructed( context ) == 1)
		context = lb_opengl_context_destruct( context );
	assert( lb_opengl_context_constructed( context ) == 0 );

	if (lb_window_constructed( window ) == 1)
		window = lb_window_destruct( window );
	assert( lb_window_constructed( window ) == 0 );

	if (lb_opengl_context_constructed( context2 ) == 1)
		context2 = lb_opengl_context_destruct( context2 );
	assert( lb_opengl_context_constructed( context2 ) == 0 );

	if (lb_window_constructed( window2 ) == 1)
		window2 = lb_window_destruct( window2 );
	assert( lb_window_constructed( window2 ) == 0 );

	screen = lb_screen_destruct( screen );
	assert( lb_screen_constructed( screen ) == 0 );
}
