
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef LB_DISPLAY_H
#define LB_DISPLAY_H
#ifdef __cplusplus
extern "C"
{
#endif

typedef void* LBDisplay;

/* An enumeration of the API-specific errors. */
enum LBDisplayError
{
    /* No error occured */
    LBDisplayNoError,
    /* The specified display mode was not valid */
    LBDisplayInvalidMode
};
typedef enum LBDisplayError LBDisplayError;

/**
 *  An integer identifying the default display of the system.
 */
#define LBDisplayDefault 0

/** Constructs a display object for the given display.
 *  LBDisplayDefault refers to the system's default display if there is one.
 *  @param display_id the id of the display to construct an object for
 *  @return a pointer to the newly created display object if successful, else returns a pointer identifying a non-constructed display object. Use lb_display_constructed to find out if an object was constructed or not.
 */
LBDisplay lb_display_construct( int display_id );

/** Destructs a previously constructed display object.
 *  Destructing a display object will deallocate all the resources associated with it.
 *  If the display mode was changed using lb_display_change_mode, destructing the object will restore the mode current at construction.
 *  @param display a pointer to the display object to destruct.
 *  @return a new pointer that can be passed to lb_display_constructed to find out if the display object was destructed or not.
 */
LBDisplay lb_display_destruct( LBDisplay display );

/** Check for successfully constructed display objects.
 *  @param display a pointer returned by lb_display_construct
 *  @return 1 if display points to a successfully constructed display object or 0 if it doesn't.
 */
int lb_display_constructed( LBDisplay display );

/** Get the width of the display in pixels.
 *  @param display a pointer to a successfully constructed display object.
 *  @return the width of the display measured in pixels
 */
int lb_display_get_width( LBDisplay display );

/** Get the height of the display in pixels.
 *  @param display a pointer to a successfully constructed display object.
 *  @return the height of the display measured in pixels
 */
int lb_display_get_height( LBDisplay display );

/** Get the number of display modes available for the display.
 *  @param display a pointer to a successfully constructed display object.
 *  @return the number of display modes available
 */
int lb_display_modes( LBDisplay display );

/** Get the width of the display mode in pixels.
 *  @param display a pointer to a successfully constructed display object.
 *  @param mode an integer identifying the display mode. Valid modes range from 0 to lb_display_modes( display ) - 1, inclusive.
 *  @return the width of the display mode measured in pixels
 */
int lb_display_get_mode_width( LBDisplay display, int mode );

/** Get the height of the display mode in pixels.
 *  @param display a pointer to a successfully constructed display object.
 *  @param mode an integer identifying the display mode. Valid modes range from 0 to lb_display_modes( display ) - 1, inclusive.
 *  @return the height of the display mode measured in pixels
 */
int lb_display_get_mode_height( LBDisplay display, int mode );

/** Changes the display mode.
 *  @param display a pointer to a successfully constructed display object.
 *  @param mode an integer identifying the display mode. Valid modes range from 0 to lb_display_modes( display ) - 1, inclusive.
 *  @return LBDisplayNoError if successful else returns the error.
 */
LBDisplayError lb_display_change_mode( LBDisplay display, int mode );

#ifdef __cplusplus
}
#endif
#endif

