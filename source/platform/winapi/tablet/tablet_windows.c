
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include "../../../util/vector.h"
#include "../../tablet.h"
#include "../win_window.h"
#include "../winapi_input_device.h"
#include <assert.h>
#include <stdlib.h>

#include <windows.h>
#include <wintab.h>

#define PACKETDATA (PK_X | PK_Y | PK_BUTTONS | PK_NORMAL_PRESSURE)
#define PACKETMODE PK_BUTTONS
#include <pktdef.h>

typedef struct TabletEventTag
{
	int type;
	int button;
	int pressure;
	int x, y;
} TabletEvent;

typedef struct WinAPITabletTag
{
	WinAPIInputDevice base;

	Vector events;
	TabletEvent event;

	int context_width, context_height;
	int res_x, res_y;

	HCTX handle;

	int added_to_window;
} WinAPITablet;

#define CAST_TABLET()\
	WinAPITablet* tablet = (WinAPITablet*)t;\
	assert( tablet != NULL );


TabletEvent translate_wintab_packet( PACKET* packet )
{
	TabletEvent event;
	event.type = TabletEventMotion;
	event.x = packet->pkX;
	event.y = packet->pkY;
	event.pressure = packet->pkNormalPressure;
	return event;
}

static void construct( void* t )
{
	LOGCONTEXT context;
/*	UINT default_device; */
	CAST_TABLET()

	/* Implementation already created */
	if (tablet->handle)
		return;
/*
	WTInfo( WTI_DEFCONTEXT, CTX_DEVICE, &default_device );
	WTInfo( WTI_DEVICES + default_device, DVC_X, &tablet->res_x );
	WTInfo( WTI_DEVICES + default_device, DVC_Y, &tablet->res_y );
*/
	tablet->context_width = 1000;
	tablet->context_height = 1000;

	WTInfo( WTI_DEFCONTEXT, 0, &context );

	strcpy( context.lcName, "Rule2 Digitizing" );
	context.lcOptions |= CXO_MESSAGES;
	context.lcPktData = PACKETDATA;
	context.lcPktMode = PACKETMODE;
	context.lcMoveMask = PACKETDATA;
	context.lcBtnUpMask = context.lcBtnDnMask;

	context.lcOutOrgX = context.lcOutOrgY = 0;

	context.lcOutExtX = 1 * context.lcInExtX;
	context.lcOutExtY = 1 * context.lcInExtY;

/*
	context.lcOutExtX = tablet->res_x;
	context.lcOutExtY = tablet->res_y;
*/

	tablet->handle = WTOpen( tablet->base.handle, &context, TRUE );

	tablet->added_to_window = 1;
}

static int handle_winapi_message( void* t, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* result )
{
	CAST_TABLET()

	if (uMsg == WT_PACKET)
	{
		PACKET packet;
		if (WTPacket( (HCTX)lParam, wParam, &packet ))
		{
			TabletEvent event = translate_wintab_packet( &packet );
			vector_push_back( &tablet->events, &event );
		}
	}
	else if (uMsg == WT_PROXIMITY)
	{
		TabletEvent event;
		event.type = (LOWORD(lParam)) ? TabletEventProximityIn : TabletEventProximityOut;
		vector_push_back( &tablet->events, &event );
	}

	return 0;
}


void* tablet_construct()
{
	WinAPITablet* tablet = (WinAPITablet*)malloc( sizeof( WinAPITablet ) );
	vector_create( &tablet->events, sizeof( TabletEvent ), 3 );

	tablet->base.construct = &construct;
	tablet->base.handle_winapi_message = &handle_winapi_message;

	tablet->handle = (HCTX)0;
	tablet->added_to_window = 0;

	return tablet;
}

void tablet_destruct( void* t )
{
	CAST_TABLET()

	vector_destroy( &tablet->events );

	if (tablet->added_to_window)
	{
		if (tablet->handle)
			WTClose( tablet->handle );
	}

	free( tablet );
}

int tablet_constructed( void* tablet )
{
	return (tablet != NULL) ? 1 : 0;
}

void tablet_set_context_width( void* t, int width )
{
	CAST_TABLET()
	tablet->context_width = width;
}

void tablet_set_context_height( void* t, int height )
{
	CAST_TABLET()
	tablet->context_height = height;
}

int tablet_get_context_width( void* t )
{
	CAST_TABLET()
	return tablet->context_width;
}

int tablet_get_context_height( void* t )
{
	CAST_TABLET()
	return tablet->context_height;
}


void tablet_toggle_wintab_context_enabled( void* t, int enabled )
{
	CAST_TABLET()

	if (!tablet->handle)
		return;

	WTEnable( tablet->handle, (enabled != 0) ? TRUE : FALSE );
	WTOverlap( tablet->handle, (enabled != 0) ? TRUE : FALSE );
}

int tablet_next_event( void* t )
{
	TabletEvent* iter;
	CAST_TABLET()

	iter = (TabletEvent*)vector_next( &tablet->events, NULL );
	if (iter != NULL)
	{
		tablet->event = *iter;
		vector_erase( &tablet->events, iter );
		return 1;
	}
	else
	{
		return 0;
	}
}

enum TabletEvent tablet_get_event_type( void* t )
{
	CAST_TABLET()
	return tablet->event.type;
}

int tablet_get_event_button( void* t )
{
	CAST_TABLET()
	return tablet->event.button;
}

int tablet_get_event_pressure( void* t )
{
	CAST_TABLET()
	return tablet->event.pressure;
}

int tablet_get_event_x( void* t )
{
	CAST_TABLET()
	return tablet->event.x;
}

int tablet_get_event_y( void* t )
{
	CAST_TABLET()
	return tablet->event.y;
}
