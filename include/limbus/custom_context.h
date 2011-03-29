
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef LB_CUSTOM_CONTEXT_H
#define LB_CUSTOM_CONTEXT_H
#ifdef __cplusplus
extern "C"
{
#endif

#include <limbus/window.h>

#ifdef _WIN32
	#include <windows.h>
	typedef struct
	{
		HWND handle;
	} LBWindowData;
#endif

LBWindowData* lb_custom_context_create_window( LBWindow window );
void lb_custom_context_bind( LBWindow window, LBWindowData* window_data );

#ifdef __cplusplus
}
#endif
#endif
