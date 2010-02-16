
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include "unicode.h"

#define MASK_FIRST_5 0xF8
#define MASK_FIRST_4 0xF0
#define MASK_FIRST_3 0xE0
#define MASK_FIRST_2 0xC0

#define MASK_LAST_6 0x3F
#define MASK_LAST_5 0x1F

static int getutf8len( unsigned char first_byte )
{
	first_byte &= MASK_FIRST_5;
	if (first_byte == MASK_FIRST_4)
		return 4;

	first_byte &= MASK_FIRST_4;
	if (first_byte == MASK_FIRST_3)
		return 3;

	first_byte &= MASK_FIRST_3;
	if (first_byte == MASK_FIRST_2)
		return 2;
	
	return 1;
}

int utf8to32( char* _str8, char32* str32, size_t max_len )
{
	unsigned char* str8 = (unsigned char*)_str8;
	size_t i = 0, u;
	for (u = 0; u < max_len; u++)
	{
		if (str8[i] == 0)
			break;

		if (str8[i] < 128)
		{
			str32[u] = str8[i];
			i++;
		}
		else
		{
			int len = getutf8len( str8[i] );
			str32[u] = 0;
			
			if (len == 2)
			{
				str32[u] |= (str8[i + 1] & MASK_LAST_6);
				str32[u] |= (str8[i] & MASK_LAST_5) << 6;
			}
			
			if (len == 3)
			{
				str32[u] |= (str8[i + 2] & MASK_LAST_6);
				str32[u] |= (str8[i + 1] & MASK_LAST_6) << 6;
				str32[u] |= (str8[i] & MASK_LAST_5) << 12;
			}
			
			if (len == 4)
			{
				str32[u] |= (str8[i + 3] & MASK_LAST_6);
				str32[u] |= (str8[i + 2] & MASK_LAST_6) << 6;
				str32[u] |= (str8[i + 1] & MASK_LAST_6) << 6;
				str32[u] |= (str8[i] & MASK_LAST_5) << 18;
			}

			i += len;
		}
	}
	
	if (u != max_len)
		str32[u] = 0;

	return i;
}

int utf8len( char* str8 )
{
	int len = 0,
		charlen;

	while (*str8 != '\0')
	{
		charlen = getutf8len( *str8 );
		len++;
		str8 += charlen;
	}

	return len;
}
/*
int utf32to8( char32* utf32, char* _utf8, size_t buffer_size )
{
	size_t i;
	
	
}
*/

#include "unittest.h"

void unicode_unittest()
{
	char* test_string = (char*)"abcd åäö";
	char32 dest_string[9] = {0x33};
/*	char32 utf32_string[4] = {0x41, 0xC1, 0xE5, 0x00};
	char utf8_dest_string[6];
*/

	EXPECT_EQUAL( utf8to32( (char*)"", dest_string, 8 ), 0 );
	EXPECT_EQUAL( 0x00, dest_string[1] );

	EXPECT_EQUAL( utf8to32( (char*)"a", dest_string, 8 ), 1 );
	EXPECT_EQUAL( 0x61, dest_string[0] );
	EXPECT_EQUAL( 0x00, dest_string[1] );

	EXPECT_EQUAL( utf8to32( test_string, dest_string, 8 ), 11 );
	
	EXPECT_EQUAL( 0x61, dest_string[0] );
	EXPECT_EQUAL( 0x62, dest_string[1] );
	EXPECT_EQUAL( 0x63, dest_string[2] );
	EXPECT_EQUAL( 0x64, dest_string[3] );
	EXPECT_EQUAL( 0x20, dest_string[4] );
	EXPECT_EQUAL( 0xE5, dest_string[5] );
	EXPECT_EQUAL( 0xE4, dest_string[6] );
	EXPECT_EQUAL( 0xF6, dest_string[7] );
	EXPECT_EQUAL( 0x00, dest_string[8] );
	
	EXPECT_EQUAL( utf8len( test_string ), 8 );
	EXPECT_EQUAL( utf8len( "" ), 0 );
	EXPECT_EQUAL( utf8len( "a" ), 1 );
	EXPECT_EQUAL( utf8len( "ÿ" ), 1 );
	EXPECT_EQUAL( utf8len( "bÿ" ), 2 );
	EXPECT_EQUAL( utf8len( "ÿb" ), 2 );
	EXPECT_EQUAL( utf8len( "2ÿå" ), 3 );
	/*
	utf32to8( utf32_string, utf8_dest_string, 6 );
	EXPECT_EQUAL( strcmp( "AÁå", utf8_dest_string ), 0 );
	*/
}

