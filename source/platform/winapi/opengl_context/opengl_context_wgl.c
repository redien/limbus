
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include <limbus/opengl_context.h>
#include "../wgl_opengl_context.h"
#include "../win_window.h"

#include <assert.h>

typedef struct
{
	wglContext base;

	PIXELFORMATDESCRIPTOR* pixelformats;
	int number_of_pixelformats;
	int default_pixelformat;
	WinWindow* window;
	int released,
	    use_current;
} wglContextImpl;

static int get_default_pixelformat( void* con );

static void set_pixelformat_list( wglContextImpl* context )
{
	int i;

	context->base.pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );
	context->base.pfd.nVersion = 1;
	context->number_of_pixelformats =
						DescribePixelFormat( context->base.device_context, 1, 0, NULL );
	assert( context->number_of_pixelformats );

	context->pixelformats = (PIXELFORMATDESCRIPTOR*)malloc(
						sizeof( PIXELFORMATDESCRIPTOR ) * context->number_of_pixelformats );
	assert( context->pixelformats );

	for (i = 0; i < context->number_of_pixelformats; i++)
	{
		int result;
		context->pixelformats[i].nSize = sizeof( PIXELFORMATDESCRIPTOR );
		context->pixelformats[i].nVersion = 1;
		result = DescribePixelFormat( context->base.device_context,
									  i + 1,
									  sizeof( PIXELFORMATDESCRIPTOR ),
									  &context->pixelformats[i] );
		assert( result );
	}
}

void* lb_opengl_context_construct( void* win, int use_current )
{
	wglContextImpl* context;
	WinWindow* window = (WinWindow*)win;

	context = (wglContextImpl*)malloc( sizeof *context );
	assert( context );

	window->construct_impl( window );

	context->window = window;
	context->released = 0;
	context->use_current = use_current;

	context->base.device_context = GetDC( context->window->handle );
	assert( context->base.device_context );

	set_pixelformat_list( context );

	context->default_pixelformat = get_default_pixelformat( context );

	return context;
}

void* lb_opengl_context_destruct( void* con )
{
	wglContextImpl* context = (wglContextImpl*)con;

	if (context->released == 0)
	{
		BOOL result;
		result = wglDeleteContext( context->base.render_context );
		assert( result );
		ReleaseDC( context->window->handle, context->base.device_context );
	}

	free( context->pixelformats );
	free( context );

	return NULL;
}

void lb_opengl_context_release_ownership( void* con )
{
	wglContextImpl* context = (wglContextImpl*)con;
	context->released = 1;
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
	wglContextImpl* context = (wglContextImpl*)con;
	SwapBuffers( context->base.device_context );
}

int lb_opengl_context_pixelformats( void* con )
{
	wglContextImpl* context = (wglContextImpl*)con;
	return context->number_of_pixelformats + 1;
}

int lb_opengl_context_get_pixelformat_double_buffer( void* con,
                                                     int format )
{
	wglContextImpl* context = (wglContextImpl*)con;
	
	if (format == 0)
		{ format = context->default_pixelformat; }
	format--;

	if (context->pixelformats[format].dwFlags & PFD_DOUBLEBUFFER)
		return 1;
	else
		return 0;
}

int lb_opengl_context_get_pixelformat_support_opengl( void* con,
                                                     int format )
{
	wglContextImpl* context = (wglContextImpl*)con;
	
	if (format == 0)
		{ format = context->default_pixelformat; }
	format--;

	if (context->pixelformats[format].dwFlags & PFD_SUPPORT_OPENGL)
		return 1;
	else
		return 0;
}

LBOpenglContextColorFormat lb_opengl_context_get_pixelformat_color_format( void* con, int format )
{
	wglContextImpl* context = (wglContextImpl*)con;

	if (format == 0)
		{ format = context->default_pixelformat; }
	format--;

	if (context->pixelformats[format].iPixelType == PFD_TYPE_RGBA)
		return LBOpenglContextColorFormatRGBA;
	else
		return LBOpenglContextColorFormatIndexed;
}

int lb_opengl_context_get_pixelformat_depth_size( void* con,
                                                  int format )
{
	wglContextImpl* context = (wglContextImpl*)con;

	if (format == 0)
		{ format = context->default_pixelformat; }
	format--;

	return context->pixelformats[format].cDepthBits;
}

static int get_default_pixelformat( void* con )
{
	int i, size;
	wglContextImpl* context = (wglContextImpl*)con;

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
	BOOL result;
	wglContextImpl* context = (wglContextImpl*)con;

	if (format == 0)
	{
		format = context->default_pixelformat;
	}

	format--;

	result = SetPixelFormat( context->base.device_context,
							 format + 1,
							 &context->pixelformats[format] );
	assert( result );

	if (context->use_current)
	{
	    context->base.render_context = wglGetCurrentContext();
	}
	else
	{
    	context->base.render_context = wglCreateContext( context->base.device_context );
	}

	assert( context->base.render_context );
}

void lb_opengl_context_make_current( void* con )
{
	BOOL result;
	wglContextImpl* context = (wglContextImpl*)con;

	result = wglMakeCurrent( context->base.device_context,
							 context->base.render_context );
	assert( result );
}

void lb_opengl_context_release_current( void* con )
{
	BOOL result;
	wglContextImpl* context = (wglContextImpl*)con;

	/* TODO: This releases the device context as well, how will this be
	   handled? */
	result = wglMakeCurrent( context->base.device_context,
							 NULL );
	assert( result );
}

typedef BOOL (*WGLSWAPINTERVALEXTPROC)( int );
int lb_opengl_context_set_swap_interval( void* con, int interval )
{
	WGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
	wglContextImpl* context = (wglContextImpl*)con;

	wglSwapIntervalEXT = (WGLSWAPINTERVALEXTPROC)wglGetProcAddress( "wglSwapIntervalEXT" );
	if (wglSwapIntervalEXT)
	{
		wglSwapIntervalEXT( interval );
		return 1;
	}

	return 0;
}

