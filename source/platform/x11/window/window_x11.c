
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "../../../util/vector.h"

#include "../x11_input_device.h"

#include "../x11_screen.h"
#include "../x11_window.h"

#include <limbus/window.h>

typedef struct
{
	int type;
	int files;
	int x, y;
} WindowEvent;

typedef struct
{
    X11Window base;

	Atom wm_delete_window;
	XIC input_context;
	XIM input_method;
	long event_mask;

	int fullscreen;
	char* caption;
	int width, height;
	int x, y,
	    absolute_x, absolute_y;
	int created_impl;
	Vector events;
	WindowEvent event;
	Vector devices;
	
	Atom XdndEnter,
	     XdndPosition,
	     XdndStatus,
	     XdndDrop,
	     XdndActionCopy,
	     XdndFinished,
	     XdndSelection,
	     selection_atom,
	     dnd_target_type;
	Window dnd_source_window;
	int dnd_version,
	    dnd_x, dnd_y;
	Vector files;
} X11WindowImpl;

#include "dnd.h"

static void impl_create_window( void* win, XVisualInfo* visual_info );
static void impl_destroy_window( void* win );
static void impl_update_window_caption( X11WindowImpl* window );

void* lb_window_construct( void* scr )
{
	X11WindowImpl* window;
	X11Screen* screen;

	screen = (X11Screen*)scr;
	assert( screen );

	window = (X11WindowImpl*)malloc( sizeof *window );
	assert( window != NULL );

	window->base.display = screen->display;
	window->base.screen = screen->screen;
	window->base.create_window_impl = &impl_create_window;
	window->base.destroy_window_impl = &impl_destroy_window;

	vector_construct( &window->events, sizeof( WindowEvent ) );
	vector_construct( &window->devices, sizeof( X11InputDevice* ) );
	vector_construct( &window->files, DND_MAX_PATH_SIZE );

	window->created_impl = 0;
	window->caption = NULL;
	window->x = 0;
	window->y = 0;
	window->width = 640;
	window->height = 480;
	window->fullscreen = 0;

	lb_window_set_caption( window, "" );

	return window;
}

void* lb_window_destruct( void* win )
{
	X11WindowImpl* window = (X11WindowImpl*)win;

	if (window->created_impl)
	{
		impl_destroy_window( window );
	}

	free( window->caption );

	vector_destruct( &window->events );
	vector_destruct( &window->devices );
	vector_destruct( &window->files );

	free( window );
	return NULL;
}

int lb_window_constructed( void* window )
{
    return (window != NULL) ? 1 : 0;
}

void lb_window_clear_decoration( void* win )
{
	X11WindowImpl* window = (X11WindowImpl*)win;
    window->fullscreen = 1;
}

