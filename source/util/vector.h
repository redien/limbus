
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef _VECTOR_H_
#define _VECTOR_H_

typedef struct _Vector
{
	unsigned int size,
				 element_size,
				 data_size;

	void* data;
} Vector;

void vector_construct( Vector* vector, unsigned int element_size );
void vector_destruct( Vector* vector );
int vector_push_back( Vector* vector, void* element );
void* vector_at( Vector* vector, unsigned int position );
void* vector_next( Vector* vector, void* current );
int vector_size( Vector* vector );
void vector_erase( Vector* vector, void* iterator );
void vector_clear( Vector* vector );
void* vector_data( Vector* vector );

#endif

