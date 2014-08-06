
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#define _CRT_SECURE_NO_WARNINGS

#include "../../../util/vector.h"
#include "../win_window.h"
#include "../winapi_input_device.h"
#include <limbus/tablet.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "wintab.h"

#define DESIRED_WINTAB_QUEUE_SIZE 128

typedef struct WinAPITabletEventTag
{
	int type;
	int button;
	int pressure;
	int x, y;
} WinAPITabletEvent;

typedef struct WinAPITabletTag
{
	WinAPIInputDevice base;

	Vector events;
	WinAPITabletEvent event;

	int context_width, context_height;
	int res_x, res_y;

	HCTX handle;

	int added_to_window;
	
	WTInfoProc WTInfo;
	WTOpenProc WTOpen;
	WTCloseProc WTClose;
	WTPacketsGetProc WTPacketsGet;
	WTPacketProc WTPacket;
	WTEnableProc WTEnable;
	WTOverlapProc WTOverlap;
	WTQueueSizeSetProc WTQueueSizeSet;
} WinAPITablet;

#define CAST_TABLET()\
	WinAPITablet* tablet = (WinAPITablet*)t;\
	assert( tablet != NULL );


static WinAPITabletEvent translate_wintab_packet( PACKET* packet )
{
	WinAPITabletEvent event;
	event.type = LBTabletEventMotion;
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

	tablet->WTInfo( WTI_DEFCONTEXT, 0, &context );

	strcpy( context.lcName, "Rule2 Digitizing" );
	context.lcOptions |= CXO_MESSAGES | CXO_SYSTEM;
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

	tablet->handle = tablet->WTOpen( tablet->base.handle, &context, TRUE );
	
	/* Set the the queue size to a valid size equal to or below the desired one */
	{
		int size = DESIRED_WINTAB_QUEUE_SIZE;
		while (tablet->WTQueueSizeSet( tablet->handle, size ) == FALSE)
		{
			size /= 2;
			assert( size > 1 );
		}
	}

	tablet->added_to_window = 1;
}

static int handle_winapi_message( void* t, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* result )
{
	CAST_TABLET()

	if (uMsg == WT_PACKET)
	{
		PACKET packet;
		if (tablet->WTPacket( (HCTX)lParam, wParam, &packet ))
		{
			WinAPITabletEvent event = translate_wintab_packet( &packet );
			vector_push_back( &tablet->events, &event );
		}
	}
	else if (uMsg == WT_PROXIMITY)
	{
		WinAPITabletEvent event;
		event.type = (LOWORD(lParam)) ? LBTabletEventProximityIn : LBTabletEventProximityOut;
		vector_push_back( &tablet->events, &event );
		
		/* Flush WinTab packet queue */
		tablet->WTPacketsGet( (HCTX)lParam, DESIRED_WINTAB_QUEUE_SIZE + 1, NULL );
	}

	return 0;
}


void* lb_tablet_construct()
{
	HMODULE dll_module;
	WinAPITablet* tablet = (WinAPITablet*)malloc( sizeof( WinAPITablet ) );
	vector_construct( &tablet->events, sizeof( WinAPITabletEvent ) );

	tablet->base.construct = &construct;
	tablet->base.handle_winapi_message = &handle_winapi_message;
	
	dll_module = LoadLibrary( "WINTAB32.DLL" );
	if ( !dll_module )
	{
		lb_tablet_destruct( tablet );
		return NULL;
	}

	#define LOAD_WT_PROC( name )\
		tablet->##name = (name##Proc)GetProcAddress( dll_module, (LPCSTR)MAKELONG( name##Ordinal, 0 ) );\
		if ( !tablet->##name )\
		{\
			lb_tablet_destruct( tablet );\
			return NULL;\
		}

	LOAD_WT_PROC( WTInfo )
	LOAD_WT_PROC( WTOpen )
	LOAD_WT_PROC( WTClose )
	LOAD_WT_PROC( WTPacketsGet )
	LOAD_WT_PROC( WTPacket )
	LOAD_WT_PROC( WTEnable )
	LOAD_WT_PROC( WTOverlap )
	LOAD_WT_PROC( WTQueueSizeSet )

	tablet->handle = (HCTX)0;
	tablet->added_to_window = 0;

	return tablet;
}

void lb_tablet_destruct( void* t )
{
	CAST_TABLET()

	vector_destruct( &tablet->events );

	if (tablet->added_to_window)
	{
		if (tablet->handle)
			tablet->WTClose( tablet->handle );
	}

	free( tablet );
}

int lb_tablet_constructed( void* tablet )
{
	return (tablet != NULL) ? 1 : 0;
}

void lb_tablet_set_context_width( void* t, int width )
{
	CAST_TABLET()
	tablet->context_width = width;
}

void lb_tablet_set_context_height( void* t, int height )
{
	CAST_TABLET()
	tablet->context_height = height;
}

int lb_tablet_get_context_width( void* t )
{
	CAST_TABLET()
	return tablet->context_width;
}

int lb_tablet_get_context_height( void* t )
{
	CAST_TABLET()
	return tablet->context_height;
}

int lb_tablet_next_event( void* t )
{
	WinAPITabletEvent* iter;
	CAST_TABLET()

	iter = (WinAPITabletEvent*)vector_next( &tablet->events, NULL );

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

LBTabletEvent lb_tablet_get_event_type( void* t )
{
	CAST_TABLET()
	return tablet->event.type;
}

int lb_tablet_get_event_button( void* t )
{
	CAST_TABLET()
	return tablet->event.button;
}

int lb_tablet_get_event_pressure( void* t )
{
	CAST_TABLET()
	return tablet->event.pressure;
}

int lb_tablet_get_event_x( void* t )
{
	CAST_TABLET()
	return tablet->event.x;
}

int lb_tablet_get_event_y( void* t )
{
	CAST_TABLET()
	return tablet->event.y;
}
