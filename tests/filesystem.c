
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
#include <string.h>
#include <limbus/filesystem.h>

#include "common.h"

int main( int argc, char** argv )
{
	LBFilesystem filesystem;
	
	filesystem = lb_filesystem_construct();
	ASSERT( lb_filesystem_constructed( filesystem ) == LBFilesystemNoError, "A filesystem object could not be constructed" );
	
	ASSERT( lb_filesystem_path_is_directory( filesystem, "../tests/" ) == 1, "Failed to identify a directory with a suffixed slash" );
	ASSERT( lb_filesystem_path_is_directory( filesystem, "../tests" ) == 1, "Failed to identify a directory" );
	ASSERT( lb_filesystem_path_is_directory( filesystem, "../tests/filesystem.c" ) == 0, "Identified a file as a directory" );
	ASSERT( lb_filesystem_path_is_file( filesystem, "../tests/filesystem.c" ) == 1, "Failed to identify a file" );
	ASSERT( lb_filesystem_path_is_file( filesystem, "../tests" ) == 0, "Identified a directory as a file" );

	lb_filesystem_destruct( filesystem );

	return test_result;
}
