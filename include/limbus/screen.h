
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

/* An enumeration of the API-specific errors. */
enum LBScreenError
{
    /* No error occured */
    LBScreenNoError,
    /* The specified screen mode was not valid */
    LBScreenInvalidMode
};

/**
 *  An integer identifying the default screen of the system.
 */
#define LBScreenDefault 0

/** Constructs a screen object for the given screen.
 *  LBScreenDefault refers to the system's default screen if there is one.
 *  @param lb_screen_id the id of the screen to construct an object for
 *  @return a pointer to the newly created screen object if successful, else returns a pointer identifying a non-constructed screen object. Use lb_screen_constructed to find out if a screen object was constructed or not.
 */
void* lb_screen_construct( int screen_id );

/** Destructs a previously constructed screen object.
 *  Destructing a screen object will deallocate all the resources associated with it.
 *  @param screen a pointer to the screen object to destruct.
 *  @return a new pointer that can be passed to lb_screen_constructed to find out if the screen object was destructed or not.
 */
void* lb_screen_destruct( void* screen );

/** Check for successfully constructed screen objects.
 *  @param screen a pointer returned by lb_screen_construct or lb_screen_destruct
 *  @return 1 if screen points to a successfully constructed screen object or 0 if it doesn't.
 */
int lb_screen_constructed( void* screen );

/** Get the width of the screen in pixels.
 *  @param screen a pointer to a successfully constructed screen object.
 *  @return the width of the screen measured in pixels
 */
int lb_screen_get_width( void* screen );

/** Get the height of the screen in pixels.
 *  @param screen a pointer to a successfully constructed screen object.
 *  @return the height of the screen measured in pixels
 */
int lb_screen_get_height( void* screen );

/** Get the number of screen modes.
 *  @param screen a pointer to a successfully constructed screen object.
 *  @return the number of screen modes available
 */
int lb_screen_modes( void* screen );

/** Get the width of the screen mode in pixels.
 *  @param screen a pointer to a successfully constructed screen object.
 *  @param mode an integer identifying the screen mode. Valid modes range from 0 to lb_screen_modes( screen ) - 1, inclusive.
 *  @return the width of the screen mode measured in pixels
 */
int lb_screen_get_mode_width( void* screen, int mode );

/** Get the height of the screen mode in pixels.
 *  @param screen a pointer to a successfully constructed screen object.
 *  @param mode an integer identifying the screen mode. Valid modes range from 0 to lb_screen_modes( screen ) - 1, inclusive.
 *  @return the height of the screen mode measured in pixels
 */
int lb_screen_get_mode_height( void* screen, int mode );

/** Changes the screen mode.
 *  @param screen a pointer to a successfully constructed screen object.
 *  @param mode an integer identifying the screen mode. Valid modes range from 0 to lb_screen_modes( screen ) - 1, inclusive.
 *  @return LBScreenNoError if successful else returns the error.
 */
enum LBScreenError lb_screen_change_mode( void* screen, int mode );

#ifdef __cplusplus
}
#endif
#endif

