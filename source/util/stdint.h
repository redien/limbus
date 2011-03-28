
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
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

