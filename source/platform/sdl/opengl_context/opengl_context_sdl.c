
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include "../glx_opengl_context.h"
#include "../x11_window.h"
#include "../x11_screen.h"

#include <limbus/opengl_context.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <limbus/opengl.h>

#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef struct
{
	XVisualInfo* visuals;
	int number_of_visuals;
	int default_pixelformat;
	X11Window* window;
	int created;
	int released;
	int use_current;
} glXContextData;

static void create_opengl_context_impl( glXContext* context,
										glXContextData* context_data,
										int pixelformat );
static void destroy_opengl_context_impl( glXContext* context,
										 glXContextData* context_data );
static void read_context_visuals( glXContext* context,
								  glXContextData* context_data );
static void free_context_visuals( glXContext* context,
								  glXContextData* context_data );
static int read_visual_attrib( glXContext* context,
							   int format,
							   int attrib );
static int get_default_pixelformat( void* con );

void* lb_opengl_context_construct( void* win, int use_current )
{
	X11Window* window;
	glXContext* context;
	glXContextData* context_data;
	
	window = (X11Window*)win;
	assert( window );
	
	context = (glXContext*)malloc( sizeof( glXContext ) );
	assert( context != NULL );
	context_data = (glXContextData*)malloc( sizeof( glXContextData ) );
	assert( context_data != NULL );
	context->impl_data = context_data;

	context->display = window->screen->display;
	context->screen = window->screen->screen;

	context_data->window = window;
	context_data->created = 0;
	context_data->released = 0;
	context_data->use_current = use_current;
	read_context_visuals( context, context_data );
	
	context_data->default_pixelformat = get_default_pixelformat( context );

	return context;
}

#define DECLARE_CONTEXT_AND_DATA()\
	glXContext* context;\
	glXContextData* context_data;\
	context = (glXContext*)con;\
	assert( context );\
	context_data = (glXContextData*)context->impl_data;\
	assert( context_data );

void* lb_opengl_context_destruct( void* con )
{
	DECLARE_CONTEXT_AND_DATA()

	if (context_data->created && !context_data->released)
		destroy_opengl_context_impl( context, context_data );

	free_context_visuals( context, context_data );

	free( context_data );
	free( context );
	
	return NULL;
}

void lb_opengl_context_release_ownership( void* con )
{
	DECLARE_CONTEXT_AND_DATA()
	context_data->released = 1;
}

int lb_opengl_context_constructed( void* con )
{
	if (con != NULL)
		return 1;
	else
		return 0;
}

void lb_opengl_context_swap_buffers( void* con )
{
	DECLARE_CONTEXT_AND_DATA()

	glXSwapBuffers( context->display,
					context->window );

	XFlush( context->display );
}

int lb_opengl_context_pixelformats( void* con )
{
	DECLARE_CONTEXT_AND_DATA()
	
	return context_data->number_of_visuals + 1;
}

#define DEFINE_PIXELFORMAT_ATTRIBUTE( name, GL_name )\
    int lb_opengl_context_get_pixelformat_##name( void* con, int format )\
    {\
	    DECLARE_CONTEXT_AND_DATA()\
	    assert( context_data->visuals );\
\
	    if (format == LBOpenglContextDefaultPixelformat)\
		    format = context_data->default_pixelformat;\
\
	    format--;\
\
	    return read_visual_attrib( context, format, GL_name );\
    }

DEFINE_PIXELFORMAT_ATTRIBUTE( double_buffer, GLX_DOUBLEBUFFER )
DEFINE_PIXELFORMAT_ATTRIBUTE( support_opengl, GLX_USE_GL )
DEFINE_PIXELFORMAT_ATTRIBUTE( depth_size, GLX_DEPTH_SIZE )

LBOpenglContextColorFormat lb_opengl_context_get_pixelformat_color_format( void* con, int format )
{
	DECLARE_CONTEXT_AND_DATA()
	assert( context_data->visuals );

	if (format == LBOpenglContextDefaultPixelformat)
		format = context_data->default_pixelformat;

	format--;

	return (read_visual_attrib( context, format, GLX_RGBA )) ? LBOpenglContextColorFormatRGBA : LBOpenglContextColorFormatIndexed;
}

