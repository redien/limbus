
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
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

