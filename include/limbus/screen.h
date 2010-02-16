
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef _LB_SCREEN_H_
#define _LB_SCREEN_H_
#ifdef __cplusplus
extern "C"
{
#endif

/**
 *  An integer identifying the default screen of the system.
 */
#define LBScreenDefault 0

/** Constructs a screen object for the given screen.
 *  LBScreenDefault refers to the system's default screen if there is one.
 *  @param lb_screen_id the id of the screen to construct an object for
 *  @return a pointer to the newly created screen object if successful, else returns a pointer identifying an unconstructed screen object. Use lb_screen_constructed to find out if a screen object was constructed or not.
 *  @see lb_screen_constructed
 *  @see lb_screen_destruct
 */
void* lb_screen_construct( int screen_id );

/** Destructs a previously constructed screen object.
 *  Destructing a screen object will deallocate all the resources associated with it. The returned pointer is used to check if an object was destructed or not.
 *  @param screen a pointer to the screen object to destruct.
 *  @see lb_screen_construct
 *  @see lb_screen_constructed
 *  @return a new pointer representing an unconstructed screen object.
 */
void* lb_screen_destruct( void* screen );

/** Returns 1 if screen points to a successfully constructed screen object or 0 if it doesn't.
 *  @param screen a pointer returned by lb_screen_construct or lb_screen_destruct
 *  @see lb_screen_construct
 *  @see lb_screen_destruct
 *  @return 1 if screen points to a successfully constructed screen object or 0 if it doesn't.
 */
int lb_screen_constructed( void* screen );

/** Returns the width of the screen in pixels.
 *  @param screen a pointer returned by lb_screen_construct. If screen does not point to a successfully constructed screen object, calling this function will result in undefined behaviour.
 *  @see lb_screen_construct
 *  @see lb_screen_constructed
 *  @see lb_screen_get_height
 *  @return the width of the screen measured in pixels
 */
int lb_screen_get_width( void* screen );

/** Returns the height of the screen in pixels.
 *  @param screen a pointer returned by lb_screen_construct. If screen does not point to a successfully constructed screen object, calling this function will result in undefined behaviour.
 *  @see lb_screen_construct
 *  @see lb_screen_constructed
 *  @see lb_screen_get_width
 *  @return the height of the screen measured in pixels
 */
int lb_screen_get_height( void* screen );

/** Returns the number of screen modes.
 *  @param screen a pointer returned by lb_screen_construct. If screen does not point to a successfully constructed screen object, calling this function will result in undefined behaviour.
 *  @see lb_screen_constructed
 *  @return the number of screen modes available
 */
int lb_screen_modes( void* screen );

/** Returns the width of the screen mode in pixels.
 *  @param screen a pointer returned by lb_screen_construct. If screen does not point to a successfully constructed screen object, calling this function will result in undefined behaviour.
 *  @param mode an integer identifying the screen mode. Valid modes range from 0 to lb_screen_modes( screen ) - 1, inclusive.
 *  @see lb_screen_construct
 *  @see lb_screen_constructed
 *  @see lb_screen_modes
 *  @see lb_screen_get_mode_height
 *  @return the width of the screen mode measured in pixels
 */
int lb_screen_get_mode_width( void* screen, int mode );

/** Returns the height of the screen mode in pixels.
 *  @param screen a pointer returned by lb_screen_construct. If screen does not point to a successfully constructed screen object, calling this function will result in undefined behaviour.
 *  @param mode an integer identifying the screen mode. Valid modes range from 0 to lb_screen_modes( screen ) - 1, inclusive.
 *  @see lb_screen_construct
 *  @see lb_screen_constructed
 *  @see lb_screen_modes
 *  @see lb_screen_get_mode_width
 *  @return the height of the screen mode measured in pixels
 */
int lb_screen_get_mode_height( void* screen, int mode );

/** Changes the screen mode.
 *  @param screen a pointer returned by lb_screen_construct. If screen does not point to a successfully constructed screen object, calling this function will result in undefined behaviour.
 *  @param mode an integer identifying the screen mode. Valid modes range from 0 to lb_screen_modes( screen ) - 1, inclusive.
 *  @see lb_screen_construct
 *  @see lb_screen_constructed
 *  @see lb_screen_modes
 *  @return 1 if successful else returns 0.
 */
int lb_screen_change_mode( void* screen, int mode );

#ifdef __cplusplus
}
#endif
#endif