static int get_default_pixelformat( void* con )
{
	int i, size;
	DECLARE_CONTEXT_AND_DATA()

	size = lb_opengl_context_pixelformats( context );
	/* iterate over every pixelformat except the default one (0) */
	for (i = 1; i < size; i++)
	{
		if (lb_opengl_context_get_pixelformat_support_opengl( context, i ) == 1)
		{
			if (lb_opengl_context_get_pixelformat_color_format( context, i ) == LBOpenglContextColorFormatRGBA)
			{
				if (lb_opengl_context_get_pixelformat_double_buffer( context, i ) == 1)
				{
					if (lb_opengl_context_get_pixelformat_depth_size( context, i ) > 0)
					{
						return i;
					}
				}
			}
		}
	}

	assert( 0 && "Could not determine a default pixelformat." );
	return 0;
}

void lb_opengl_context_bind( void* con, int format )
{
	DECLARE_CONTEXT_AND_DATA()
	
	if (format == 0)
		format = context_data->default_pixelformat;

	format--;

	create_opengl_context_impl( context, context_data, format );
}

void lb_opengl_context_make_current( void* con )
{
	DECLARE_CONTEXT_AND_DATA()

	assert( context->window != None );
	glXMakeCurrent( context->display,
					context->window,
					context->context );

	XFlush( context->display );
}

void lb_opengl_context_release_current( void* con )
{
	DECLARE_CONTEXT_AND_DATA()

	/* When not releasing the current context before destroying it,
	   apparently any subsequent calls to glXMakeCurrent will fail.
	   Maybe it tries to reference the old (destroyed) context? */
	glXMakeCurrent( context->display,
					None,
					NULL );

	XFlush( context->display );
}

void create_opengl_context_impl( glXContext* context,
								 glXContextData* context_data,
								 int format )
{
	assert( !context_data->created );

    if (context_data->use_current)
    {
        context->context = glXGetCurrentContext();
    }
    else
    {
	    context->context = glXCreateContext( context->display,
										     &context_data->visuals[format],
										     NULL,
										     GL_TRUE );
	    assert( context->context != NULL );
    }
	
	/* The X11Window implementation defers the creation of the actual window
	   until we call create_window_impl as it needs the same visual as
	   the graphics context uses and we can't determine which visual to use
	   until now. */
	context_data->window->create_window_impl( context_data->window,
											  &context_data->visuals[format] );
	context->window = context_data->window->window;

	XFlush( context->display );

	context_data->created = 1;
}

void destroy_opengl_context_impl( glXContext* context,
								  glXContextData* context_data )
{
	assert( context_data->created );

	glXDestroyContext( context->display,
					   context->context );

	XFlush( context->display );

	context_data->window->destroy_window_impl( context_data->window );
	
	context_data->created = 0;
}

void read_context_visuals( glXContext* context,
						   glXContextData* context_data )
{
	XVisualInfo visual_template;

	visual_template.screen = context->screen;
	context_data->visuals = XGetVisualInfo( context->display,
											VisualScreenMask,
											&visual_template,
											&context_data->number_of_visuals );
}

void free_context_visuals( glXContext* context,
						   glXContextData* context_data )
{
	XFree( context_data->visuals );
}

int read_visual_attrib( glXContext* context,
						int format,
						int attrib )
{
	glXContextData* context_data;
	int result;

	assert( format >= 0 );
	
	context_data = (glXContextData*)context->impl_data;
	assert( context_data != NULL );
	
	glXGetConfig( context->display,
				  &context_data->visuals[format],
				  attrib,
				  &result );
	return result;
}


typedef int (APIENTRY *GLXSWAPINTERVALSGIPROC)( int interval );
int lb_opengl_context_set_swap_interval( void* con, int interval )
{
	GLXSWAPINTERVALSGIPROC glxSwapIntervalSGI;
	char* extensions = (char*)glGetString( GL_EXTENSIONS );
	
	while (*extensions)
	{
		if (strcmp( extensions, "GLX_SGI_swap_control" ) == 0)
		{
			glxSwapIntervalSGI = (GLXSWAPINTERVALSGIPROC)glGetProcAddress( "glxSwapIntervalSGI" );
			if (glxSwapIntervalSGI)
			{
				glxSwapIntervalSGI( interval );
			}
			return 1;
		}
		extensions += 1;
	}
	
	return 0;
}

