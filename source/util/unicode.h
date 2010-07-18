
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef _UNICODE_H_
#define _UNICODE_H_

#include <string.h>

typedef int char32;

/** Converts an utf8 string to utf32.
	str8		utf8 string to convert.
	str32		buffer to store the utf32 string in.
	max_len		the size of the destination buffer in unicode characters.

	returns the number of bytes of the initial string that were actually converted.
*/
int utf8to32( char* str8, char32* str32, size_t max_len );

/** Calculates the length of a utf8 string in unicode characters.
	str8		string to calculate the length of.
	returns the number of unicode characters encoded in the string.
*/
int utf8len( char* str8 );

#endif

