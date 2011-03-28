
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef LB_SCREEN_H
#define LB_SCREEN_H
#ifdef __cplusplus
extern "C"
{
#endif

typedef void* LBScreen;

/* An enumeration of the API-specific errors. */
enum LBScreenError
{
    /* No error occured */
    LBScreenNoError,
    /* The specified screen mode was not valid */
    LBScreenInvalidMode
};
typedef enum LBScreenError LBScreenError;

/**
 *  An integer identifying the default screen of the system.
 */
#define LBScreenDefault 0

/** Constructs a screen object for the given screen.
 *  LBScreenDefault refers to the system's default screen if there is one.
 *  @param lb_screen_id the id of the screen to construct an object for
 *  @return a pointer to the newly created screen object if successful, else returns a pointer identifying a non-constructed screen object. Use lb_screen_constructed to find out if a screen object was constructed or not.
 */
LBScreen lb_screen_construct( int screen_id );

/** Destructs a previously constructed screen object.
 *  Destructing a screen object will deallocate all the resources associated with it.
 *  If the screen mode was changed using lb_screen_change_mode, destructing the object will restore the mode current at construction.
 *  @param screen a pointer to the screen object to destruct.
 *  @return a new pointer that can be passed to lb_screen_constructed to find out if the screen object was destructed or not.
 */
LBScreen lb_screen_destruct( LBScreen screen );

/** Check for successfully constructed screen objects.
 *  @param screen a pointer returned by lb_screen_construct or lb_screen_destruct
 *  @return 1 if screen points to a successfully constructed screen object or 0 if it doesn't.
 */
int lb_screen_constructed( LBScreen screen );

/** Get the width of the screen in pixels.
 *  @param screen a pointer to a successfully constructed screen object.
 *  @return the width of the screen measured in pixels
 */
int lb_screen_get_width( LBScreen screen );

/** Get the height of the screen in pixels.
 *  @param screen a pointer to a successfully constructed screen object.
 *  @return the height of the screen measured in pixels
 */
int lb_screen_get_height( LBScreen screen );

/** Get the number of screen modes available for the screen.
 *  @param screen a pointer to a successfully constructed screen object.
 *  @return the number of screen modes available
 */
int lb_screen_modes( LBScreen screen );

/** Get the width of the screen mode in pixels.
 *  @param screen a pointer to a successfully constructed screen object.
 *  @param mode an integer identifying the screen mode. Valid modes range from 0 to lb_screen_modes( screen ) - 1, inclusive.
 *  @return the width of the screen mode measured in pixels
 */
int lb_screen_get_mode_width( LBScreen screen, int mode );

/** Get the height of the screen mode in pixels.
 *  @param screen a pointer to a successfully constructed screen object.
 *  @param mode an integer identifying the screen mode. Valid modes range from 0 to lb_screen_modes( screen ) - 1, inclusive.
 *  @return the height of the screen mode measured in pixels
 */
int lb_screen_get_mode_height( LBScreen screen, int mode );

/** Changes the screen mode.
 *  @param screen a pointer to a successfully constructed screen object.
 *  @param mode an integer identifying the screen mode. Valid modes range from 0 to lb_screen_modes( screen ) - 1, inclusive.
 *  @return LBScreenNoError if successful else returns the error.
 */
LBScreenError lb_screen_change_mode( LBScreen screen, int mode );

#ifdef __cplusplus
}
#endif
#endif

