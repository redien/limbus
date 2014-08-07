
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include <limbus/window.h>
#include <limbus/keyboard.h>
#include <limbus/mouse.h>
#include <limbus/opengl_context.h>
#include <limbus/opengl.h>

#include <stdio.h>

int main( int argc, char** argv )
{
	LBWindow window;
	LBOpenglContext context;
	LBKeyboard keyboard;
	LBMouse mouse;
	int running;

	window = lb_window_construct();
	if (lb_window_constructed( window ) == 0)
		return -1;

	lb_window_set_width( window, 800 );
	lb_window_set_height( window, 600 );
	lb_window_set_caption( window, "Window name" );

	context = lb_opengl_context_construct( window, LBOpenglContextCreateNew );
	if (lb_opengl_context_constructed( context ) == 0)
	{
		lb_window_destruct( window );
		return -1;
	}

	lb_opengl_context_bind( context, LBOpenglContextDefaultPixelformat );
	lb_opengl_context_make_current( context );

	keyboard = lb_keyboard_construct();
	if (lb_keyboard_constructed( keyboard ) == 0)
	{
		lb_opengl_context_destruct( context );
		lb_window_destruct( window );
	    return -1;
	}
	lb_window_add_input_device( window, keyboard );

	mouse = lb_mouse_construct();
	if (lb_mouse_constructed( mouse ) == 0)
	{
	    lb_keyboard_destruct( keyboard );
		lb_opengl_context_destruct( context );
		lb_window_destruct( window );
	    return -1;
	}
	lb_window_add_input_device( window, mouse );

	glClearColor( 0.0f, 0.0f, 1.0f, 1.0f );

	running = 1;
	while (running)
	{
		while (lb_window_next_event( window ) == 1)
		{
			int type = lb_window_get_event_type( window );
			if (type == LBWindowEventClose)
				running = 0;
		}

		while (lb_keyboard_next_event( keyboard ) == 1)
		{
			int type = lb_keyboard_get_event_type( keyboard );
			int key = lb_keyboard_get_event_key( keyboard );
			if (type == LBKeyboardEventKeyPress && key == LBKeyEscape)
				running = 0;
		}

		while (lb_mouse_next_event( mouse ) == 1)
		{
			int type = lb_mouse_get_event_type( mouse );
			int x = lb_mouse_get_event_x( mouse );
			int y = lb_mouse_get_event_y( mouse );
			if (type == LBMouseEventMotion)
				printf( "%d, %d\n", x, y );
		}

		glClear( GL_COLOR_BUFFER_BIT );
		
		/* Render OpenGL primitives here! */

		lb_opengl_context_swap_buffers( context );
	}

    lb_mouse_destruct( mouse );
    lb_keyboard_destruct( keyboard );
	lb_opengl_context_destruct( context );
	lb_window_destruct( window );
	return 0;
}

