
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include "../../../util/vector.h"
#include "../sdl_input_device.h"

#include <limbus/mouse.h>

#include <stdlib.h>
#include <assert.h>

typedef struct
{
	int type;
	int button;
	int x, y;
} MouseEvent;

typedef struct
{
	SDLInputDevice base;
	Vector events;
	MouseEvent event;
    
    int visible;
    int added_to_window;
    
    SDL_Cursor* no_cursor;
    SDL_Cursor* arrow_cursor;
} SDLMouse;

static void construct( void* m )
{
	SDLMouse* mouse = (SDLMouse*)m;
	lb_mouse_set_visibility( mouse, mouse->visible );
    
    mouse->arrow_cursor = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_ARROW );
    mouse->no_cursor = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_NO );
    
	mouse->added_to_window = 1;
}

#define CONVERT_BUTTON( button ) (button == SDL_BUTTON_LEFT)   ? LBMouseButtonLeft :\
								((button == SDL_BUTTON_RIGHT)  ? LBMouseButtonRight :\
								((button == SDL_BUTTON_MIDDLE) ? LBMouseButtonMiddle : -1))

static void handle_sdl_event( void* m, SDL_Event* event )
{
	SDLMouse* mouse = (SDLMouse*)m;

	if (event->type == SDL_MOUSEMOTION)
	{
		MouseEvent mouse_event;
		mouse_event.type = LBMouseEventMotion;
		mouse_event.x = event->motion.x;
		mouse_event.y = event->motion.y;
		
		vector_push_back( &mouse->events, &mouse_event );
	}

	if (event->type == SDL_MOUSEBUTTONDOWN)
	{
		MouseEvent mouse_event;
		mouse_event.type = LBMouseEventButtonPress;
		mouse_event.button = CONVERT_BUTTON( event->button.button );
		mouse_event.x = event->button.x;
		mouse_event.y = event->button.x;

		vector_push_back( &mouse->events, &mouse_event );
	}

	if (event->type == SDL_MOUSEBUTTONUP)
	{
		MouseEvent mouse_event;
		mouse_event.type = LBMouseEventButtonRelease;
		mouse_event.button = CONVERT_BUTTON( event->button.button );
		mouse_event.x = event->button.x;
		mouse_event.y = event->button.x;

		vector_push_back( &mouse->events, &mouse_event );
	}
}

void* lb_mouse_construct()
{
	SDLMouse* mouse = (SDLMouse*)malloc( sizeof( SDLMouse ) );
	vector_construct( &mouse->events, sizeof( MouseEvent ) );

	mouse->added_to_window = 0;
	mouse->visible = 1;

	mouse->base.construct = &construct;
	mouse->base.handle_sdl_event = &handle_sdl_event;
	
	return mouse;
}

void lb_mouse_destruct( void* m )
{
	SDLMouse* mouse = (SDLMouse*)m;
	vector_destruct( &mouse->events );
	free( mouse );
}

int lb_mouse_constructed( void* mouse )
{
	return (mouse != NULL) ? 1 : 0;
}

void lb_mouse_set_x( void* m, int x )
{
	SDLMouse* mouse = (SDLMouse*)m;
	SDL_WarpMouseInWindow( mouse->base.window, x, lb_mouse_get_y( mouse ) );
}

void lb_mouse_set_y( void* m, int y )
{
	SDLMouse* mouse = (SDLMouse*)m;
	SDL_WarpMouseInWindow( mouse->base.window, lb_mouse_get_x( mouse ), y );
}

int lb_mouse_get_x( void* m )
{
	SDLMouse* mouse = (SDLMouse*)m;
	int x;
    SDL_GetMouseState(&x, NULL);
	return x;
}

int lb_mouse_get_y( void* m )
{
	SDLMouse* mouse = (SDLMouse*)m;
	int y;
    SDL_GetMouseState(NULL, &y);
	return y;
}

void lb_mouse_set_visibility( void* m, int state )
{
	SDLMouse* mouse = (SDLMouse*)m;

	if (mouse->added_to_window)
	{
		SDL_SetCursor( state ? mouse->arrow_cursor : mouse->no_cursor );
	}
	
	mouse->visible = state;
}

int lb_mouse_get_visibility( void* m )
{
	SDLMouse* mouse = (SDLMouse*)m;
	return mouse->visible;
}

int lb_mouse_next_event( void* m )
{
	MouseEvent* iter;
	SDLMouse* mouse = (SDLMouse*)m;

	iter = (MouseEvent*)vector_next( &mouse->events, NULL );
	if (iter != NULL)
	{
		mouse->event = *iter;
		vector_erase( &mouse->events, iter );
		return 1;
	}
	else
	{
		return 0;
	}
}

enum LBMouseEvent lb_mouse_get_event_type( void* m )
{
	SDLMouse* mouse = (SDLMouse*)m;
	return mouse->event.type;
}

LBMouseButton lb_mouse_get_event_button( void* m )
{
	SDLMouse* mouse = (SDLMouse*)m;
	return mouse->event.button;
}

int lb_mouse_get_event_x( void* m )
{
	SDLMouse* mouse = (SDLMouse*)m;
	return mouse->event.x;
}

int lb_mouse_get_event_y( void* m )
{
	SDLMouse* mouse = (SDLMouse*)m;
	return mouse->event.y;
}

