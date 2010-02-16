
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

static void create_window_impl( void* win, XVisualInfo* visual_info );
static void destroy_window_impl( void* win );

typedef struct
{
	int type;
	int files;
	int x, y;
} WindowEvent;

typedef struct
{
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
} X11WindowData;

#define DECLARE_WINDOW_AND_DATA()\
	X11Window* window;\
	X11WindowData* window_data;\
	window = (X11Window*)win;\
	assert( window );\
	window_data = (X11WindowData*)window->impl_data;\
	assert( window_data );

#include "dnd.h"

void* lb_window_construct( void* scr )
{
	X11Window* window;
	X11WindowData* window_data;
	X11Screen* screen;

	screen = (X11Screen*)scr;
	assert( screen );

	window = (X11Window*)malloc( sizeof *window );
	assert( window != NULL );

	window->display = screen->display;
	window->screen = screen->screen;
	window->create_window_impl = &create_window_impl;
	window->destroy_window_impl = &destroy_window_impl;

	window_data = (X11WindowData*)malloc( sizeof *window_data );
	assert( window_data != NULL );

	vector_construct( &window_data->events, sizeof( WindowEvent ) );
	vector_construct( &window_data->devices, sizeof( X11InputDevice* ) );
	vector_construct( &window_data->files, sizeof( char ) * DND_MAX_PATH_SIZE );

	window_data->created_impl = 0;
	window_data->caption = 0;
	window_data->x = 0;
	window_data->y = 0;
	window_data->width = 640;
	window_data->height = 480;
	window_data->fullscreen = 0;
	window->impl_data = window_data;

	lb_window_set_caption( window, "" );

	return window;
}

void* lb_window_destruct( void* win )
{
	DECLARE_WINDOW_AND_DATA()

	if (window_data->created_impl)
	{
		destroy_window_impl( win );
	}

	free( window_data->caption );

	vector_destruct( &window_data->events );
	vector_destruct( &window_data->devices );
	vector_destruct( &window_data->files );

	free( window_data );
	free( window );
	return NULL;
}

int lb_window_constructed( void* window )
{
    return (window != NULL) ? 1 : 0;
}

void create_window_impl( void* win, XVisualInfo* visual_info )
{
	Colormap color_map;
	XTextProperty text_property;
    XSetWindowAttributes attribs;
    unsigned long attribute_mask = CWColormap
                                 | CWOverrideRedirect;
	DECLARE_WINDOW_AND_DATA()
	assert( visual_info );

	/* Create colormap from visual info */
    color_map = XCreateColormap( window->display,
    							 RootWindow( window->display,
			    							 visual_info->screen ),
    							 visual_info->visual,
    							 AllocNone );
    attribs.colormap = color_map;

    attribs.override_redirect = (window_data->fullscreen) ? True : False;
    
	/* Create! */
	window->window = XCreateWindow( window->display,
									XDefaultRootWindow( window->display ),
									window_data->x, window_data->y,
									window_data->width, window_data->height,
									0,
									visual_info->depth,
									InputOutput,
									visual_info->visual,
									attribute_mask,
									&attribs );

	window->root_window = XRootWindow( window->display, window->screen );
	
	/* Set event settings */
	window->wm_delete_window = XInternAtom( window->display,
											"WM_DELETE_WINDOW",
											False );
	XSetWMProtocols( window->display,
					 window->window,
					 &window->wm_delete_window,
					 1 );

	window->event_mask = ExposureMask |
						 KeyPressMask |
						 KeyReleaseMask |
						 PointerMotionMask |
						 ButtonPressMask |
						 ButtonReleaseMask |
						 StructureNotifyMask;

	XSelectInput( window->display,
				  window->window,
				  window->event_mask );

	/* Create input context/method */
	window->input_method = XOpenIM( window->display, NULL, NULL, NULL );
	assert( window->input_method != NULL );
	window->input_context = XCreateIC( window->input_method,
									   XNClientWindow, window->window,
									   XNFocusWindow, window->window,
									   XNInputStyle,
									   XIMPreeditNothing | XIMStatusNothing,
									   NULL );
	assert( window->input_context != NULL );

	XMapWindow( window->display,
				window->window );

    dnd_setup( window, window_data );

	XFlush( window->display );

	/* Set caption */
	if (XStringListToTextProperty( &window_data->caption, 1, &text_property ))
	{
		XSetWMName( window->display,
					window->window,
					&text_property );

		XFlush( window->display );
		
		XFree( text_property.value );
	}

	{
		X11InputDevice** iter = NULL;
		while ((iter = (X11InputDevice**)vector_next( &window_data->devices, iter )))
		{
			(*iter)->display = window->display;
			(*iter)->window = window->window;
			(*iter)->root_window = window->root_window;
			(*iter)->input_context = window->input_context;
			(*iter)->construct( (*iter) );
		}
	}

	window_data->created_impl = 1;
}

void destroy_window_impl( void* win )
{
	DECLARE_WINDOW_AND_DATA()

	XDestroyIC( window->input_context );
	XCloseIM( window->input_method );

	XDestroyWindow( window->display,
					window->window );

	XFlush( window->display );

	window_data->created_impl = 0;
}

