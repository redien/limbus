
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
	PIXELFORMATDESCRIPTOR* pixelformats;
	int number_of_pixelformats;
	int default_pixelformat;
	WinWindow* window;
	int released,
	    use_current;
} wglContextData;

static int get_default_pixelformat( void* con );

static void set_pixelformat_list( wglContext* context, wglContextData* context_data )
{
	int i;

	context->pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );
	context->pfd.nVersion = 1;
	context_data->number_of_pixelformats =
						DescribePixelFormat( context->device_context, 1, 0, NULL );
	assert( context_data->number_of_pixelformats );

	context_data->pixelformats = (PIXELFORMATDESCRIPTOR*)malloc(
						sizeof( PIXELFORMATDESCRIPTOR ) * context_data->number_of_pixelformats );
	assert( context_data->pixelformats );

	for (i = 0; i < context_data->number_of_pixelformats; i++)
	{
		int result;
		context_data->pixelformats[i].nSize = sizeof( PIXELFORMATDESCRIPTOR );
		context_data->pixelformats[i].nVersion = 1;
		result = DescribePixelFormat( context->device_context,
									  i + 1,
									  sizeof( PIXELFORMATDESCRIPTOR ),
									  &context_data->pixelformats[i] );
		assert( result );
	}
}

void* lb_opengl_context_construct_in_window( void* win, int use_current )
{
	wglContext* context;
	wglContextData* context_data;
	WinWindow* window = (WinWindow*)win;

	context = (wglContext*)malloc( sizeof( wglContext ) );
	assert( context );
	context_data = (wglContextData*)malloc( sizeof( wglContextData ) );
	assert( context_data );

	window->construct_impl( window );

	context_data->window = window;
	context_data->released = 0;
	context_data->use_current = use_current;
	context->impl_data = context_data;

	context->device_context = GetDC( context_data->window->handle );
	assert( context->device_context );

	set_pixelformat_list( context, context_data );

	context_data->default_pixelformat = get_default_pixelformat( context );

	return context;
}

#define DEFINE_CONTEXT_AND_DATA()\
	wglContext* context;\
	wglContextData* context_data;\
	context = (wglContext*)con;\
	assert( context );\
	context_data = (wglContextData*)context->impl_data;\
	assert( context_data );

void* lb_opengl_context_destruct( void* con )
{
	DEFINE_CONTEXT_AND_DATA();

	if (context_data->released == 0)
	{
		BOOL result;
		result = wglDeleteContext( context->render_context );
		assert( result );
		ReleaseDC( context_data->window->handle, context->device_context );
	}

	free( context_data->pixelformats );
	free( context_data );
	free( context );

	return NULL;
}

void lb_opengl_context_release_ownership( void* con )
{
	DEFINE_CONTEXT_AND_DATA()
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
	DEFINE_CONTEXT_AND_DATA()

	SwapBuffers( context->device_context );
}

int lb_opengl_context_pixelformats( void* con )
{
	DEFINE_CONTEXT_AND_DATA()
	return context_data->number_of_pixelformats + 1;
}

int lb_opengl_context_get_pixelformat_double_buffer( void* con,
                                                     int format )
{
	DEFINE_CONTEXT_AND_DATA()
	if (format == 0)
	{ format = context_data->default_pixelformat; }
	format--;

	if (context_data->pixelformats[format].dwFlags & PFD_DOUBLEBUFFER)
		return 1;
	else
		return 0;
}

int lb_opengl_context_get_pixelformat_support_opengl( void* con,
                                                     int format )
{
	DEFINE_CONTEXT_AND_DATA()
	if (format == 0)
	{ format = context_data->default_pixelformat; }
	format--;

	if (context_data->pixelformats[format].dwFlags & PFD_SUPPORT_OPENGL)
		return 1;
	else
		return 0;
}
int lb_opengl_context_get_pixelformat_rgba( void* con,
                                            int format )
{
	DEFINE_CONTEXT_AND_DATA()
	if (format == 0)
	{ format = context_data->default_pixelformat; }
	format--;

	if (context_data->pixelformats[format].iPixelType == PFD_TYPE_RGBA)
		return 1;
	else
		return 0;
}
int lb_opengl_context_get_pixelformat_depth_size( void* con,
                                                  int format )
{
	DEFINE_CONTEXT_AND_DATA()
	if (format == 0)
	{ format = context_data->default_pixelformat; }
	format--;

	return context_data->pixelformats[format].cDepthBits;
}

static int get_default_pixelformat( void* con )
{
	int i, size;
	DEFINE_CONTEXT_AND_DATA()

	size = lb_opengl_context_pixelformats( context );
	/* iterate over every pixelformat except the default one (0) */
	for (i = 1; i < size; i++)
	{
		if (lb_opengl_context_get_pixelformat_support_opengl( context, i ) == 1)
		{
			if (lb_opengl_context_get_pixelformat_rgba( context, i ) == 1)
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

void lb_opengl_context_set_pixelformat( void* con, int format )
{
	BOOL result;
	DEFINE_CONTEXT_AND_DATA()

	if (format == 0)
	{
		format = context_data->default_pixelformat;
	}

	format--;

	result = SetPixelFormat( context->device_context,
							 format + 1,
							 &context_data->pixelformats[format] );
	assert( result );

	if (context_data->use_current)
	{
	    context->render_context = wglGetCurrentContext();
	}
	else
	{
    	context->render_context = wglCreateContext( context->device_context );
	}

	assert( context->render_context );
}

void lb_opengl_context_make_current( void* con )
{
	BOOL result;
	DEFINE_CONTEXT_AND_DATA()

	result = wglMakeCurrent( context->device_context,
							 context->render_context );
	assert( result );
}

void lb_opengl_context_release_current( void* con )
{
	BOOL result;
	DEFINE_CONTEXT_AND_DATA()

	/* TODO: This releases the device context as well, how will this be
	   handled? */
	result = wglMakeCurrent( context->device_context,
							 NULL );
	assert( result );
}

typedef BOOL (*WGLSWAPINTERVALEXTPROC)( int );
void lb_opengl_context_set_swap_interval( void* con, int interval )
{
	WGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
	DEFINE_CONTEXT_AND_DATA()
	wglSwapIntervalEXT = (WGLSWAPINTERVALEXTPROC)wglGetProcAddress( "wglSwapIntervalEXT" );
	if (wglSwapIntervalEXT)
	{
		wglSwapIntervalEXT( interval );
	}
}
