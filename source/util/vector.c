
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "vector.h"

void vector_construct( Vector* vector, unsigned int element_size )
{
	assert( vector != NULL );

	vector->size = 0;
	vector->element_size = element_size;
	vector->data_size = 3 * element_size;
	
	vector->data = malloc( vector->data_size );
	
	assert( vector->data != NULL );
}

void vector_destruct( Vector* vector )
{
	assert( vector != NULL );
	assert( vector->data != NULL );
	
	free( vector->data );
	vector->data = NULL;
}

int vector_push_back( Vector* vector, void* element )
{
	unsigned int offset;

	assert( vector != NULL );
	assert( vector->data != NULL );
	assert( vector->data_size > 0 );
	assert( vector->element_size > 0 );
	assert( element );

	offset = (vector->element_size * vector->size);

	if (offset >= vector->data_size)
	{
		vector->data_size *= 2;
		vector->data = realloc( vector->data, vector->data_size );
		assert( vector->data != NULL );
	}

	memcpy( (char*)vector->data + offset, element, vector->element_size );
	
	vector->size++;
	return vector->size - 1;
}

void* vector_at( Vector* vector, unsigned int position )
{
	assert( vector != NULL );
	assert( vector->data != NULL );
	assert( vector->element_size > 0 );

	if (position < vector->size)
		return (char*)vector->data + (vector->element_size * position);
	else
		return NULL;
}

#define ASSERT_ITERATOR_IS_IN_RANGE( iterator )\
	assert( iterator >= vector->data &&\
			iterator < (void*)((char*)vector->data + vector->data_size) );\
	assert( 0 == (((char*)iterator - (char*)vector->data)\
					% vector->element_size) );

void* vector_next( Vector* vector, void* current )
{
	void *next, *last;

	assert( vector != NULL );
	assert( vector->data != NULL );
	assert( vector->size >= 0 );
	
	if (current == NULL)
	{
		if (vector->size == 0)
			return NULL;
		else
			return vector->data;
	}
	else
	{
		ASSERT_ITERATOR_IS_IN_RANGE( current );

		next = (char*)current + vector->element_size;
		last = (char*)vector->data + (vector->element_size * vector->size);

		if (next >= last)
			return NULL;
	
		return next;
	}
}

int vector_size( Vector* vector )
{
	assert( vector );
	assert( vector->data != NULL );

	return vector->size;
}

void vector_erase( Vector* vector, void* iterator )
{
	void *next, *last;
	size_t tail_size;
	assert( vector != NULL );
	assert( vector->data != NULL );
	assert( iterator != NULL );
	ASSERT_ITERATOR_IS_IN_RANGE( iterator );
	
	next = (char*)iterator + vector->element_size;
	last = (char*)vector->data + (vector->element_size * vector->size);
	tail_size = ((char*)last - (char*)next) + vector->element_size;

	vector->size--;

	if (iterator != last)
	{
		memcpy( iterator, next, tail_size );
	}
}

void vector_clear( Vector* vector )
{
	assert( vector != NULL );
	assert( vector->data != NULL );
	
	vector->size = 0;
}

void* vector_data( Vector* vector )
{
	assert( vector != NULL );
	assert( vector->data != NULL );
	return vector->data;
}

#include "unittest.h"

void vector_unittest()
{
	Vector vector;
	int integer, i;
	int* iterator;
	
	/* An empty vector. */
	vector_construct( &vector, sizeof( int ) );
	EXPECT_EQUAL( vector_next( &vector, NULL ), NULL );
	EXPECT_EQUAL( vector_at( &vector, 0 ), NULL );
	EXPECT_EQUAL( vector_at( &vector, 1 ), NULL );
	EXPECT_EQUAL( vector_at( &vector, 2 ), NULL );
	EXPECT_EQUAL( vector_at( &vector, 3 ), NULL );
	EXPECT_EQUAL( vector_at( &vector, 14 ), NULL );
	EXPECT_EQUAL( vector_at( &vector, 2345 ), NULL );
	vector_destruct( &vector );


	/* Adding elements */
	
	vector_construct( &vector, sizeof( int ) );

	for (i = 0; i < 9; i++)
	{
		integer = 9 - i;
		EXPECT_EQUAL( vector_push_back( &vector, &integer ), i );
		EXPECT_EQUAL( vector_size( &vector ), (i + 1) );
	}

	EXPECT_NOT_EQUAL( vector_at( &vector, 0 ), NULL );
	EXPECT_EQUAL( *((int*)vector_at( &vector, 0 )), 9 );
	EXPECT_NOT_EQUAL( vector_at( &vector, 1 ), NULL );
	EXPECT_EQUAL( *((int*)vector_at( &vector, 1 )), 8 );
	EXPECT_NOT_EQUAL( vector_at( &vector, 2 ), NULL );
	EXPECT_EQUAL( *((int*)vector_at( &vector, 2 )), 7 );
	EXPECT_NOT_EQUAL( vector_at( &vector, 3 ), NULL );
	EXPECT_EQUAL( *((int*)vector_at( &vector, 3 )), 6 );
	EXPECT_EQUAL( vector_at( &vector, 14 ), NULL );
	EXPECT_EQUAL( vector_at( &vector, 2345 ), NULL );

	i = 9;
	iterator = NULL;
	while ((iterator = (int*)vector_next( &vector, iterator )))
	{
		EXPECT_EQUAL( i, *iterator );
		i--;
	}

	/* Erasing elements */
	for (i = 9 - 1; i >= 4; i--)
	{
		vector_erase( &vector, vector_next( &vector, NULL ) );
		EXPECT_EQUAL( vector_size( &vector ), i );
	}

	i = 4;
	iterator = NULL;
	while ((iterator = (int*)vector_next( &vector, iterator )))
	{
		EXPECT_EQUAL( i, *iterator );
		i--;
	}

	vector_destruct( &vector );	

	/* Erase from the middle of a vector */
	vector_construct( &vector, sizeof( int ) );
	integer = 0x12345678; vector_push_back( &vector, &integer );
	integer = 0x21436587; vector_push_back( &vector, &integer );
	integer = 0x87654321; vector_push_back( &vector, &integer );
	vector_erase( &vector, vector_at( &vector, 1 ) );
	EXPECT_EQUAL( 0x12345678, *(int*)vector_at( &vector, 0 ) );
	EXPECT_EQUAL( 0x87654321, *(int*)vector_at( &vector, 1 ) );
	EXPECT_EQUAL( NULL, vector_at( &vector, 2 ) );
	vector_destruct( &vector );

	/* Clearing a vector */
	vector_construct( &vector, sizeof( int ) );
	EXPECT_EQUAL( NULL, vector_next( &vector, NULL ) );
	vector_clear( &vector );
	EXPECT_EQUAL( NULL, vector_next( &vector, NULL ) );
	integer = 3; vector_push_back( &vector, &integer );
	vector_clear( &vector );
	EXPECT_EQUAL( NULL, vector_next( &vector, NULL ) );
	vector_clear( &vector );
	EXPECT_EQUAL( NULL, vector_next( &vector, NULL ) );
	vector_destruct( &vector );
}

