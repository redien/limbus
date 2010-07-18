
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef _UNITTEST_H_
#define _UNITTEST_H_

#include <stdio.h>
#include <stdlib.h>

#define EXPECT_TRUE( expr )\
	if (!(expr)) {\
		fprintf( stderr, "Test EXPECT_TRUE( " #expr " ) failed\n	at %s:%d\n", __FILE__, __LINE__ );\
		abort();\
	}

#define EXPECT_FALSE( expr )\
	if (expr) {\
		fprintf( stderr, "Test EXPECT_FALSE( " #expr " ) failed\n	at %s:%d\n", __FILE__, __LINE__ );\
		abort();\
	}

#define EXPECT_EQUAL( a, b )\
	if ((a) != (b)) {\
		fprintf( stderr, "Test EXPECT_EQUAL( " #a ", " #b " ) failed\n	at %s:%d\n", __FILE__, __LINE__ );\
		abort();\
	}

#define EXPECT_NOT_EQUAL( a, b )\
	if ((a) == (b)) {\
		fprintf( stderr, "Test EXPECT_NOT_EQUAL( " #a ", " #b " ) failed\n	at %s:%d\n", __FILE__, __LINE__ );\
		abort();\
	}

#endif

