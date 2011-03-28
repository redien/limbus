
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef LB_OPENGL_CONTEXT_H
#define LB_OPENGL_CONTEXT_H
#ifdef __cplusplus
extern "C"
{
#endif

#include <limbus/window.h>

typedef void* LBOpenglContext;

/** An enumeration of the different color formats. */
enum LBOpenglContextColorFormat
{
    /** Color format consisting of the colors red, green and blue as well as an additional opacity value. */
	LBOpenglContextColorFormatRGBA,
    /** Color format consisting of color indecies. */
	LBOpenglContextColorFormatIndexed
};
typedef enum LBOpenglContextColorFormat LBOpenglContextColorFormat;

#define LBOpenglContextCreateNew 0
#define LBOpenglContextUseCurrent 1

/**
 *  An integer identifying the default pixelformat.
 */
#define LBOpenglContextDefaultPixelformat 0

/** Constructs a new OpenGL context object.
  * The actual context will not be created until it is bound to the window using lb_opengl_context_bind.
  * @param window a pointer to a window object created with
  * lb_window_construct. The context object does not take ownership
  * of the window object pointed to by window.
  * @return a pointer to the newly created context object if successful,
  * else returns a pointer identifying a non-constructed context object.
  * Use lb_opengl_context_constructed to find out if a context object was
  * constructed or not.
  */
LBOpenglContext lb_opengl_context_construct( LBWindow window, int use_current );

/** Destructs a previously constructed OpenGL context object.
  * Destructing a OpenGL context object will deallocate all the resources
  * associated with it.
  * @param context a pointer to the context object to destruct.
  * @return a new pointer that can be passed to lb_opengl_context_constructed
  * to find out if the context object was destructed or not.
  */
LBOpenglContext lb_opengl_context_destruct( LBOpenglContext context );

/** Releases the ownership of the context, destructing
  * the context object will no longer destroy the context.
  * @param context a pointer to a successfully constructed context object.
  */
void lb_opengl_context_release_ownership( LBOpenglContext context );

/** Check for successfully constructed OpenGL context objects.
  * @param context a pointer returned by lb_opengl_context_construct
  * or lb_opengl_context_destruct
  * @return 1 if context points to a successfully constructed
  * OpenGL context object or 0 if it doesn't.
  */
int lb_opengl_context_constructed( LBOpenglContext context );

/** Get the number of available pixelformats.
  * @param context a pointer to a successfully constructed context object.
  * @return the number of available pixelformats.
  */
int lb_opengl_context_pixelformats( LBOpenglContext context );

/** Get the availability of double buffering for the specified pixelformat.
  * @param context a pointer to a successfully constructed context object.
  * @param pixelformat an integer identifying a valid pixelformat.
  * Valid pixelformats range from 0 to lb_opengl_context_pixelformats( context ) - 1, inclusive.
  * @return 1 if double buffering is available, 0 otherwise.
  */
int lb_opengl_context_get_pixelformat_double_buffer( LBOpenglContext context, int pixelformat );

/** Get the availability of OpenGL support for the specified pixelformat.
  * @param context a pointer to a successfully constructed context object.
  * @param pixelformat an integer identifying a valid pixelformat.
  * Valid pixelformats range from 0 to lb_opengl_context_pixelformats( context ) - 1, inclusive.
  * @return 1 if OpenGL support is available, 0 otherwise.
  */
int lb_opengl_context_get_pixelformat_support_opengl( LBOpenglContext context, int pixelformat );

/** Get the color format of the specified pixelformat.
  * @param context a pointer to a successfully constructed context object.
  * @param pixelformat an integer identifying a valid pixelformat.
  * Valid pixelformats range from 0 to lb_opengl_context_pixelformats( context ) - 1, inclusive.
  * @return the color format of the specified pixelformat.
  */
LBOpenglContextColorFormat lb_opengl_context_get_pixelformat_color_format( LBOpenglContext context, int pixelformat );

/** Get the depth size of the specified pixelformat.
  * @param context a pointer to a successfully constructed context object.
  * @param pixelformat an integer identifying a valid pixelformat.
  * Valid pixelformats range from 0 to lb_opengl_context_pixelformats( context ) - 1, inclusive.
  * @return the depth size of the specified pixelformat.
  */
int lb_opengl_context_get_pixelformat_depth_size( LBOpenglContext context, int pixelformat );

/** Bind the OpenGL context to the window using the specified pixelformat.
  * @param context a pointer to a successfully constructed context object.
  * @param pixelformat an integer identifying a valid pixelformat.
  * Valid pixelformats range from 0 to lb_opengl_context_pixelformats( context ) - 1, inclusive.
  */
void lb_opengl_context_bind( LBOpenglContext context, int pixelformat );

/** Sets the OpenGL context as the current context.
  * @param context a pointer to a successfully constructed context object.
  */
void lb_opengl_context_make_current( LBOpenglContext context );

/** Releases the OpenGL context as the current context.
  * @param context a pointer to a successfully constructed context object.
  */
void lb_opengl_context_release_current( LBOpenglContext context );

/** Swaps the framebuffers.
  * @param context a pointer to a successfully constructed context object.
  */
void lb_opengl_context_swap_buffers( LBOpenglContext context );

/** Sets the swap interval for the OpenGL context.
  * @param context a pointer to a successfully constructed context object.
  * @param interval the new interval.
  * @return 1 if successful, 0 otherwise.
  */
int lb_opengl_context_set_swap_interval( LBOpenglContext context, int interval );

#ifdef __cplusplus
}
#endif
#endif

