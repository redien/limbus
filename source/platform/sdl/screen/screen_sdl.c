
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include <limbus/screen.h>
#include "../sdl_screen.h"

#include <SDL.h>
#include <stdlib.h>
#include <assert.h>

typedef struct SDLScreenImpl
{
    SDLScreen base;
} SDLScreenImpl;

void* lb_screen_construct( int screen_id )
{
	SDLScreenImpl* screen;
	int default_id;
	
	screen = (SDLScreenImpl*)malloc( sizeof *screen );
	if (!screen)
		return NULL;

    /* Make sure we can assign every screen id, even
       if LBScreenDefault is not the system's default */
	default_id = 0;
	if (screen_id == LBScreenDefault)
	{
		screen->base.screen = default_id;
	}
	else if (screen_id == default_id)
	{
		screen->base.screen = LBScreenDefault;
	}
	else
	{
		screen->base.screen = screen_id;
	}

	return screen;
}

void* lb_screen_destruct( void* scr )
{
	SDLScreenImpl* screen = (SDLScreenImpl*)scr;
	assert( screen );
	free( screen );
	
	return NULL;
}

int lb_screen_constructed( void* scr )
{
	if (scr != NULL)
		return 1;
	else
		return 0;
}

int lb_screen_get_width( void* scr )
{
	return 0;
}

int lb_screen_get_height( void* scr )
{
	return 0;
}

int lb_screen_modes( void* scr )
{
    return 0;
}

int lb_screen_get_mode_width( void* scr, int mode )
{
    return 0;
}

int lb_screen_get_mode_height( void* scr, int mode )
{
    return 0;
}

enum LBScreenError lb_screen_change_mode( void* scr, int mode )
{
    return LBScreenNoError;
}

