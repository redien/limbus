
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include "hashmap.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void hashmap_create( HashMap* map, int table_size,
					 int (*hashfunc)( void* key ),
					 int (*cmpfunc)( void* key1, void* key2 ),
					 int key_size, int value_size )
{
	int i;

	assert( map != NULL );
	assert( table_size > 0 );
	assert( hashfunc != NULL );
	assert( cmpfunc != NULL );
	assert( key_size > 0 );
	assert( value_size > 0 );

	vector_construct( &map->slots, sizeof( Vector ) );
	
	for (i = 0; i < table_size; i++)
	{
		Vector slot;
		vector_construct( &slot, key_size + value_size );
		vector_push_back( &map->slots, &slot );
	}
	
	map->hashfunc = hashfunc;
	map->cmpfunc = cmpfunc;
	map->key_size = key_size;
	map->value_size = value_size;
}

void hashmap_put( HashMap* map, void* key, void* value )
{
	int hash;
	void* iterator;
	Vector* slot;

	assert( map != NULL );
	assert( map->hashfunc != NULL );
	assert( map->cmpfunc != NULL );
	assert( key != NULL );
	assert( value != NULL );
	
	hash = map->hashfunc( key );
	slot = vector_at( &map->slots, hash );
	assert( slot != NULL );
	
	/* If there is a value at that key, replace it. */
	iterator = NULL;
	while ((iterator = vector_next( slot, iterator )))
	{
		if (map->cmpfunc( key, iterator ))
		{
			memcpy( (char*)iterator + map->key_size,
					value,
					map->value_size );
			break;
		}
	}
	
	/* Else, push on the new value */
	if (iterator == NULL)
	{
		int i;
		/* Might cause problems since push_back copies from memory past key */
		i = vector_push_back( slot, key );
		memcpy( (char*)vector_at( slot, i ) + map->key_size,
				value,
				map->value_size );
	}
}

void* hashmap_get( HashMap* map, void* key )
{
	int hash;
	void* iterator;
	Vector* slot;

	assert( map != NULL );
	assert( map->hashfunc != NULL );
	assert( map->cmpfunc != NULL );
	assert( key != NULL );
	
	hash = map->hashfunc( key );
	slot = vector_at( &map->slots, hash );
	assert( slot != NULL );
	
	iterator = NULL;
	while ((iterator = vector_next( slot, iterator )))
	{
		if (map->cmpfunc( key, iterator ))
			break;
	}
	
	if (iterator != NULL)
		return ((char*)iterator + map->key_size);
	else
		return NULL;
}

void hashmap_destroy( HashMap* map )
{
	Vector* iterator = NULL;
	
	while ((iterator = (Vector*)vector_next( &map->slots, iterator )))
	{
		vector_destruct( iterator );
	}
	
	vector_destruct( &map->slots );
}

void* hashmap_getkey( HashMap* map, void* iterator )
{
	return iterator;
}

void* hashmap_getvalue( HashMap* map, void* iterator )
{
	return (char*)iterator + map->key_size;
}

void* hashmap_next( HashMap* map, void* iterator )
{
	Vector* slot = NULL;
	void* next = NULL;
	int hash;

	assert( map != NULL );
	
	if (iterator != NULL)
	{
		/* Get the slot that the iterator is in. */
		hash = map->hashfunc( hashmap_getkey( map, iterator ) );
		slot = (Vector*)vector_at( &map->slots, hash );
		assert( slot != NULL );

		/* Get next */
		next = vector_next( slot, iterator );
	}
	
	while (NULL == next)
	{
		slot = (Vector*)vector_next( &map->slots, slot );
		
		if (NULL == slot)
			return NULL;
		
		next = vector_next( slot, NULL );
	}
	
	return next;
}

static int cmpfunc( void* key1, void* key2 )
{
	return (*((int*)key1)) == (*((int*)key2));
}

static int hashfunc( void* key )
{
	return (*((int*)key)) % 100;
}

#include "unittest.h"

void hashmap_unittest()
{
	HashMap map;
	int key, value;

	hashmap_create( &map, 100,
					&hashfunc, &cmpfunc,
					sizeof( int ), sizeof( int ) );

	key = 0;	value = 0; hashmap_put( &map, &key, &value );
	key = 23;	value = 1; hashmap_put( &map, &key, &value );
	key = 223;	value = 2; hashmap_put( &map, &key, &value );
	key = 1022;	value = 3; hashmap_put( &map, &key, &value );
	key = 34543;value = 4; hashmap_put( &map, &key, &value );
	key = 1022;	value = 22; hashmap_put( &map, &key, &value );
	key = 34543;value = 33; hashmap_put( &map, &key, &value );
	
	key = 0;	EXPECT_EQUAL( 0, *((int*)hashmap_get( &map, &key ) ));
	key = 23;	EXPECT_EQUAL( 1, *((int*)hashmap_get( &map, &key ) ));
	key = 223;	EXPECT_EQUAL( 2, *((int*)hashmap_get( &map, &key ) ));
	key = 1022;	EXPECT_EQUAL( 22, *((int*)hashmap_get( &map, &key ) ));
	key = 34543;EXPECT_EQUAL( 33, *((int*)hashmap_get( &map, &key ) ));
	key = 1;	EXPECT_EQUAL( NULL, hashmap_get( &map, &key ));
	key = 22;	EXPECT_EQUAL( NULL, hashmap_get( &map, &key ));
	key = 24;	EXPECT_EQUAL( NULL, hashmap_get( &map, &key ));
	key = 234;	EXPECT_EQUAL( NULL, hashmap_get( &map, &key ));
	key = 43567;EXPECT_EQUAL( NULL, hashmap_get( &map, &key ));

	hashmap_destroy( &map );

	/* Iterating */
	hashmap_create( &map, 100,
					&hashfunc, &cmpfunc,
					sizeof( int ), sizeof( int ) );

	EXPECT_EQUAL( hashmap_next( &map, NULL ), NULL );

	key = 24;	value = 0; hashmap_put( &map, &key, &value );
	key = 324;	value = 1; hashmap_put( &map, &key, &value );
	key = 693;	value = 2; hashmap_put( &map, &key, &value );
	key = 1209;	value = 3; hashmap_put( &map, &key, &value );
	
	{
		void* iterator = NULL;
		int *key, *value;

		iterator = hashmap_next( &map, iterator );
		EXPECT_NOT_EQUAL( iterator, NULL );
		iterator = hashmap_next( &map, iterator );
		EXPECT_NOT_EQUAL( iterator, NULL );
		iterator = hashmap_next( &map, iterator );
		EXPECT_NOT_EQUAL( iterator, NULL );
		iterator = hashmap_next( &map, iterator );
		EXPECT_NOT_EQUAL( iterator, NULL );
		iterator = hashmap_next( &map, iterator );
		EXPECT_EQUAL( iterator, NULL );

		
		while ((iterator = hashmap_next( &map, iterator )))
		{
			key = hashmap_getkey( &map, iterator );
			value = hashmap_getvalue( &map, iterator );
			EXPECT_NOT_EQUAL( NULL, key );
			EXPECT_NOT_EQUAL( NULL, value );
			EXPECT_EQUAL( *(int*)hashmap_get( &map, key ), *value );
		}
	}
	
	hashmap_destroy( &map );
}

