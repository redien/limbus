
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef LB_TABLET_H
#define LB_TABLET_H
#ifdef __cplusplus
extern "C"
{
#endif

/* Experimental */

enum LBTabletEvent
{
	LBTabletEventMotion,
	LBTabletEventButtonPress,
	LBTabletEventButtonRelease,
	LBTabletEventProximityIn,
	LBTabletEventProximityOut
};

void* lb_tablet_construct( void );
void lb_tablet_destruct( void* tablet );
int lb_tablet_constructed( void* tablet );

void lb_tablet_set_context_width( void* tablet, int width );
void lb_tablet_set_context_height( void* tablet, int height );

int lb_tablet_get_context_width( void* tablet );
int lb_tablet_get_context_height( void* tablet );

int lb_tablet_next_event( void* tablet );
enum LBTabletEvent lb_tablet_get_event_type( void* tablet );
int lb_tablet_get_event_button( void* tablet );
int lb_tablet_get_event_pressure( void* tablet );
int lb_tablet_get_event_x( void* tablet );
int lb_tablet_get_event_y( void* tablet );

#ifdef __cplusplus
}
#endif
#endif
