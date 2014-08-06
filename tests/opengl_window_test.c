
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
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
	void *window, *context;
	void *window2, *context2;
	int running;
	double time;

	window = lb_window_construct();
	assert( lb_window_constructed( window ) == 1 );

	lb_window_set_width( window, 201 );
	lb_window_set_height( window, 202 );
	lb_window_set_x( window, 101 );
	lb_window_set_y( window, 102 );
	lb_window_set_caption( window, "No name" );

	context = lb_opengl_context_construct( window, LBOpenglContextCreateNew );
	assert( lb_opengl_context_constructed( context ) == 1 );

	lb_opengl_context_bind( context, LBOpenglContextDefaultPixelformat );

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


	window2 = lb_window_construct();
	assert( lb_window_constructed( window2 ) == 1 );

	context2 = lb_opengl_context_construct( window2, LBOpenglContextCreateNew );
	assert( lb_opengl_context_constructed( context2 ) == 1 );

	lb_opengl_context_bind( context2, LBOpenglContextDefaultPixelformat );
	
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
		while (lb_window_constructed( window ) == 1
		    && lb_window_next_event( window ) == 1)
		{
			int type = lb_window_get_event_type( window );
			if (type == LBWindowEventClose)
			{
				context = lb_opengl_context_destruct( context );
				window = lb_window_destruct( window );
			}
		}

		while (lb_window_constructed( window2 ) == 1
		    && lb_window_next_event( window2 ) == 1)
		{
			int type = lb_window_get_event_type( window2 );
			if (type == LBWindowEventClose)
			{
				context2 = lb_opengl_context_destruct( context2 );
				window2 = lb_window_destruct( window2 );
			}
		}

		if (lb_window_constructed( window ) == 0
		 && lb_window_constructed( window2 ) == 0)
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
}

