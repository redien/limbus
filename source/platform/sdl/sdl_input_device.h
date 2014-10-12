
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
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

