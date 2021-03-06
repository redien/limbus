
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
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
