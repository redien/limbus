
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef _LB_TIMING_H_
#define _LB_TIMING_H_
#ifdef __cplusplus
extern "C"
{
#endif

double lb_time_count_in_seconds( void );
void lb_msleep( unsigned int msec );
void lb_usleep( unsigned int usec );

#ifdef __cplusplus
}
#endif
#endif

