
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

/** An enumeration of the different window event types. */
enum LBWindowEvent
{
	LBWindowEventClose, /**< Event sent when the window has been closed by the user. */
	LBWindowEventFileDrop /**< Event sent when a file is dragged to the window. */
};

/** Constructs a new window object.
  * Simply calling this function might not create an actual window. On most platforms a graphics context is needed before the window can be fully created.
  * @param screen a pointer to a screen object created with lb_screen_construct. The window object does not take ownership of the screen object pointed to by screen.
  * @return a pointer to the newly created window object if successful, else returns a pointer identifying a non-constructed window object. Use lb_window_constructed to find out if a window object was constructed or not.
  */
void* lb_window_construct( void* screen );

/** Destructs a previously constructed window object.
  * Destructing a window object will deallocate all the resources associated with it.
  * @param window a pointer to the window object to destruct.
  * @return a new pointer that can be passed to lb_window_constructed to find out if the window object was destructed or not.
  */
void* lb_window_destruct( void* window );

/** Check for successfully constructed window objects.
 *  @param window a pointer returned by lb_window_construct or lb_window_destruct
 *  @return 1 if window points to a successfully constructed window object or 0 if it doesn't.
 */
int lb_window_constructed( void* window );

void lb_window_clear_decoration( void* window );


/** Set the caption of the window.
  * @param window a pointer to a successfully constructed window object.
  * @param caption the new caption as a zero-terminated string.
  */
void lb_window_set_caption( void* window, const char* caption );

/** Set the width of the window.
  * @param window a pointer to a successfully constructed window object.
  * @param width the new width in pixels.
  */
void lb_window_set_width( void* window, int width );

/** Set the height of the window.
  * @param window a pointer to a successfully constructed window object.
  * @param height the new height in pixels.
  */
void lb_window_set_height( void* window, int height );

/** Set the x-coordinate of the window's position.
  * @param window a pointer to a successfully constructed window object.
  * @param x the new x-coordinate measured in pixels from the left side of the screen.
  */
void lb_window_set_x( void* window, int x );

/** Set the y-coordinate of the window's position.
  * @param window a pointer to a successfully constructed window object.
  * @param y the new y-coordinate measured in pixels from the top side of the screen.
  */
void lb_window_set_y( void* window, int y );

/** Get the caption of the window.
  * @param window a pointer to a successfully constructed window object.
  * @return the caption as a zero-terminated string.
  */
const char* lb_window_get_caption( void* window );

/** Get the width of the window.
  * @param window a pointer to a successfully constructed window object.
  * @return the width in pixels.
  */
int lb_window_get_width( void* window );

/** Get the height of the window.
  * @param window a pointer to a successfully constructed window object.
  * @return the height in pixels.
  */
int lb_window_get_height( void* window );

/** Get the x-coordinate of the window's position.
  * @param window a pointer to a successfully constructed window object.
  * @return the x-coordinate measured in pixels from the left side of the screen.
  */
int lb_window_get_x( void* window );

/** Get the y-coordinate of the window's position.
  * @param window a pointer to a successfully constructed window object.
  * @return the y-coordinate measured in pixels from the top side of the screen.
  */
int lb_window_get_y( void* window );


/** Advances to the next event in the event stream if there is one.
  * @param window a pointer to a successfully constructed window object.
  * @return 1 if a new event is available else returns 0.
  */
int lb_window_next_event( void* window );

/** Get the type of the current event.
  * @param window a pointer to a successfully constructed window object.
  * @return the type of the current event.
  */
enum LBWindowEvent lb_window_get_event_type( void* window );

/** Get the x-coordinate of the current event.
  * @param window a pointer to a successfully constructed window object.
  * @return on an LBWindowEventFileDrop event, returns the x-coordinate of the position where the file was dropped.
  */
int lb_window_get_event_x( void* window );

/** Get the y-coordinate of the current event.
  * @param window a pointer to a successfully constructed window object.
  * @return on an LBWindowEventFileDrop event, returns the y-coordinate of the position where the file was dropped.
  */
int lb_window_get_event_y( void* window );

/** Get the number of files associated with an LBWindowEventFileDrop event.
  * @param window a pointer to a successfully constructed window object.
  * @return on an LBWindowEventFileDrop event, returns the number of files dropped.
  */
int lb_window_get_event_files( void* window );

/** Get a file associated with an LBWindowEventFileDrop event.
  * @param window a pointer to a successfully constructed window object.
  * @param file an integer identifying the file to be returned. Valid values range from 0 to lb_window_get_event_files( window ) - 1, inclusive.
  * @return a path to the file identified by file.
  */
const char* lb_window_get_event_file( void* window, int file );

void lb_window_add_input_device( void* window, void* device );

#ifdef __cplusplus
}
#endif
#endif

