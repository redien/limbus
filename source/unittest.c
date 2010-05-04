
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include <stdio.h>

extern void vector_unittest();
extern void hashmap_unittest();
extern void unicode_unittest();

int main( int argc, char** argv )
{
	vector_unittest();
	hashmap_unittest();
	unicode_unittest();

	printf( "ALL TESTS PASSED!\n" );
	return 0;
}

