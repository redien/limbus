
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

typedef void* LBTablet;

enum LBTabletEvent
{
	LBTabletEventMotion,
	LBTabletEventButtonPress,
	LBTabletEventButtonRelease,
	LBTabletEventProximityIn,
	LBTabletEventProximityOut
};
typedef enum LBTabletEvent LBTabletEvent;

LBTablet lb_tablet_construct( void );
void lb_tablet_destruct( LBTablet tablet );
int lb_tablet_constructed( LBTablet tablet );

void lb_tablet_set_context_width( LBTablet tablet, int width );
void lb_tablet_set_context_height( LBTablet tablet, int height );

int lb_tablet_get_context_width( LBTablet tablet );
int lb_tablet_get_context_height( LBTablet tablet );

int lb_tablet_next_event( LBTablet tablet );
LBTabletEvent lb_tablet_get_event_type( LBTablet tablet );
int lb_tablet_get_event_button( LBTablet tablet );
int lb_tablet_get_event_pressure( LBTablet tablet );
int lb_tablet_get_event_x( LBTablet tablet );
int lb_tablet_get_event_y( LBTablet tablet );

#ifdef __cplusplus
}
#endif
#endif
