
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
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

#define DEFINE_TYPE( name, type )\
    void lb_static_buffer_set##name( void* buffer, unsigned int offset, type value ) {\
        memcpy( &((char*)buffer)[offset], &value, sizeof value );\
    }\
    type lb_static_buffer_get##name( void* buffer, unsigned int offset ) {\
        return *((type*)&((char*)buffer)[offset]);\
    }

DEFINE_TYPE( f32, float )
DEFINE_TYPE( ui8, unsigned char )

