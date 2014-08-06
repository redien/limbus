
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
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

