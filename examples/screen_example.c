
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include <limbus/screen.h>

#include <stdio.h>

int main( int argc, char** argv )
{
	void *screen;

	screen = lb_screen_construct( LBScreenDefault );
	if (lb_screen_constructed( screen ) == 0)
		return -1;

	printf( "current resolution: (%d, %d)\n", lb_screen_get_width( screen ),
											  lb_screen_get_height( screen ) );

	{
		int screen_modes = lb_screen_modes( screen );
		int i;
		for (i = 0; i < screen_modes; ++i)
		{
			printf( "%d - (%d, %d)\n", i,
									   lb_screen_get_mode_width( screen, i ),
									   lb_screen_get_mode_height( screen, i ) );
		}

		{
			int mode;
			printf( "Pick a screen mode. " );
			scanf( "%d", &mode );

			if (lb_screen_change_mode( screen, mode ) != LBScreenNoError)
			{
				printf( "Could not change mode...\n" );
			}
		}
	}

	lb_screen_destruct( screen );
	return 0;
}
