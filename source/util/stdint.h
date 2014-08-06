
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef _STDINT_H_
#define _STDINT_H_

#ifndef __int8_t_defined
	#define __int8_t_defined
	typedef char int8_t;
#endif

#ifndef __uint8_t_defined
	#define __uint8_t_defined
	typedef unsigned char uint8_t;
#endif

#ifndef __int16_t_defined
	#define __int16_t_defined
	typedef short int16_t;
#endif

#ifndef __uint16_t_defined
	#define __uint16_t_defined
	typedef unsigned short uint16_t;
#endif

#ifndef __int32_t_defined
	#define __int32_t_defined
	typedef int int32_t;
#endif

#ifndef __uint32_t_defined
	#define __uint32_t_defined
	typedef unsigned int uint32_t;
#endif


typedef union
{
	#define STDINT_ASSERT_CORRECT_SIZE( type, size )\
		type type##_HAS_INCORRECT_SIZE[1 - 2 * !(sizeof( type ) == size)];

	STDINT_ASSERT_CORRECT_SIZE( int8_t, 1 )
	STDINT_ASSERT_CORRECT_SIZE( uint8_t, 1 )
	STDINT_ASSERT_CORRECT_SIZE( int16_t, 2 )
	STDINT_ASSERT_CORRECT_SIZE( uint16_t, 2 )
	STDINT_ASSERT_CORRECT_SIZE( int32_t, 4 )
	STDINT_ASSERT_CORRECT_SIZE( uint32_t, 4 )
} _STDINT_SIZE_ASSERT_UNION;

#endif

