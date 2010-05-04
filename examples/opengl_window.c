
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include <limbus/screen.h>
#include <limbus/window.h>
#include <limbus/keyboard.h>
#include <limbus/mouse.h>
#include <limbus/opengl_context.h>

#include <stdio.h>

#ifdef _WIN32
	#include <windows.h>
#endif

#include <GL/gl.h>

void *screen = NULL,
     *window = NULL,
     *context = NULL,
     *keyboard = NULL,
     *mouse = NULL;

void cleanup( void )
{
    if (lb_mouse_constructed( mouse ))
        lb_mouse_destruct( mouse );

    if (lb_keyboard_constructed( keyboard ))
        lb_keyboard_destruct( keyboard );

    if (lb_opengl_context_constructed( context ))
    	lb_opengl_context_destruct( context );

    if (lb_window_constructed( window ))
		lb_window_destruct( window );

    if (lb_screen_constructed( screen ))
		lb_screen_destruct( screen );
}

int main( int argc, char** argv )
{
	int running;

	screen = lb_screen_construct( LBScreenDefault );
	if (!lb_screen_constructed( screen ))
		return -1;

	window = lb_window_construct( screen );
	if (!lb_window_constructed( window ))
	{
	    cleanup();
		return -1;
	}

	lb_window_set_width( window, 800 );
	lb_window_set_height( window, 600 );
	lb_window_set_caption( window, "Window name" );

	context = lb_opengl_context_construct_in_window( window, LBOpenglContextCreateNew );
	if (!lb_opengl_context_constructed( context ))
	{
		cleanup();
		return -1;
	}

	lb_opengl_context_set_pixelformat( context, LBOpenglContextDefaultPixelformat );
	lb_opengl_context_make_current( context );

	keyboard = lb_keyboard_construct();
	if (!lb_keyboard_constructed( keyboard ))
	{
	    cleanup();
	    return -1;
	}
	lb_window_add_input_device( window, keyboard );

	mouse = lb_mouse_construct();
	if (!lb_mouse_constructed( mouse ))
	{
	    cleanup();
	    return -1;
	}
	lb_window_add_input_device( window, mouse );

	glClearColor( 0.0f, 0.0f, 1.0f, 1.0f );

	running = 1;
	while (running)
	{
		while (lb_window_next_event( window ))
		{
			int type = lb_window_get_event_type( window );
			if (type == LBWindowEventClose)
				running = 0;
		}

		while (lb_keyboard_next_event( keyboard ))
		{
			int type = lb_keyboard_get_event_type( keyboard );
			int key = lb_keyboard_get_event_key( keyboard );
			if (type == LBKeyboardEventKeyPress && key == LBKeyEscape)
				running = 0;
		}

		while (lb_mouse_next_event( mouse ))
		{
			int type = lb_mouse_get_event_type( mouse );
			int x = lb_mouse_get_event_x( mouse );
			int y = lb_mouse_get_event_y( mouse );
			if (type == LBMouseEventMotion)
				printf( "%d, %d\n", x, y );
		}

		glClear( GL_COLOR_BUFFER_BIT );
		lb_opengl_context_swap_buffers( context );
	}

	cleanup();
	return 0;
}

