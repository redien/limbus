
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include <stdlib.h>

#define ASSERT( exp, msg )\
	if (!(exp)) {\
		fprintf( stderr, "%s", msg );\
		test_result = EXIT_FAILURE;\
	}

int test_result = EXIT_SUCCESS;

