
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SDL_INPUT_DEVICE_H
#define SDL_INPUT_DEVICE_H

#include <SDL.h>

typedef struct
{
	SDL_Window* window;

	void (*construct)( void* device );
	void (*handle_sdl_event)( void* device, SDL_Event* event );
	/* TODO: Add destruct callback */
} SDLInputDevice;

#endif

