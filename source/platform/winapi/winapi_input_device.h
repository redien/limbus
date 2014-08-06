
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef WINAPI_INPUT_DEVICE_H
#define WINAPI_INPUT_DEVICE_H

#include <windows.h>

typedef struct WinAPIInputDeviceTag
{
	HWND handle;

	void (*construct)( void* device );
	int (*handle_winapi_message)( void* device, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* result );
} WinAPIInputDevice;

#endif

