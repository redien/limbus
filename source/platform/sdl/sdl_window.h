
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SDL_WINDOW_H
#define SDL_WINDOW_H

#include <SDL.h>
#include "sdl_screen.h"

typedef struct
{
    SDL_Window *window;
	SDLScreen* screen;
	void (*create_window_impl)(void*, Uint32 flags);
	void (*destroy_window_impl)(void*);
} SDLWindow;

#endif
