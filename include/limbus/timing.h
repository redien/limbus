
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef LB_TIMING_H
#define LB_TIMING_H
#ifdef __cplusplus
extern "C"
{
#endif

double lb_time_count_in_seconds( void );
unsigned long lb_time_count_in_milliseconds( void );
void lb_msleep( unsigned int msec );
void lb_usleep( unsigned int usec );

#ifdef __cplusplus
}
#endif
#endif

