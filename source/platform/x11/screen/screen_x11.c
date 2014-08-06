
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

#include "../x11_screen.h"

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <stdlib.h>
#include <assert.h>

typedef struct _X11ScreenImpl
{
    X11Screen base;

    int config_at_construction,
        current_config;
    int size_count;
    XRRScreenSize* sizes;
    XRRScreenConfiguration* screen_config;
    Rotation current_rotation;
} X11ScreenImpl;

void* lb_screen_construct( int screen_id )
{
	X11ScreenImpl* screen;
	int default_id;
	
	screen = (X11ScreenImpl*)malloc( sizeof *screen );
	if (!screen)
		return NULL;

	screen->base.display = XOpenDisplay( 0 );
	if (!screen->base.display)
	{
		free( screen );
		return NULL;
	}

    /* Make sure we can assign every screen id, even
       if LBScreenDefault is not the system's default */
	default_id = XDefaultScreen( screen->base.display );
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

    /* Get information on the available screen modes */
    {
        int not_used;

        screen->size_count = 0;
        screen->sizes = NULL;
        screen->screen_config = NULL;

        if (XQueryExtension( screen->base.display, "RANDR",
                             &not_used, &not_used, &not_used ))
        {
            screen->screen_config = XRRGetScreenInfo(
                screen->base.display,
                XRootWindow( screen->base.display,
                             screen->base.screen )
            );

            if (screen->screen_config)
            {
                screen->current_config = XRRConfigCurrentConfiguration(
                    screen->screen_config,
                    &screen->current_rotation
                );

                screen->config_at_construction = screen->current_config;

                screen->sizes = XRRConfigSizes( screen->screen_config,
                                                &screen->size_count );
            }
        }
    }

	return screen;
}

void* lb_screen_destruct( void* scr )
{
	X11ScreenImpl* screen = (X11ScreenImpl*)scr;
	assert( screen );

    if (screen->sizes && screen->current_config != screen->config_at_construction)
    {
        lb_screen_change_mode( screen, screen->config_at_construction );
    }

    if (screen->screen_config)
    {
        XRRFreeScreenConfigInfo( screen->screen_config );
    }

	XCloseDisplay( screen->base.display );
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
	X11ScreenImpl* screen = (X11ScreenImpl*)scr;
	assert( screen );
	/* We don't want these to depend on any extensions */
	return XDisplayWidth( screen->base.display, screen->base.screen );
}

int lb_screen_get_height( void* scr )
{
	X11ScreenImpl* screen = (X11ScreenImpl*)scr;
	assert( screen );
	return XDisplayHeight( screen->base.display, screen->base.screen );
}

int lb_screen_modes( void* scr )
{
	X11ScreenImpl* screen = (X11ScreenImpl*)scr;
	assert( screen );
    return screen->size_count;
}

int lb_screen_get_mode_width( void* scr, int mode )
{
	X11ScreenImpl* screen = (X11ScreenImpl*)scr;
	assert( screen );
	/* If RANDR is not availiable or an error occured,
	   screen->size_count should be 0 */
	assert( screen->sizes );
    return screen->sizes[mode].width;
}

int lb_screen_get_mode_height( void* scr, int mode )
{
	X11ScreenImpl* screen = (X11ScreenImpl*)scr;
	assert( screen );
	assert( screen->sizes );
    return screen->sizes[mode].height;
}

enum LBScreenError lb_screen_change_mode( void* scr, int mode )
{
    int error;
	X11ScreenImpl* screen = (X11ScreenImpl*)scr;
	assert( screen );
	assert( screen->sizes );

    error = XRRSetScreenConfig( screen->base.display,
                                screen->screen_config,
                                RootWindow( screen->base.display,
                                            screen->base.screen ),
                                mode,
                                screen->current_rotation,
                                CurrentTime );

    if (error == BadValue)
        return LBScreenInvalidMode;

    screen->current_config = mode;
    return LBScreenNoError;
}

