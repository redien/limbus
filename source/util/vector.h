
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
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

