
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
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
	
	ASSERT( lb_filesystem_path_is_directory( filesystem, "../tests/" ) == 1, "Failed to identify a directory with a prefixed slash" );
	ASSERT( lb_filesystem_path_is_directory( filesystem, "../tests" ) == 1, "Failed to identify a directory" );
	ASSERT( lb_filesystem_path_is_directory( filesystem, "../tests/filesystem.c" ) == 0, "Identified a file as a directory" );
	ASSERT( lb_filesystem_path_is_file( filesystem, "../tests/filesystem.c" ) == 1, "Failed to identify a file" );
	ASSERT( lb_filesystem_path_is_file( filesystem, "../tests" ) == 0, "Identified a directory as a file" );

	lb_filesystem_destruct( filesystem );

	return test_result;
}
