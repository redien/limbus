
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "../../../util/vector.h"

#include "../sdl_input_device.h"

#include "../sdl_screen.h"
#include "../sdl_window.h"

#include <limbus/window.h>

typedef struct
{
	int type;
	union
	{
		struct
		{
			int width, height;
		} resize;
	} u;
} WindowEvent;

typedef struct
{
    SDLWindow base;
	SDLScreen* initial_screen;
	
    int styled;
    int resizable;
    int width, height;
    int x, y;
    char* caption;
    int created_impl;

    WindowEvent event;
    
    Vector events;
	Vector devices;
} SDLWindowImpl;

void impl_create_window( void* win, Uint32 flags );
void impl_destroy_window( void* win );
void impl_update_window_caption( SDLWindowImpl* window );

void* lb_window_construct( void )
{
	SDLWindowImpl* window;
    
	window = (SDLWindowImpl*)malloc( sizeof *window );
	assert( window != NULL );

	window->base.screen = lb_screen_construct( LBScreenDefault );
	assert( lb_screen_constructed( window->base.screen ) == 1 );
	window->initial_screen = window->base.screen;

	window->base.create_window_impl = &impl_create_window;
	window->base.destroy_window_impl = &impl_destroy_window;

	vector_construct( &window->events, sizeof( WindowEvent ) );
	vector_construct( &window->devices, sizeof( SDLInputDevice* ) );

	window->created_impl = 0;
	window->caption = NULL;
	window->x = 0;
	window->y = 0;
	window->width = 640;
	window->height = 480;
	window->styled = 1;
	window->resizable = 0;

	lb_window_set_caption( window, "" );

	return window;
}

void* lb_window_destruct( LBWindow win )
{
	SDLWindowImpl* window = (SDLWindowImpl*)win;

	if (window->created_impl)
	{
		impl_destroy_window( window );
	}

	free( window->caption );

	vector_destruct( &window->events );
	vector_destruct( &window->devices );
	
	free( window->initial_screen );

	free( window );
	return NULL;
}

int lb_window_constructed( LBWindow window )
{
    return (window != NULL) ? 1 : 0;
}


void lb_window_set_screen( LBWindow _window, LBScreen screen )
{
	SDLWindowImpl* window = (SDLWindowImpl*)_window;
	window->base.screen = screen;
}

LBScreen lb_window_get_screen( LBWindow _window )
{
	SDLWindowImpl* window = (SDLWindowImpl*)_window;
	return window->base.screen;
}


/* TODO: Make it possible to enable/disable styles after a window
         has been created. */

void lb_window_set_styled( LBWindow _window, int value )
{
	SDLWindowImpl* window = (SDLWindowImpl*)_window;
	window->styled = value;
}

int lb_window_get_styled( LBWindow _window )
{
	SDLWindowImpl* window = (SDLWindowImpl*)_window;
	return window->styled;
}

void lb_window_set_resizable( LBWindow _window, int value )
{
	SDLWindowImpl* window = (SDLWindowImpl*)_window;
	window->resizable = value;
}

int lb_window_get_resizable( LBWindow _window )
{
	SDLWindowImpl* window = (SDLWindowImpl*)_window;
	return window->resizable;
}


void lb_window_set_caption( void* win, const char* caption )
{
	SDLWindowImpl* window = (SDLWindowImpl*)win;
	int size;

	if (window->caption)
	{
		free( window->caption );
	}

	size = strlen( caption ) + 1;
	window->caption = (char*)malloc( size );
	assert( window->caption != NULL );
	memcpy( window->caption, caption, size );

	if (window->created_impl)
	{
		impl_update_window_caption( window );
	}
}

/* TODO: Make sure that setting width or height to 0 to generate an error */
void lb_window_set_width( void* win, int width )
{
	SDLWindowImpl* window = (SDLWindowImpl*)win;
	window->width = width;
	if (window->created_impl)
	{
        SDL_SetWindowSize(window->base.window, window->width, window->height);
	}
}

void lb_window_set_height( void* win, int height )
{
	SDLWindowImpl* window = (SDLWindowImpl*)win;
	window->height = height;
	if (window->created_impl)
	{
        SDL_SetWindowSize(window->base.window, window->width, window->height);
	}
}

void lb_window_set_x( void* win, int x )
{
	SDLWindowImpl* window = (SDLWindowImpl*)win;
	window->x = x;
	if (window->created_impl)
	{
		SDL_SetWindowPosition(window->base.window, window->x, window->y);
	}
}

