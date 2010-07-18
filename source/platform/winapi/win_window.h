
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef _WIN_WINDOW_H_
#define _WIN_WINDOW_H_

#include <windows.h>

typedef struct
{
	HWND handle;
	WNDCLASSEX window_class;
	ATOM class_atom;
	HCURSOR cursor;
	void (*construct_impl)( void* window );
	void* impl_data;
} WinWindow;

#endif
