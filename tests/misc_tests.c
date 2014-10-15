
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include <limbus/display.h>
#include <limbus/window.h>
#include <limbus/opengl_context.h>

int main()
{
    void *screen, *window, *context;

    screen = lb_display_construct( LBDisplayDefault );

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
    lb_display_destruct( screen );
    return 0;
}

