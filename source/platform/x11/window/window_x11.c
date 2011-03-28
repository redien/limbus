
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
	union
	{
		struct
		{
			int files;
			int x, y;
		} file_drop;
		
		struct
		{
			int width, height;
		} resize;
	} u;
} WindowEvent;

typedef struct
{
    X11Window base;

	X11Screen* initial_screen;

	Atom wm_delete_window;
	XIC input_context;
	XIM input_method;
	long event_mask;

	int styled;
	int resizable;
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

static void impl_create_window( LBWindow win, XVisualInfo* visual_info );
static void impl_destroy_window( LBWindow win );
static void impl_update_window_caption( X11WindowImpl* window );

void* lb_window_construct( void )
{
	X11WindowImpl* window;

	window = (X11WindowImpl*)malloc( sizeof *window );
	assert( window != NULL );

	window->base.screen = lb_screen_construct( LBScreenDefault );
	assert( lb_screen_constructed( window->base.screen ) == 1 );
	window->initial_screen = window->base.screen;

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
	window->styled = 1;
	window->resizable = 0;

	lb_window_set_caption( window, "" );

	return window;
}

void* lb_window_destruct( LBWindow win )
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
	X11WindowImpl* window = (X11WindowImpl*)_window;
	window->base.screen = screen;
}

LBScreen lb_window_get_screen( LBWindow _window )
{
	X11WindowImpl* window = (X11WindowImpl*)_window;
	return window->base.screen;
}


/* TODO: Make it possible to enable/disable styles after a window
         has been created. */

void lb_window_set_styled( LBWindow _window, int value )
{
	X11WindowImpl* window = (X11WindowImpl*)_window;
	window->styled = value;
}

int lb_window_get_styled( LBWindow _window )
{
	X11WindowImpl* window = (X11WindowImpl*)_window;
	return window->styled;
}

void lb_window_set_resizable( LBWindow _window, int value )
{
	X11WindowImpl* window = (X11WindowImpl*)_window;
	window->resizable = value;
}

int lb_window_get_resizable( LBWindow _window )
{
	X11WindowImpl* window = (X11WindowImpl*)_window;
	return window->resizable;
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

/* TODO: Make sure that setting width or height to 0 to generate an error */
void lb_window_set_width( void* win, int width )
{
	X11WindowImpl* window = (X11WindowImpl*)win;
	window->width = width;
	if (window->created_impl)
	{
		XResizeWindow( window->base.screen->display, window->base.window,
		               window->width, window->height );
	}
}

void lb_window_set_height( void* win, int height )
{
	X11WindowImpl* window = (X11WindowImpl*)win;
	window->height = height;
	if (window->created_impl)
	{
		XResizeWindow( window->base.screen->display, window->base.window,
		               window->width, window->height );
	}
}

void lb_window_set_x( void* win, int x )
{
	X11WindowImpl* window = (X11WindowImpl*)win;
	window->x = x;
	if (window->created_impl)
	{
		XMoveWindow( window->base.screen->display, window->base.window,
		             window->x, window->y );
	}
}

void lb_window_set_y( void* win, int y )
{
	X11WindowImpl* window = (X11WindowImpl*)win;
	window->y = y;
	if (window->created_impl)
	{
		XMoveWindow( window->base.screen->display, window->base.window,
		             window->x, window->y );
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

	while (XPending( window->base.screen->display ))
	{
	    XNextEvent( window->base.screen->display, &event );
	    
	    /* Let registered input devices look at the event */
	    {
            X11InputDevice** iter = NULL;
		    while ((iter = (X11InputDevice**)vector_next( &window->devices,
		                                                  iter )))
		    {
			    (*iter)->handle_x11_event( (*iter), event );
		    }
        }

		if (event.type == ConfigureNotify)
		{
		    XConfigureEvent* configure_event = (XConfigureEvent*)&event;
		    
		    if (configure_event->width != window->width ||
		    	configure_event->height != window->height)
		    {
		        WindowEvent event;
		        event.type = LBWindowEventResize;
		        event.u.resize.width = window->width;
		        event.u.resize.height = window->height;
                vector_push_back( &window->events, &event );
		    }

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
	return window->event.u.file_drop.x;
}

int lb_window_get_event_y( void* win )
{
	X11WindowImpl* window = (X11WindowImpl*)win;
	return window->event.u.file_drop.y;
}

int lb_window_get_event_files( void* win )
{
	X11WindowImpl* window = (X11WindowImpl*)win;
	return window->event.u.file_drop.files;
}

int lb_window_get_event_width( void* win )
{
	X11WindowImpl* window = (X11WindowImpl*)win;
	return window->event.u.resize.width;
}

int lb_window_get_event_height( void* win )
{
	X11WindowImpl* window = (X11WindowImpl*)win;
	return window->event.u.resize.height;
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
		device->display = window->base.screen->display;
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

    color_map = XCreateColormap( window->base.screen->display,
    							 RootWindow( window->base.screen->display,
			    							 visual_info->screen ),
    							 visual_info->visual,
    							 AllocNone );

    window_attributes.colormap = color_map;
    window_attributes.override_redirect = (window->styled) ? False : True;
    window_attribute_mask = CWColormap | CWOverrideRedirect;

	window->base.root_window = XRootWindow( window->base.screen->display,
	                                        window->base.screen->screen );
	window->base.window = XCreateWindow( window->base.screen->display,
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
	window->wm_delete_window = XInternAtom( window->base.screen->display,
                                            "WM_DELETE_WINDOW",
                                            False );
	XSetWMProtocols( window->base.screen->display,
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

	XSelectInput( window->base.screen->display,
				  window->base.window,
				  window->event_mask );

	/* Create input context/method */
	window->input_method = XOpenIM( window->base.screen->display, NULL, NULL, NULL );
	assert( window->input_method != NULL );
	window->input_context = XCreateIC( window->input_method,
                                       XNClientWindow, window->base.window,
                                       XNFocusWindow, window->base.window,
                                       XNInputStyle,
                                       XIMPreeditNothing | XIMStatusNothing,
                                       NULL );
	assert( window->input_context != NULL );
	
	/* Enable/Disable decorations */
	if (!window->resizable)
	{
		XSizeHints* size_hints = XAllocSizeHints();
		
		size_hints->flags = PMaxSize | PMinSize;
		size_hints->min_width = size_hints->max_width = window->width;
		size_hints->min_height = size_hints->max_height = window->height;
		XSetWMNormalHints( window->base.screen->display, window->base.window, size_hints );

		XFree( size_hints );
	}

	XMapWindow( window->base.screen->display,
				window->base.window );

    dnd_setup( window );
	impl_update_window_caption( window );

	XFlush( window->base.screen->display );

	{
		X11InputDevice** iter = NULL;
		while ((iter = (X11InputDevice**)vector_next( &window->devices, iter )))
		{
			(*iter)->display = window->base.screen->display;
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

	XDestroyWindow( window->base.screen->display,
					window->base.window );

	XFlush( window->base.screen->display );

	window->created_impl = 0;
}

void impl_update_window_caption( X11WindowImpl* window )
{
	XTextProperty text_property;

	if (XStringListToTextProperty( &window->caption, 1, &text_property ))
	{
		XSetWMName( window->base.screen->display,
					window->base.window,
					&text_property );

		XFlush( window->base.screen->display );

		XFree( text_property.value );
	}
}

