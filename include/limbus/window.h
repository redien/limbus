
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef _LB_WINDOW_H_
#define _LB_WINDOW_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include "screen.h"

typedef void* LBWindow;

/* TODO: Figure out a way to use the LBScreen typedef in lb_window_construct */

/** An enumeration of the different window event types. */
enum LBWindowEvent
{
    /** Event sent when the window has been closed by the user. */
	LBWindowEventClose,
	/** Event sent when a file is dragged to the window. */
	LBWindowEventFileDrop
};

/** Constructs a new window object.
  * Simply calling this function might not create an actual window.
  * On most platforms a graphics context is needed before the
  * window can be fully created.
  * @param screen a pointer to a screen object created with
  * lb_screen_construct. The window object does not take ownership
  * of the screen object pointed to by screen.
  * @return a pointer to the newly created window object if successful,
  * else returns a pointer identifying a non-constructed window object.
  * Use lb_window_constructed to find out if a window object was
  * constructed or not.
  */
LBWindow lb_window_construct( void* screen );

/** Destructs a previously constructed window object.
  * Destructing a window object will deallocate all the resources
  * associated with it.
  * @param window a pointer to the window object to destruct.
  * @return a new pointer that can be passed to lb_window_constructed
  * to find out if the window object was destructed or not.
  */
LBWindow lb_window_destruct( LBWindow window );

/** Check for successfully constructed window objects.
  * @param window a pointer returned by lb_window_construct
  * or lb_window_destruct
  * @return 1 if window points to a successfully constructed
  * window object or 0 if it doesn't.
  */
int lb_window_constructed( LBWindow window );


/** Enables all window decorations.
  * All window decorations are enabled by default.
  * @param window a pointer to a successfully constructed window object.
  */
void lb_window_enable_decorations( LBWindow window );

/** Disables all window decorations.
  * All window decorations are enabled by default.
  * @param window a pointer to a successfully constructed window object.
  */
void lb_window_disable_decorations( LBWindow window );


/** Set the caption of the window.
  * @param window a pointer to a successfully constructed window object.
  * @param caption the new caption as a zero-terminated UTF-8 string.
  */
void lb_window_set_caption( LBWindow window, const char* caption );

/** Set the width of the window.
  * @param window a pointer to a successfully constructed window object.
  * @param width the new width in pixels.
  */
void lb_window_set_width( LBWindow window, int width );

/** Set the height of the window.
  * @param window a pointer to a successfully constructed window object.
  * @param height the new height in pixels.
  */
void lb_window_set_height( LBWindow window, int height );

/** Set the x-coordinate of the window's position.
  * @param window a pointer to a successfully constructed window object.
  * @param x the new x-coordinate measured in pixels from the left side of
  * the screen to the left side of the window.
  */
void lb_window_set_x( LBWindow window, int x );

/** Set the y-coordinate of the window's position.
  * @param window a pointer to a successfully constructed window object.
  * @param y the new y-coordinate measured in pixels from the top side of
  * the screen to the top side of the window.
  */
void lb_window_set_y( LBWindow window, int y );

/** Get the caption of the window.
  * @param window a pointer to a successfully constructed window object.
  * @return the caption as a zero-terminated UTF-8 string.
  */
const char* lb_window_get_caption( LBWindow window );

/** Get the width of the window.
  * @param window a pointer to a successfully constructed window object.
  * @return the width in pixels.
  */
int lb_window_get_width( LBWindow window );

/** Get the height of the window.
  * @param window a pointer to a successfully constructed window object.
  * @return the height in pixels.
  */
int lb_window_get_height( LBWindow window );

/** Get the x-coordinate of the window's position.
  * @param window a pointer to a successfully constructed window object.
  * @return the x-coordinate measured in pixels from the left side of
  * the screen to the left side of the window.
  */
int lb_window_get_x( LBWindow window );

/** Get the y-coordinate of the window's position.
  * @param window a pointer to a successfully constructed window object.
  * @return the y-coordinate measured in pixels from the top side of
  * the screen to the top side of the window.
  */
int lb_window_get_y( LBWindow window );


/** Advances to the next event in the event stream if there is one.
  * @param window a pointer to a successfully constructed window object.
  * @return 1 if a new event is available else returns 0.
  */
int lb_window_next_event( LBWindow window );

/** Get the type of the current event.
  * @param window a pointer to a successfully constructed window object.
  * @return the type of the current event.
  */
enum LBWindowEvent lb_window_get_event_type( LBWindow window );

/** Get the x-coordinate of the current event.
  * @param window a pointer to a successfully constructed window object.
  * @return on an LBWindowEventFileDrop event, returns the x-coordinate of
  * the position where the file was dropped.
  */
int lb_window_get_event_x( LBWindow window );

/** Get the y-coordinate of the current event.
  * @param window a pointer to a successfully constructed window object.
  * @return on an LBWindowEventFileDrop event, returns the y-coordinate of
  * the position where the file was dropped.
  */
int lb_window_get_event_y( LBWindow window );

/** Get the number of files associated with an LBWindowEventFileDrop event.
  * @param window a pointer to a successfully constructed window object.
  * @return on an LBWindowEventFileDrop event, returns the number of
  * files dropped.
  */
int lb_window_get_event_files( LBWindow window );

/** Get a file associated with an LBWindowEventFileDrop event.
  * @param window a pointer to a successfully constructed window object.
  * @param file an integer identifying the file to be returned.
  * Valid values range from 0 to
  * lb_window_get_event_files( window ) - 1, inclusive.
  * @return a path to the file identified by file.
  * The path is returned as a zero-terminated string.
  * The encoding of the path returned depends on the platform.
  */
const char* lb_window_get_event_file( LBWindow window, int file );

void lb_window_add_input_device( LBWindow window, void* device );

#ifdef __cplusplus
}
#endif
#endif

