
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

#include <windows.h>
#include <stdlib.h>

typedef struct WinapiScreen_
{
	DISPLAY_DEVICE device;
	DEVMODE mode, mode_at_construction;
} WinapiScreen;

void* lb_screen_construct( int screen_num )
{
	WinapiScreen* self;
	DWORD default_device;
	self = (WinapiScreen*)malloc( sizeof *self );

	self->device.cb = sizeof self->device;

	/* Find default device */
	{
		DWORD i = 0;
		while (EnumDisplayDevices( NULL, i, &self->device, 0 ))
		{
			if (self->device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
			{
				default_device = i;
				break;
			}
			++i;
		}
	}

	if (screen_num == LBScreenDefault)
	{
		screen_num = default_device;
	}
	else if (screen_num == default_device)
	{
		screen_num = LBScreenDefault;
	}

	/* Fetch info from the specified screen */
	if (EnumDisplayDevices( NULL, screen_num, &self->device, 0 ))
	{
		self->mode.dmSize = sizeof self->mode;
		self->mode.dmDriverExtra = 0;
		if (EnumDisplaySettings( self->device.DeviceName, ENUM_CURRENT_SETTINGS, &self->mode ))
		{
			self->mode_at_construction = self->mode;
			return self;
		}
	}

	free( self );
	return NULL;
}

void* lb_screen_destruct( LBScreen screen )
{
	WinapiScreen* self = (WinapiScreen*)screen;

	if (self->mode.dmPelsWidth != self->mode_at_construction.dmPelsWidth
	 || self->mode.dmPelsHeight != self->mode_at_construction.dmPelsHeight
	 || self->mode.dmBitsPerPel != self->mode_at_construction.dmBitsPerPel
	 || self->mode.dmDisplayFrequency != self->mode_at_construction.dmDisplayFrequency)
	{
		ChangeDisplaySettingsEx( self->device.DeviceName,
								 &self->mode_at_construction,
								 NULL,
								 CDS_FULLSCREEN,
								 NULL );
	}

	free( screen );
	return NULL;
}

int lb_screen_constructed( LBScreen screen )
{
	return (screen != NULL) ? 1 : 0;
}

int lb_screen_get_width( LBScreen screen )
{
	WinapiScreen* self = (WinapiScreen*)screen;
	return self->mode.dmPelsWidth;
}

int lb_screen_get_height( LBScreen screen )
{
	WinapiScreen* self = (WinapiScreen*)screen;
	return self->mode.dmPelsHeight;
}

int lb_screen_modes( LBScreen screen )
{
	WinapiScreen* self = (WinapiScreen*)screen;
	DWORD count = 0;
	DEVMODE devmode;
	devmode.dmSize = sizeof devmode;
	devmode.dmDriverExtra = 0;

	while (EnumDisplaySettings( self->device.DeviceName, count, &devmode ))
	{ ++count; }

    return count;
}

int lb_screen_get_mode_width( LBScreen screen, int mode )
{
	WinapiScreen* self = (WinapiScreen*)screen;
	DEVMODE devmode;
	devmode.dmSize = sizeof devmode;
	devmode.dmDriverExtra = 0;

	/* Assume that this will work... */
	EnumDisplaySettings( self->device.DeviceName, mode, &devmode );

    return devmode.dmPelsWidth;
}

int lb_screen_get_mode_height( LBScreen screen, int mode )
{
	WinapiScreen* self = (WinapiScreen*)screen;
	DEVMODE devmode;
	devmode.dmSize = sizeof devmode;
	devmode.dmDriverExtra = 0;

	/* Assume that this will work... */
	EnumDisplaySettings( self->device.DeviceName, mode, &devmode );

    return devmode.dmPelsHeight;
}

LBScreenError lb_screen_change_mode( LBScreen screen, int mode )
{
	WinapiScreen* self = (WinapiScreen*)screen;
	LONG result;
	DEVMODE devmode;
	devmode.dmSize = sizeof devmode;
	devmode.dmDriverExtra = 0;

	if (!EnumDisplaySettings( self->device.DeviceName, mode, &devmode ))
	{
		return LBScreenInvalidMode;
	}

	result = ChangeDisplaySettingsEx( self->device.DeviceName,
									 &devmode,
									 NULL,
									 CDS_FULLSCREEN,
									 NULL );

	if (result == DISP_CHANGE_SUCCESSFUL)
	{
		self->mode = devmode;
		return LBScreenNoError;
	}

    return LBScreenInvalidMode;
}