void lb_window_set_caption( void* win, const char* caption )
{
	X11WindowImpl* window = (X11WindowImpl*)win;
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

void lb_window_set_width( void* win, int width )
{
	X11WindowImpl* window = (X11WindowImpl*)win;
	window->width = width;
}

void lb_window_set_height( void* win, int height )
{
	X11WindowImpl* window = (X11WindowImpl*)win;
	window->height = height;
}

void lb_window_set_x( void* win, int x )
{
	X11WindowImpl* window = (X11WindowImpl*)win;
	window->x = x;
	if (window->created_impl)
	{
		XMoveWindow( window->base.display, window->base.window, window->x, window->y );
	}
}

void lb_window_set_y( void* win, int y )
{
	X11WindowImpl* window = (X11WindowImpl*)win;
	window->y = y;
	if (window->created_impl)
	{
		XMoveWindow( window->base.display, window->base.window, window->x, window->y );
	}
}

const char* lb_window_get_caption( void* win )
{
	X11WindowImpl* window = (X11WindowImpl*)win;
	return window->caption;
}

int lb_window_get_width( void* win )
{
	X11WindowImpl* window = (X11WindowImpl*)win;
	return window->width;
}

int lb_window_get_height( void* win )
{
	X11WindowImpl* window = (X11WindowImpl*)win;
	return window->height;
}

int lb_window_get_x( void* win )
{
	X11WindowImpl* window = (X11WindowImpl*)win;
	return window->x;
}

int lb_window_get_y( void* win )
{
	X11WindowImpl* window = (X11WindowImpl*)win;
	return window->y;
}

int lb_window_next_event( void* win )
{
	X11WindowImpl* window = (X11WindowImpl*)win;
	WindowEvent* iter;
	XEvent event;

	while (XPending( window->base.display ))
	{
	    XNextEvent( window->base.display, &event );
	    
	    /* Let registered input devices look at the event */
	    {
            X11InputDevice** iter = NULL;
		    while ((iter = (X11InputDevice**)vector_next( &window->devices, iter )))
		    {
			    (*iter)->handle_x11_event( (*iter), event );
		    }
        }

		if (event.type == ConfigureNotify)
		{
		    XConfigureEvent* configure_event = (XConfigureEvent*)&event;
		    
		    window->x = configure_event->x;
		    window->y = configure_event->y;
		    window->width = configure_event->width;
		    window->height = configure_event->height;
		}

		if (event.type == ClientMessage)
		{
			if (event.xclient.data.l[0] == window->wm_delete_window)
			{
				window->event.type = LBWindowEventClose;
				return 1;
			}
		}

		dnd_handle_event( window, event );
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
	X11WindowImpl* window = (X11WindowImpl*)win;
	return window->event.type;
}

int lb_window_get_event_x( void* win )
{
	X11WindowImpl* window = (X11WindowImpl*)win;
	return window->event.x;
}

int lb_window_get_event_y( void* win )
{
	X11WindowImpl* window = (X11WindowImpl*)win;
	return window->event.y;
}

int lb_window_get_event_files( void* win )
{
	X11WindowImpl* window = (X11WindowImpl*)win;
	return window->event.files;
}

const char* lb_window_get_event_file( void* win, int i )
{
	X11WindowImpl* window = (X11WindowImpl*)win;
	return (char*)vector_at( &window->files, i );
}

void lb_window_add_input_device( void* win, void* d )
{
	X11InputDevice* device = (X11InputDevice*)d;
	X11WindowImpl* window = (X11WindowImpl*)win;

	vector_push_back( &window->devices, &device );

	if (window->created_impl)
	{
		device->display = window->base.display;
		device->window = window->base.window;
		device->root_window = window->base.root_window;
		device->input_context = window->input_context;
		device->construct( device );
	}
}

void impl_create_window( void* win, XVisualInfo* visual_info )
{
	X11WindowImpl* window = (X11WindowImpl*)win;
	Colormap color_map;
    XSetWindowAttributes window_attributes;
    unsigned long window_attribute_mask;

    color_map = XCreateColormap( window->base.display,
    							 RootWindow( window->base.display,
			    							 visual_info->screen ),
    							 visual_info->visual,
    							 AllocNone );

    window_attributes.colormap = color_map;
    window_attributes.override_redirect = (window->fullscreen) ? True : False;
    window_attribute_mask = CWColormap | CWOverrideRedirect;

	window->base.root_window = XRootWindow( window->base.display, window->base.screen );
	window->base.window = XCreateWindow( window->base.display,
                                         window->base.root_window,
                                         window->x, window->y,
                                         window->width, window->height,
                                         0,
                                         visual_info->depth,
                                         InputOutput,
                                         visual_info->visual,
                                         window_attribute_mask,
                                         &window_attributes );

	/* Configure window events */
	window->wm_delete_window = XInternAtom( window->base.display,
                                            "WM_DELETE_WINDOW",
                                            False );
	XSetWMProtocols( window->base.display,
					 window->base.window,
					 &window->wm_delete_window,
					 1 );

	window->event_mask = ExposureMask |
                         KeyPressMask |
                         KeyReleaseMask |
                         PointerMotionMask |
                         ButtonPressMask |
                         ButtonReleaseMask |
                         StructureNotifyMask;

	XSelectInput( window->base.display,
				  window->base.window,
				  window->event_mask );

	/* Create input context/method */
	window->input_method = XOpenIM( window->base.display, NULL, NULL, NULL );
	assert( window->input_method != NULL );
	window->input_context = XCreateIC( window->input_method,
                                       XNClientWindow, window->base.window,
                                       XNFocusWindow, window->base.window,
                                       XNInputStyle,
                                       XIMPreeditNothing | XIMStatusNothing,
                                       NULL );
	assert( window->input_context != NULL );

	XMapWindow( window->base.display,
				window->base.window );

    dnd_setup( window );
	impl_update_window_caption( window );

	XFlush( window->base.display );

	{
		X11InputDevice** iter = NULL;
		while ((iter = (X11InputDevice**)vector_next( &window->devices, iter )))
		{
			(*iter)->display = window->base.display;
			(*iter)->window = window->base.window;
			(*iter)->root_window = window->base.root_window;
			(*iter)->input_context = window->input_context;
			(*iter)->construct( (*iter) );
		}
	}

	window->created_impl = 1;
}

void impl_destroy_window( void* win )
{
	X11WindowImpl* window = (X11WindowImpl*)win;

	XDestroyIC( window->input_context );
	XCloseIM( window->input_method );

	XDestroyWindow( window->base.display,
					window->base.window );

	XFlush( window->base.display );

	window->created_impl = 0;
}

void impl_update_window_caption( X11WindowImpl* window )
{
	XTextProperty text_property;

	if (XStringListToTextProperty( &window->caption, 1, &text_property ))
	{
		XSetWMName( window->base.display,
					window->base.window,
					&text_property );

		XFlush( window->base.display );

		XFree( text_property.value );
	}
}

