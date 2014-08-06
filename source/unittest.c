
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
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

