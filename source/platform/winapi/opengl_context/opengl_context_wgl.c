
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include "../../opengl_context.h"
#include "../../opengl_ext.h"
#include "../wgl_opengl_context.h"
#include "../win_window.h"

#include <assert.h>

typedef struct
{
	PIXELFORMATDESCRIPTOR* pixelformats;
	int number_of_pixelformats;
	int default_pixelformat;
	WinWindow* window;
	int released;
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

void* opengl_context_construct_in_window( void* win )
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

void* opengl_context_destruct( void* con )
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

void opengl_context_release( void* con )
{
	DEFINE_CONTEXT_AND_DATA()
	context_data->released = 1;
}

int opengl_context_constructed( void* con )
{
	if (con != NULL)
		return 1;
	else
		return 0;
}

void opengl_context_swap_buffers( void* con )
{
	DEFINE_CONTEXT_AND_DATA()
	
	SwapBuffers( context->device_context );
}

int opengl_context_pixelformats( void* con )
{
	DEFINE_CONTEXT_AND_DATA()
	return context_data->number_of_pixelformats + 1;
}

int opengl_context_get_attribute( void* con, int format, enum OpenglAttribute attribute )
{
	DEFINE_CONTEXT_AND_DATA()
	
	if (format == 0)
		format = context_data->default_pixelformat;

	format--;
	
	switch (attribute)
	{
		case OpenglAttributeDoubleBuffer:
			if (context_data->pixelformats[format].dwFlags & PFD_DOUBLEBUFFER)
				return 1;
			else
				return 0;

		case OpenglAttributeOpenglSupport:
			if (context_data->pixelformats[format].dwFlags & PFD_SUPPORT_OPENGL)
				return 1;
			else
				return 0;

		case OpenglAttributeRGBA:
			if (context_data->pixelformats[format].iPixelType == PFD_TYPE_RGBA)
				return 1;
			else
				return 0;

		case OpenglAttributeDepthSize:
			return context_data->pixelformats[format].cDepthBits;

		default:
			assert( 0 && "Invalid pixelformat attribute" );
			return 0;
	}
}

static int get_default_pixelformat( void* con )
{
	int i, size;
	DEFINE_CONTEXT_AND_DATA()

	size = opengl_context_pixelformats( context );
	/* iterate over every pixelformat except the default one (0) */
	for (i = 1; i < size; i++)
	{
		if (opengl_context_get_attribute( context, i, OpenglAttributeOpenglSupport ) == 1)
		{
			if (opengl_context_get_attribute( context, i, OpenglAttributeRGBA ) == 1)
			{
				if (opengl_context_get_attribute( context, i, OpenglAttributeDoubleBuffer ) == 1)
				{
					if (opengl_context_get_attribute( context, i, OpenglAttributeDepthSize ) > 0)
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

void opengl_context_set_pixelformat( void* con, int format )
{
	BOOL result;
	DEFINE_CONTEXT_AND_DATA()
	
	if (format == 0)
		format = context_data->default_pixelformat;

	format--;
	
	result = SetPixelFormat( context->device_context,
							 format + 1,
							 &context_data->pixelformats[format] );
	assert( result );
	
	context->render_context = wglCreateContext( context->device_context );
	assert( context->render_context );
}

void opengl_context_make_current( void* con )
{
	BOOL result;
	DEFINE_CONTEXT_AND_DATA()

	result = wglMakeCurrent( context->device_context,
							 context->render_context );
	assert( result );
}

void opengl_context_release_current( void* con )
{
	BOOL result;
	DEFINE_CONTEXT_AND_DATA()

	/* TODO: This releases the device context as well, how will this be
	   handled? */
	result = wglMakeCurrent( context->device_context,
							 NULL );
	assert( result );
}

