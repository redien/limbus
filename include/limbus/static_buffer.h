
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

typedef void* LBStaticBuffer;

LBStaticBuffer lb_static_buffer_construct( unsigned int size );
void lb_static_buffer_destruct( LBStaticBuffer buffer );

void lb_static_buffer_setf32( LBStaticBuffer buffer, unsigned int offset, float value );
void lb_static_buffer_setui8( LBStaticBuffer buffer, unsigned int offset, unsigned char value );

float lb_static_buffer_getf32( LBStaticBuffer buffer, unsigned int offset );
unsigned char lb_static_buffer_getui8( LBStaticBuffer buffer, unsigned int offset );

#ifdef __cplusplus
}
#endif
#endif

