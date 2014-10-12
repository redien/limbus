
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include <limbus/static_buffer.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>

void* lb_static_buffer_construct( unsigned int size )
{
    void* buffer = malloc( size );
    assert( buffer );
    return buffer;
}

void lb_static_buffer_destruct( void* buffer )
{
    free( buffer );
}

int lb_static_buffer_constructed( void* buffer )
{
    return (buffer) ? 1 : 0;
}

#define DEFINE_TYPE( name, type )\
    void lb_static_buffer_set##name( void* buffer, unsigned int offset, type value ) {\
        memcpy( &((char*)buffer)[offset], &value, sizeof value );\
    }\
    type lb_static_buffer_get##name( void* buffer, unsigned int offset ) {\
        return *((type*)&((char*)buffer)[offset]);\
    }

DEFINE_TYPE( f32, float )
DEFINE_TYPE( ui8, unsigned char )