void lb_window_set_y( void* win, int y )
{
	SDLWindowImpl* window = (SDLWindowImpl*)win;
	window->y = y;
	if (window->created_impl)
	{
		SDL_SetWindowPosition(window->base.window, window->x, window->y);
	}
}

const char* lb_window_get_caption( void* win )
{
	SDLWindowImpl* window = (SDLWindowImpl*)win;
	return window->caption;
}

int lb_window_get_width( void* win )
{
	SDLWindowImpl* window = (SDLWindowImpl*)win;
	return window->width;
}

int lb_window_get_height( void* win )
{
	SDLWindowImpl* window = (SDLWindowImpl*)win;
	return window->height;
}

int lb_window_get_x( void* win )
{
	SDLWindowImpl* window = (SDLWindowImpl*)win;
	return window->x;
}

int lb_window_get_y( void* win )
{
	SDLWindowImpl* window = (SDLWindowImpl*)win;
	return window->y;
}

int lb_window_next_event( void* win )
{
	SDLWindowImpl* window = (SDLWindowImpl*)win;
	WindowEvent* iter;
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
	    /* Let registered input devices look at the event */
	    {
            SDLInputDevice** iter = NULL;
		    while ((iter = (SDLInputDevice**)vector_next( &window->devices,
		                                                  iter )))
		    {
			    (*iter)->handle_sdl_event( (*iter), &event );
		    }
        }

		if (event.type == SDL_WINDOWEVENT)
		{
            if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
		        WindowEvent window_event;
		        window_event.type = LBWindowEventResize;
		        window_event.u.resize.width = event.window.data1;
		        window_event.u.resize.height = event.window.data2;
                vector_push_back( &window->events, &window_event );

                window->width = event.window.data1;
                window->height = event.window.data2;
            }

            if (event.window.event == SDL_WINDOWEVENT_MOVED) {
                window->x = event.window.data1;
                window->y = event.window.data2;
            }
		}

		if (event.type == SDL_QUIT)
		{
            window->event.type = LBWindowEventClose;
            return 1;
		}
	}

	iter = (WindowEvent*)vector_next( &window->events, NULL );
	if (iter != NULL)
	{
		window->event = *iter;
		vector_erase( &window->events, iter );
		
		return 1;
	}

	return 0;
}

enum LBWindowEvent lb_window_get_event_type( void* win )
{
	SDLWindowImpl* window = (SDLWindowImpl*)win;
	return window->event.type;
}

int lb_window_get_event_x( void* win )
{
	return 0;
}

int lb_window_get_event_y( void* win )
{
	return 0;
}

int lb_window_get_event_files( void* win )
{
	return 0;
}

int lb_window_get_event_width( void* win )
{
	SDLWindowImpl* window = (SDLWindowImpl*)win;
	return window->event.u.resize.width;
}

int lb_window_get_event_height( void* win )
{
	SDLWindowImpl* window = (SDLWindowImpl*)win;
	return window->event.u.resize.height;
}

const char* lb_window_get_event_file( void* win, int i )
{
	return NULL;
}

void lb_window_add_input_device( void* win, void* d )
{
	SDLInputDevice* device = (SDLInputDevice*)d;
	SDLWindowImpl* window = (SDLWindowImpl*)win;

	vector_push_back( &window->devices, &device );

	if (window->created_impl)
	{
		device->window = window->base.window;
		device->construct( device );
	}
}

void impl_create_window( void* win, Uint32 flags )
{
	SDLWindowImpl* window = (SDLWindowImpl*)win;

    assert( SDL_Init(SDL_INIT_VIDEO) == 0 );
    
	window->base.window = SDL_CreateWindow(
        window->caption,
        window->x, window->y,
        window->width, window->height,
        flags | SDL_WINDOW_SHOWN
    );

    assert( window->base.window );

	{
		SDLInputDevice** iter = NULL;
		while ((iter = (SDLInputDevice**)vector_next( &window->devices, iter )))
		{
			(*iter)->window = window->base.window;
			(*iter)->construct( (*iter) );
		}
	}

	window->created_impl = 1;
}

void impl_destroy_window( void* win )
{
	SDLWindowImpl* window = (SDLWindowImpl*)win;
    SDL_DestroyWindow( window->base.window );
	window->created_impl = 0;
}

void impl_update_window_caption( SDLWindowImpl* window )
{
    SDL_SetWindowTitle( window->base.window, window->caption );
}
