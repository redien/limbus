
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
#include <limbus/timing.h>

int main( int argc, char** argv )
{
	double start = 0.0;
	int counter = 10;
	
	printf( "Counting down from 10...\n" );
	
	while (counter > 0)
	{
		printf( "%d\n", counter );
		start = lb_time_count_in_seconds();
		while (lb_time_count_in_seconds() < (start + 1.0)) {}
		counter--;
	}

	return 0;
}
