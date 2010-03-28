
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include <limbus/screen.h>
#include <limbus/window.h>
#include <limbus/opengl_context.h>

int main()
{
    void *screen, *window, *context;

    screen = lb_screen_construct( LBScreenDefault );

    window = lb_window_construct( screen );
    lb_window_set_width( window, 100 );
    lb_window_set_height( window, 100 );
    lb_window_set_caption( window, "Window test" );

    context = lb_opengl_context_construct_in_window( window );
    lb_opengl_context_set_pixelformat( context, LBOpenglContextDefaultPixelformat );
    lb_opengl_context_make_current( context );

    int running = 1;
    while (running)
    {
        if (lb_window_next_event( window ))
        {
            int type = lb_window_get_event_type( window );
            if (type == LBWindowEventClose) {running = 0;}
        }
        
        lb_opengl_context_swap_buffers( context );
    }

    lb_opengl_context_destruct( context );
    lb_window_destruct( window );
    lb_screen_destruct( screen );
    return 0;
}

