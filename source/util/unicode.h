
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
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

