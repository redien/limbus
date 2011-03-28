
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef WIN_WINDOW_H
#define WIN_WINDOW_H

#include <windows.h>

typedef struct
{
	HWND handle;
	WNDCLASSEX window_class;
	ATOM class_atom;
	HCURSOR cursor;
	void (*construct_impl)( void* window );
} WinWindow;

#endif
