
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef LB_STATIC_BUFFER_H
#define LB_STATIC_BUFFER_H
#ifdef __cplusplus
extern "C"
{
#endif

/* Experimental */

typedef void* LBStaticBuffer;

/** Constructs a new static buffer object.
  * @param size_in_bytes the size of the buffer in bytes.
  * @return a pointer to the newly created static buffer object if successful,
  * else returns a pointer identifying a non-constructed static buffer object.
  * Use lb_static_buffer_constructed to find out if an object was
  * constructed or not.
  */
LBStaticBuffer lb_static_buffer_construct( unsigned int size_in_bytes );

/** Destructs a previously constructed static buffer object.
  * Destructing a static buffer object will deallocate all the resources
  * associated with it.
  * @param static_buffer a pointer to the static buffer object to destruct.
  */
void lb_static_buffer_destruct( LBStaticBuffer static_buffer );

/** Check for successfully constructed static buffer objects.
  * @param static_buffer a pointer returned by lb_static_buffer_construct
  * @return 1 if static_buffer points to a successfully constructed
  * static buffer object or 0 if it doesn't.
  */
int lb_static_buffer_constructed( LBFilesystem filesystem );

/** Sets a 32-bit floating-point value at the specified offset.
  * @param static_buffer a pointer returned by lb_static_buffer_construct
  * @param offset offset in the buffer to set the value in bytes.
  * @param value value to set at the specified offset.
  */
void lb_static_buffer_setf32( LBStaticBuffer static_buffer, unsigned int offset, float value );

/** Sets a 8-bit unsigned integer value at the specified offset.
  * @param static_buffer a pointer returned by lb_static_buffer_construct
  * @param offset offset in the buffer to set the value in bytes.
  * @param value value to set at the specified offset.
  */
void lb_static_buffer_setui8( LBStaticBuffer static_buffer, unsigned int offset, unsigned char value );

/** Gets a 32-bit floating-point value at the specified offset.
  * @param static_buffer a pointer returned by lb_static_buffer_construct
  * @param offset offset in the buffer to get the value in bytes.
  * @return value at the specified offset.
  */
float lb_static_buffer_getf32( LBStaticBuffer static_buffer, unsigned int offset );

/** Gets a 8-bit unsigned integer value at the specified offset.
  * @param static_buffer a pointer returned by lb_static_buffer_construct
  * @param offset offset in the buffer to get the value in bytes.
  * @return value at the specified offset.
  */
unsigned char lb_static_buffer_getui8( LBStaticBuffer static_buffer, unsigned int offset );

#ifdef __cplusplus
}
#endif
#endif

