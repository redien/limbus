
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include <limbus/screen.h>

#include "../x11_screen.h"

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <stdlib.h>
#include <assert.h>

typedef struct _X11ScreenImpl
{
    X11Screen base;

    int mode_count;
    int previous_mode,
        current_mode;
    XRRScreenSize* modes;
    XRRScreenConfiguration* screen_config;
    Rotation current_rotation;
} X11ScreenImpl;

void* lb_screen_construct( int screen_id )
{
	X11Screen* screen;
	X11ScreenImpl* screen_impl;
	int default_id;
	
	screen_impl = (X11ScreenImpl*)malloc( sizeof *screen_impl );
	if (!screen_impl)
		return 0;
	
	screen = &screen_impl->base;

	screen->display = XOpenDisplay( 0 );
	if (!screen->display)
	{
		free( screen );
		return 0;
	}

	default_id = XDefaultScreen( screen->display );
	if (screen_id == 0)
		screen->screen = default_id;	
	else if (screen_id == default_id)
		screen->screen = 0;
	else
		screen->screen = screen_id;

    {
        int not_used;

        screen_impl->mode_count = 0;

        if (XQueryExtension( screen->display, "RANDR",
                             &not_used, &not_used, &not_used ))
        {
            screen_impl->screen_config = XRRGetScreenInfo(
                screen->display,
                XRootWindow( screen->display,
                             screen->screen )
            );

            if (screen_impl->screen_config)
            {
                screen_impl->previous_mode = XRRConfigCurrentConfiguration(
                    screen_impl->screen_config,
                    &screen_impl->current_rotation
                );
                
                screen_impl->current_mode = screen_impl->previous_mode;

                screen_impl->modes = XRRConfigSizes( screen_impl->screen_config,
                                                     &screen_impl->mode_count );
            }
        }
    }

	return screen_impl;
}

#define DECLARE_SCREEN()\
	X11ScreenImpl* screen_impl = (X11ScreenImpl*)scr;\
	X11Screen* screen = (X11Screen*)&screen_impl->base;\
	screen = screen;\
	screen_impl = screen_impl;\
	assert( screen_impl );

void* lb_screen_destruct( void* scr )
{
    DECLARE_SCREEN()

    {
        if (screen_impl->current_mode != screen_impl->previous_mode)
            lb_screen_change_mode( scr, screen_impl->previous_mode );

        if (screen_impl->screen_config)
            XRRFreeScreenConfigInfo( screen_impl->screen_config );
    }

	XCloseDisplay( screen->display );
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
    DECLARE_SCREEN()
	return XDisplayWidth( screen->display, screen->screen );
}

int lb_screen_get_height( void* scr )
{
    DECLARE_SCREEN()
	return XDisplayHeight( screen->display, screen->screen );
}

int lb_screen_modes( void* scr )
{
    DECLARE_SCREEN()
    return screen_impl->mode_count;
}

int lb_screen_get_mode_width( void* scr, int mode )
{
    DECLARE_SCREEN()
    return screen_impl->modes[mode].width;
}

int lb_screen_get_mode_height( void* scr, int mode )
{
    DECLARE_SCREEN()
    return screen_impl->modes[mode].height;
}

int lb_screen_change_mode( void* scr, int mode )
{
    DECLARE_SCREEN()
    XRRSetScreenConfig( screen->display,
                        screen_impl->screen_config,
                        RootWindow( screen->display, screen->screen ),
                        mode,
                        screen_impl->current_rotation,
                        CurrentTime );

    screen_impl->current_mode = mode;
    return 1;
}

