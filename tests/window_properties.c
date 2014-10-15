
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include <stdio.h>
#include <string.h>
#include <limbus/display.h>
#include <limbus/window.h>

#include "common.h"

int main( int argc, char** argv )
{
	LBWindow window;
	
	window = lb_window_construct();
	ASSERT( lb_window_constructed( window ) == 1, "Window could not be constructed" );
	lb_window_set_x( window, 123 );
	lb_window_set_y( window, 321 );
	lb_window_set_width( window, 456 );
	lb_window_set_height( window, 654 );
	lb_window_set_caption( window, "123456789" );
	
	ASSERT( lb_window_get_x( window ) == 123, "Window's x property was not set properly" );
	ASSERT( lb_window_get_y( window ) == 321, "Window's y property was not set properly" );
	ASSERT( lb_window_get_width( window ) == 456, "Window's width property was not set properly" );
	ASSERT( lb_window_get_height( window ) == 654, "Window's height property was not set properly" );
	ASSERT( strcmp( lb_window_get_caption( window ), "123456789" ) == 0 , "Window's caption was not set properly" );

	window = lb_window_destruct( window );
	ASSERT( lb_window_constructed( window ) == 0, "Window was not destructed properly" );
	
	return test_result;
}

