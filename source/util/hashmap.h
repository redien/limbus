
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
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

