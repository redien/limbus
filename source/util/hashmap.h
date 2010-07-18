
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef _HASHMAP_H_
#define _HASHMAP_H_

#include "vector.h"

/* TODO: Probably shouldn't use 'int' for storing hash values */

typedef struct _HashMap
{
	Vector slots;
	int (*hashfunc)( void* key );
	int (*cmpfunc)( void* key1, void* key2 );
	int key_size, value_size;
} HashMap;

void hashmap_create( HashMap* map, int table_size,
					 int (*hashfunc)( void* key ),
					 int (*cmpfunc)( void* key1, void* key2 ),
					 int key_size, int value_size );
void hashmap_put( HashMap* map, void* key, void* value );
void* hashmap_get( HashMap* map, void* key );
void hashmap_destroy( HashMap* map );
void* hashmap_getkey( HashMap* map, void* iterator );
void* hashmap_getvalue( HashMap* map, void* iterator );
void* hashmap_next( HashMap* map, void* iterator );

#endif

