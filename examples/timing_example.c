
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include <stdio.h>
#include <platform/timing.h>

int main( int argc, char** argv )
{
	double start = 0.0;
	int counter = 10;
	
	printf( "Counting down from 10...\n" );
	
	while (counter > 0)
	{
		printf( "%d\n", counter );
		start = time_count_in_seconds();
		while (time_count_in_seconds() < (start + 1.0)) {}
		counter--;
	}

	return 0;
}
