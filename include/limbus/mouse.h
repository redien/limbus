
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MOUSE_H
#define MOUSE_H
#ifdef __cplusplus
extern "C"
{
#endif

typedef void* LBMouse;

/** An enumeration of the different mouse event types. */
enum LBMouseEvent
{
    /** Event sent when the mouse has changed positon. */
	LBMouseEventMotion,
    /** Event sent when a button is pressed. */
	LBMouseEventButtonPress,
    /** Event sent when a button is released. */
	LBMouseEventButtonRelease
};
typedef enum LBMouseEvent LBMouseEvent;

/** An enumeration of the different mouse buttons. */
enum LBMouseButton
{
	LBMouseButtonLeft,
	LBMouseButtonRight,
	LBMouseButtonMiddle
};
typedef enum LBMouseButton LBMouseButton;

/** Constructs a new mouse object.
  * @return a pointer to the newly created mouse object if successful,
  * else returns a pointer identifying a non-constructed mouse object.
  * Use lb_mouse_constructed to find out if a mouse object was
  * constructed or not.
  */
LBMouse lb_mouse_construct( void );

/** Destructs a previously constructed mouse object.
  * Destructing a mouse object will deallocate all the resources
  * associated with it.
  * @param mouse a pointer to the mouse object to destruct.
  */
void lb_mouse_destruct( LBMouse mouse );

/** Check for successfully constructed mouse objects.
  * @param mouse a pointer returned by lb_mouse_construct
  * or lb_mouse_destruct
  * @return 1 if mouse points to a successfully constructed
  * mouse object or 0 if it doesn't.
  */
int lb_mouse_constructed( LBMouse mouse );

/** Set the x-coordinate of the mouse's position.
  * @param mouse a pointer to a successfully constructed mouse object.
  * @param x the new x-coordinate measured in pixels from the left side of
  * the window to the mouse's position.
  */
void lb_mouse_set_x( LBMouse mouse, int x );

/** Set the y-coordinate of the mouse's position.
  * @param mouse a pointer to a successfully constructed mouse object.
  * @param y the new y-coordinate measured in pixels from the top of
  * the window to the mouse's position.
  */
void lb_mouse_set_y( LBMouse mouse, int y );

/** Get the x-coordinate of the mouse's position.
  * @param mouse a pointer to a successfully constructed mouse object.
  * @return the x-coordinate measured in pixels from the left side of
  * the window to the mouse's position.
  */
int lb_mouse_get_x( LBMouse mouse );

/** Get the y-coordinate of the mouse's position.
  * @param mouse a pointer to a successfully constructed mouse object.
  * @return the y-coordinate measured in pixels from the top of
  * the window to the mouse's position.
  */
int lb_mouse_get_y( LBMouse mouse );

/** Shows/hides the mouse cursor.
  * @param mouse a pointer to a successfully constructed mouse object.
  * @param state 1 shows and 0 hides the mouse cursor.
  */
void lb_mouse_set_visibility( LBMouse mouse, int state );

/** Get the visibility state of the mouse cursor.
  * @param mouse a pointer to a successfully constructed mouse object.
  * @return 1 if the mouse is visible and 0 if it's not.
  */
int lb_mouse_get_visibility( LBMouse mouse );

/** Advances to the next event in the event stream if there is one.
  * @param mouse a pointer to a successfully constructed mouse object.
  * @return 1 if a new event is available else returns 0.
  */
int lb_mouse_next_event( LBMouse mouse );

/** Get the type of the current event.
  * @param mouse a pointer to a successfully constructed mouse object.
  * @return the type of the current event.
  */
LBMouseEvent lb_mouse_get_event_type( LBMouse mouse );

/** Get the button associated with the current event.
  * @param mouse a pointer to a successfully constructed mouse object.
  * @return on an LBMouseEventButtonPress or LBMouseEventButtonRelease event,
  * returns the button being pressed or released.
  */
LBMouseButton lb_mouse_get_event_button( LBMouse mouse );

/** Get the x-coordinate associated with the current event.
  * @param mouse a pointer to a successfully constructed mouse object.
  * @return on an LBMouseEventButtonPress or LBMouseEventButtonRelease event, returns the x-coordinate of
  * the position where the button was pressed/released.
  * On an LBMouseEventMotion event, returns the x-coordinate of the new mouse position.
  */
int lb_mouse_get_event_x( LBMouse mouse );

/** Get the y-coordinate associated with the current event.
  * @param mouse a pointer to a successfully constructed mouse object.
  * @return on an LBMouseEventButtonPress or LBMouseEventButtonRelease event, returns the y-coordinate of
  * the position where the button was pressed/released.
  * On an LBMouseEventMotion event, returns the y-coordinate of the new mouse position.
  */
int lb_mouse_get_event_y( LBMouse mouse );

#ifdef __cplusplus
}
#endif
#endif

