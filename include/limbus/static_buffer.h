
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef LB_STATIC_BUFFER_H
#define LB_STATIC_BUFFER_H
#ifdef __cplusplus
extern "C"
{
#endif

/* Experimental */

void* lb_static_buffer_construct( unsigned int size );
void lb_static_buffer_destruct( void* buffer );

void lb_static_buffer_setf32( void* buffer, unsigned int offset, float value );
void lb_static_buffer_setui8( void* buffer, unsigned int offset, unsigned char value );

float lb_static_buffer_getf32( void* buffer, unsigned int offset );
unsigned char lb_static_buffer_getui8( void* buffer, unsigned int offset );

#ifdef __cplusplus
}
#endif
#endif

