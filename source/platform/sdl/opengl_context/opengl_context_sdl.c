
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include "../sdl_opengl_context.h"
#include "../sdl_window.h"
#include "../sdl_screen.h"

#include <limbus/opengl_context.h>
#include <limbus/opengl.h>

#include <mach-o/dyld.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef struct
{
    SDLOpenglContext base;
	int created;
	int use_current;
    int released;
    int default_pixelformat;
} SDLOpenglContextImpl;

static void create_opengl_context_impl( SDLOpenglContextImpl* context,
										int pixelformat );
static void destroy_opengl_context_impl( SDLOpenglContextImpl* context );

void* lb_opengl_context_construct( void* win, int use_current )
{
	SDLWindow* window;
	SDLOpenglContextImpl* context;
	
	window = (SDLWindow*)win;
	assert( window );
	
	context = (SDLOpenglContextImpl*)malloc( sizeof( *context ) );
	assert( context != NULL );

	context->base.window = window;
	context->created = 0;
    context->released = 1;
	context->use_current = use_current;

	context->default_pixelformat = 1 * 3 /* double buffering */
                                 + 1 /* 24 bit depth buffer */
                                 + 1;

	return context;
}

void* lb_opengl_context_destruct( void* con )
{
    SDLOpenglContextImpl* context = (SDLOpenglContextImpl*)con;

	if (context->created && !context->released)
		destroy_opengl_context_impl( context );

	free( context );
	
	return NULL;
}

void lb_opengl_context_release_ownership( void* con )
{
    SDLOpenglContextImpl* context = (SDLOpenglContextImpl*)con;
	context->released = 1;
}

int lb_opengl_context_constructed( void* con )
{
	return con != NULL;
}

void lb_opengl_context_swap_buffers( void* con )
{
    SDLOpenglContextImpl* context = (SDLOpenglContextImpl*)con;
	SDL_GL_SwapWindow( context->base.window->window );
}

int lb_opengl_context_pixelformats( void* con )
{
    SDLOpenglContextImpl* context = (SDLOpenglContextImpl*)con;
	return 2 /* double buffering enable/disable */
         * 3 /* 16, 24, 32 bit depth buffer */;
}

int lb_opengl_context_get_pixelformat_double_buffer( void* con, int format )
{
    SDLOpenglContextImpl* context = (SDLOpenglContextImpl*)con;

    if (format == LBOpenglContextDefaultPixelformat)
        format = context->default_pixelformat;

    format--;

    return format >= 3;
}

int lb_opengl_context_get_pixelformat_support_opengl( void* con, int format )
{
    return 1;
}

int lb_opengl_context_get_pixelformat_depth_size( void* con, int format )
{
    SDLOpenglContextImpl* context = (SDLOpenglContextImpl*)con;

    if (format == LBOpenglContextDefaultPixelformat)
        format = context->default_pixelformat;

    format--;

    return 16 + (format % 3) * 8;
}

LBOpenglContextColorFormat lb_opengl_context_get_pixelformat_color_format( void* con, int format )
{
    return LBOpenglContextColorFormatRGBA;
}

void lb_opengl_context_bind( void* con, int format )
{
    SDLOpenglContextImpl* context = (SDLOpenglContextImpl*)con;
	
	if (format == LBOpenglContextDefaultPixelformat)
		format = context->default_pixelformat;

	format--;

	create_opengl_context_impl( context, format );
}

void lb_opengl_context_make_current( void* con )
{
    SDLOpenglContextImpl* context = (SDLOpenglContextImpl*)con;
    SDL_GL_MakeCurrent( context->base.window->window, context->base.context );
}

void lb_opengl_context_release_current( void* con )
{
    /* TODO: Figure out what to do here for SDL */
}

void create_opengl_context_impl( SDLOpenglContextImpl* context,
								 int format )
{
    if (context->use_current)
    {
        /* TODO: What do we do here? */
        context->base.window->create_window_impl( context->base.window, SDL_WINDOW_OPENGL );
        context->base.context = SDL_GL_GetCurrentContext();
    }
    else
    {
        SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, lb_opengl_context_get_pixelformat_double_buffer( context, format ) );
        SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, lb_opengl_context_get_pixelformat_depth_size( context, format ) );

        context->base.window->create_window_impl( context->base.window, SDL_WINDOW_OPENGL );
        context->base.context = SDL_GL_CreateContext( context->base.window->window );
    }

	context->created = 1;
}

void destroy_opengl_context_impl( SDLOpenglContextImpl* context )
{
	SDL_GL_DeleteContext( context->base.context );
    context->base.window->destroy_window_impl( context->base.window );
	context->created = 0;
}

int lb_opengl_context_set_swap_interval( void* con, int interval )
{
	return SDL_GL_SetSwapInterval(interval) != -1;
}

void* glGetProcAddress( const char* name )
{
    NSSymbol symbol;
    char *symbolName;
    symbolName = malloc (strlen (name) + 2); // 1
    strcpy(symbolName + 1, name); // 2
    symbolName[0] = '_'; // 3
    symbol = NULL;
    if (NSIsSymbolNameDefined (symbolName)) // 4
        symbol = NSLookupAndBindSymbol (symbolName);
    free (symbolName); // 5
    return symbol ? NSAddressOfSymbol (symbol) : NULL; // 6
}