void lb_window_clear_decoration( void* win )
{
    DECLARE_WINDOW_AND_DATA()
    window_data->fullscreen = 1;
}

void lb_window_set_caption( void* win, const char* caption )
{
	int size;
	DECLARE_WINDOW_AND_DATA()

	if (window_data->caption)
		free( window_data->caption );

	size = strlen( caption ) + 1;
	window_data->caption = (char*)malloc( size );
	assert( window_data->caption != NULL );
	memcpy( window_data->caption, caption, size );

	if (window_data->created_impl)
	{
		XTextProperty text_property;
		if (XStringListToTextProperty( (char**)&caption, 1, &text_property ))
		{
			XSetWMName( window->display,
						window->window,
						&text_property );

			XFlush( window->display );
		}
	}
}

void lb_window_set_width( void* win, int width )
{
	DECLARE_WINDOW_AND_DATA()
	window_data->width = width;
}

void lb_window_set_height( void* win, int height )
{
	DECLARE_WINDOW_AND_DATA()
	window_data->height = height;
}

void lb_window_set_x( void* win, int x )
{
	DECLARE_WINDOW_AND_DATA()
	window_data->x = x;
	if (window_data->created_impl)
		XMoveWindow( window->display, window->window, window_data->x, window_data->y );
}

void lb_window_set_y( void* win, int y )
{
	DECLARE_WINDOW_AND_DATA()
	window_data->y = y;
	if (window_data->created_impl)
		XMoveWindow( window->display, window->window, window_data->x, window_data->y );
}


const char* lb_window_get_caption( void* win )
{
	DECLARE_WINDOW_AND_DATA()
	return window_data->caption;
}

int lb_window_get_width( void* win )
{
	DECLARE_WINDOW_AND_DATA()
	return window_data->width;
}

int lb_window_get_height( void* win )
{
	DECLARE_WINDOW_AND_DATA()
	return window_data->height;
}

int lb_window_get_x( void* win )
{
	DECLARE_WINDOW_AND_DATA()
	return window_data->x;
}

int lb_window_get_y( void* win )
{
	DECLARE_WINDOW_AND_DATA()
	return window_data->y;
}

int lb_window_next_event( void* win )
{
	XEvent event;
	WindowEvent* iter;
	int get_next = 1;
	DECLARE_WINDOW_AND_DATA()

	iter = (WindowEvent*)vector_next( &window_data->events, NULL );
	if (iter != NULL)
	{
		window_data->event = *iter;
		vector_erase( &window_data->events, iter );
		return 1;
	}
	
	while (get_next)
	{
		get_next = 0;
		if (XCheckWindowEvent( window->display,
                               window->window,
                               window->event_mask,
                               &event ))
		{
			X11InputDevice** iter = NULL;
			while ((iter = (X11InputDevice**)vector_next( &window_data->devices, iter )))
			{
				(*iter)->handle_x11_event( (*iter), event );
			}

			if (event.type == ConfigureNotify)
			{
			    XConfigureEvent* configure_event = (XConfigureEvent*)&event;
			    
			    window_data->x = configure_event->x;
			    window_data->y = configure_event->y;
			    window_data->width = configure_event->width;
			    window_data->height = configure_event->height;
			}
			
			dnd_handle_message( window, window_data, event );

			get_next = 1;
		}
		else if (XCheckTypedWindowEvent( window->display,
										 window->window,
										 ClientMessage,
										 &event ))
		{
			if (event.xclient.data.l[0] == window->wm_delete_window)
			{
				window_data->event.type = LBWindowEventClose;
				return 1;
			}

			dnd_handle_message( window, window_data, event );

			get_next = 1;
		}
		else if (XCheckTypedWindowEvent( window->display,
										 window->window,
										 SelectionNotify,
										 &event ))
		{
			dnd_handle_message( window, window_data, event );
			get_next = 1;
		}
	}

	return 0;
}

enum LBWindowEvent lb_window_get_event_type( void* win )
{
	DECLARE_WINDOW_AND_DATA()
	return window_data->event.type;
}

int lb_window_get_event_x( void* win )
{
	DECLARE_WINDOW_AND_DATA()
	return window_data->event.x;
}

int lb_window_get_event_y( void* win )
{
	DECLARE_WINDOW_AND_DATA()
	return window_data->event.y;
}

int lb_window_get_event_files( void* win )
{
	DECLARE_WINDOW_AND_DATA()
	return window_data->event.files;
}

const char* lb_window_get_event_file( void* win, int i )
{
	DECLARE_WINDOW_AND_DATA()
	return (char*)vector_at( &window_data->files, i );
}

void lb_window_add_input_device( void* win, void* d )
{
	X11InputDevice* device = (X11InputDevice*)d;
	DECLARE_WINDOW_AND_DATA()

	vector_push_back( &window_data->devices, &device );

	if (window_data->created_impl)
	{
		device->display = window->display;
		device->window = window->window;
		device->root_window = window->root_window;
		device->input_context = window->input_context;
		device->construct( device );
	}
}

