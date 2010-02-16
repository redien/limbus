
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef _WINAPI_INPUT_DEVICE_H_
#define _WINAPI_INPUT_DEVICE_H_

#include <windows.h>

typedef struct WinAPIInputDeviceTag
{
	HWND handle;

	void (*construct)( void* device );
	int (*handle_winapi_message)( void* device, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* result );
} WinAPIInputDevice;

#endif

