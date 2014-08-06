
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef X11_WINDOW_H
#define X11_WINDOW_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "x11_screen.h"

typedef struct
{
	X11Screen* screen;
	Window window;
	Window root_window;
	void (*create_window_impl)(void*, XVisualInfo*);
	void (*destroy_window_impl)(void*);
} X11Window;

#endif

