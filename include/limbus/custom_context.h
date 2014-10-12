
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
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

/** Creates the specified window and returns the platform-dependent window data.
  * @param window a pointer to a window object created with lb_window_construct.
  * @return a pointer to the platform-dependent window data that may be used to create a
  * custom context.
  */
LBWindowData* lb_custom_context_create_window( LBWindow window );

#ifdef __cplusplus
}
#endif
#endif
