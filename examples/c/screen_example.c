
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include <limbus/screen.h>

#include <stdio.h>

int main( int argc, char** argv )
{
	LBScreen screen;

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
