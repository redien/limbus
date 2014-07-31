
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SDL_OPENGL_CONTEXT_H
#define SDL_OPENGL_CONTEXT_H

#include <SDL.h>
#include "sdl_window.h"

typedef struct
{
    SDLWindow* window;
    SDL_GLContext context;
} SDLOpenglContext;

#endif

