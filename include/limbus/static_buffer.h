
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

