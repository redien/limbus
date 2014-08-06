
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
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
