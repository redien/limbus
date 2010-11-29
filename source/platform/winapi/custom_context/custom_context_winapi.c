
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include "../win_window.h"
#include <limbus/custom_context.h>

#include <assert.h>

LBWindowData* lb_custom_context_create_window( LBWindow win )
{
	WinWindow* window = (WinWindow*)win;
	window->construct_impl( window );
	return (LBWindowData*)window;
}

void lb_custom_context_bind( LBWindow win, LBWindowData* window_data )
{
}
