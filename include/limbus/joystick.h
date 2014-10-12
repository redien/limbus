
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef LB_JOYSTICK_H
#define LB_JOYSTICK_H
#ifdef __cplusplus
extern "C"
{
#endif

/* Experimental */

typedef void* LBJoystick;

/** An enumeration of the different joystick event types. */
enum LBJoystickEvent
{
    /** Event sent when a an axis value changed. */
	LBJoystickEventAxis,
    /** Event sent when a button was pressed/released. */
	LBJoystickEventButton,
    /** Event sent when the joystick was connected. */
	LBJoystickEventConnected,
    /** Event sent when the joystick was disconnected. */
	LBJoystickEventDisconnected
};
typedef enum LBJoystickEvent LBJoystickEvent;

/** Constructs a new joystick object.
  * @return a pointer to the newly created joystick object if successful,
  * else returns a pointer identifying a non-constructed joystick object.
  * Use lb_joystick_constructed to find out if an object was
  * constructed or not.
  */
LBJoystick lb_joystick_construct( int joystick_id );

/** Destructs a previously constructed joystick object.
  * Destructing a joystick object will deallocate all the resources
  * associated with it.
  * @param joystick a pointer to the joystick object to destruct.
  */
void lb_joystick_destruct( LBJoystick joystick );

/** Check for successfully constructed joystick objects.
  * @param joystick a pointer returned by lb_joystick_construct
  * @return 1 if joystick points to a successfully constructed
  * joystick object or 0 if it doesn't.
  */
int lb_joystick_constructed( LBJoystick joystick );

/** Check if the specified joystick is connected.
  * @param joystick a pointer returned by lb_joystick_construct
  * @return 1 if the specified joystick is connected, 0 otherwise.
  */
int lb_joystick_connected( LBJoystick joystick );

/** Returns a string that identifies the specific joystick product.
  * @param joystick a pointer returned by lb_joystick_construct
  * @return a string that identifies the specific joystick product.
  */
char* lb_joystick_get_product_string( LBJoystick joystick );

/** Returns the number of axes available on the joystick.
  * @param joystick a pointer returned by lb_joystick_construct
  * @return the number of axes available on the joystick.
  */
int lb_joystick_axes( LBJoystick joystick );

/** Returns the number of buttons available on the joystick.
  * @param joystick a pointer returned by lb_joystick_construct
  * @return the number of buttons available on the joystick.
  */
int lb_joystick_buttons( LBJoystick joystick );

void lb_joystick_set_axis_max( LBJoystick joystick, int axis, int value );
void lb_joystick_set_axis_min( LBJoystick joystick, int axis, int value );

/** Advances to the next event in the event stream if there is one.
  * @param joystick a pointer to a successfully constructed joystick object.
  * @return 1 if a new event is available else returns 0.
  */
int lb_joystick_next_event( LBJoystick joystick );

/** Advances to the next event in the event stream if there is one.
  * This returns event values that have not been calibrated.
  * @param joystick a pointer to a successfully constructed joystick object.
  * @return 1 if a new event is available else returns 0.
  */
int lb_joystick_next_raw_event( LBJoystick joystick );

/** Returns the type of the current event.
  * @param joystick a pointer to a successfully constructed joystick object.
  * @return the type of the current event.
  */
LBJoystickEvent lb_joystick_get_event_type( LBJoystick joystick );

/** Returns the number specifying which button/axis the event concerns.
  * @param joystick a pointer to a successfully constructed joystick object.
  * @return an integer specifying which button/axis the event concerns.
  */
int lb_joystick_get_event_number( LBJoystick joystick );

/** Returns the value of the current event.
  * For a button this is either 1 for pressed or 0 for released.
  * For an axis this is a value between min/max.
  * @param joystick a pointer to a successfully constructed joystick object.
  * @return an integer specifying which button/axis the event concerns.
  */
int lb_joystick_get_event_value( LBJoystick joystick );

/** Returns the time when the current event occured.
  * @param joystick a pointer to a successfully constructed joystick object.
  * @return an integer specifying the time when the current event occured.
  */
int lb_joystick_get_event_time( LBJoystick joystick );

#ifdef __cplusplus
}
#endif
#endif

