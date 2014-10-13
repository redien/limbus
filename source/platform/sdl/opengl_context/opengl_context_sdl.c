
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
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
    symbolName = malloc (strlen (name) + 2);
    strcpy(symbolName + 1, name);
    symbolName[0] = '_';
    symbol = NULL;
    if (NSIsSymbolNameDefined (symbolName))
        symbol = NSLookupAndBindSymbol (symbolName);
    free (symbolName);
    return symbol ? NSAddressOfSymbol (symbol) : NULL;
}
