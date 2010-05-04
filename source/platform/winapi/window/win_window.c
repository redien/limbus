
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include "../../../util/vector.h"

#include <limbus/window.h>
#include "../win_window.h"
#include "../winapi_input_device.h"

#include <windows.h>
#include <shellapi.h>

#include <assert.h>
#include <string.h>

#define MAX_FILE_PATH 256

LRESULT CALLBACK WindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

static void construct_impl( void* win );
static void destruct_impl( void* win );

typedef struct
{
	int type;
	int files;
	int x, y;
} WindowEvent;

typedef struct
{
	int mapped;
	char* caption;
	int width, height;
	int x, y;
	int actual_x_offset, actual_y_offset;

	WindowEvent event;
	Vector events;
	Vector filepaths;

	Vector devices;
} WinWindowData;

void* lb_window_construct( void* screen )
{
	WinWindow* window;
	WinWindowData* window_data;

	window = (WinWindow*)malloc( sizeof( WinWindow ) );
	assert( window );
	
	window->construct_impl = &construct_impl;

	window_data = (WinWindowData*)malloc( sizeof( WinWindowData ) );
	assert( window_data );

	window_data->mapped = 0;
	window_data->caption = 0;
	window_data->x = CW_USEDEFAULT;
	window_data->y = CW_USEDEFAULT;
	window_data->actual_x_offset = 0;
	window_data->actual_y_offset = 0;
	
	window_data->width = 640;
	window_data->height = 480;

	vector_construct( &window_data->events, sizeof( WindowEvent ) );
	vector_construct( &window_data->devices, sizeof( WinAPIInputDevice* ) );
	window->impl_data = window_data;

	vector_construct( &window_data->filepaths, MAX_FILE_PATH + 1 );

	lb_window_set_caption( window, "" );

	return window;
}

#define DECLARE_WINDOW_AND_DATA()\
	WinWindow* window;\
	WinWindowData* window_data;\
	window = (WinWindow*)win;\
	assert( window );\
	window_data = (WinWindowData*)window->impl_data;\
	assert( window_data );

void* lb_window_destruct( void* win )
{
	DECLARE_WINDOW_AND_DATA()

	if (window_data->mapped)
		destruct_impl( window );

	if (window_data->caption)
		free( window_data->caption );

	vector_destruct( &window_data->filepaths );
	vector_destruct( &window_data->events );
	vector_destruct( &window_data->devices );

	free( window_data );
	free( window );

	return NULL;
}

int lb_window_constructed( LBWindow window )
{
	return window != NULL;
}

void lb_window_enable_decorations( LBWindow window )
{
}

void lb_window_disable_decorations( LBWindow window )
{
}

void lb_window_set_caption( void* win, const char* caption )
{
	int size;
	DECLARE_WINDOW_AND_DATA()

	if (window_data->caption)
		free( window_data->caption );

	size = strlen( caption ) + 1;
	window_data->caption = (char*)malloc( size );
	if (!window_data->caption)
		return;
	strcpy( window_data->caption, caption );

	if (window_data->mapped)
		SetWindowText( window->handle, window_data->caption );
}

static void correct_window_size( void* win )
{
	HWND handle;
	RECT client_rect,
		 window_rect;
	int window_width,
		window_height,
		client_width,
		client_height,
		new_width,
		new_height;
	DECLARE_WINDOW_AND_DATA()

	handle = window->handle;

	GetClientRect( handle, &client_rect );
	GetWindowRect( handle, &window_rect );

	window_width = window_rect.right - window_rect.left;
	window_height = window_rect.bottom - window_rect.top;

	client_width = client_rect.right;
	client_height = client_rect.bottom;

	new_width = window_width + (window_width - client_width);
	new_height = window_height + (window_height - client_height);

	MoveWindow( handle, window_rect.left, window_rect.top, new_width, new_height, TRUE );
}

static void update_window_rect( void* win )
{
	int* pos;
	DECLARE_WINDOW_AND_DATA()
	pos = &window_data->x;
	MoveWindow( window->handle, window_data->x,
								window_data->y,
								window_data->width,
								window_data->height,
								TRUE );
 	correct_window_size( window );
}

#define DEFINE_WINDOW_INT_SETTER( property )\
	void lb_window_set_##property( void* win, int property ) {\
		DECLARE_WINDOW_AND_DATA()\
		window_data->property = property;\
		if (window_data->mapped)\
			update_window_rect( win );\
	}

DEFINE_WINDOW_INT_SETTER( width )
DEFINE_WINDOW_INT_SETTER( height )
DEFINE_WINDOW_INT_SETTER( x )
DEFINE_WINDOW_INT_SETTER( y )

const char* lb_window_get_caption( void* win )
{
	DECLARE_WINDOW_AND_DATA()
	return (const char*)window_data->caption;
}

#define DEFINE_WINDOW_INT_GETTER( property )\
	int lb_window_get_##property( void* win ) {\
		DECLARE_WINDOW_AND_DATA()\
		return window_data->property;\
	}

DEFINE_WINDOW_INT_GETTER( width )
DEFINE_WINDOW_INT_GETTER( height )
DEFINE_WINDOW_INT_GETTER( x )
DEFINE_WINDOW_INT_GETTER( y )

static void construct_impl( void* win )
{
	RECT window_rect;
	DECLARE_WINDOW_AND_DATA()

	window->cursor = LoadCursor( NULL, IDC_ARROW );
	assert( window->cursor != NULL );

	window->window_class.cbSize =			sizeof( WNDCLASSEX );
	window->window_class.style =			CS_HREDRAW | CS_VREDRAW;
	window->window_class.lpfnWndProc =		&WindowProc;
	window->window_class.cbClsExtra =		0;
	window->window_class.cbWndExtra =		0;
	window->window_class.hInstance =		GetModuleHandle( NULL );
	window->window_class.hIcon =			LoadIcon( GetModuleHandle( NULL ), IDI_APPLICATION );
	window->window_class.hCursor =			window->cursor;
	window->window_class.hbrBackground =	NULL;
	window->window_class.lpszMenuName =		NULL;
	window->window_class.lpszClassName =	"GameWindowClass";
	window->window_class.hIconSm =			NULL;
	window->class_atom = RegisterClassEx( &window->window_class );

	window->handle = CreateWindowEx( WS_EX_APPWINDOW,
									 "GameWindowClass",
									 window_data->caption,
									 WS_OVERLAPPEDWINDOW ^ WS_SIZEBOX ^ WS_MAXIMIZEBOX,
									 window_data->x, window_data->y,
									 window_data->width, window_data->height,
									 NULL, NULL,
									 GetModuleHandle( NULL ),
									 NULL );
	assert( window->handle );

	SetWindowLongPtr( window->handle, GWLP_USERDATA, (LONG)window );

	ShowWindow( window->handle, SW_SHOW );

	GetWindowRect( window->handle, &window_rect );
	window_data->actual_x_offset = window_data->x - window_rect.left;
	window_data->actual_y_offset = window_data->y - window_rect.top;
	/* TODO: Fix this mess */
	window_data->x = window_data->x - window_data->actual_x_offset;
	window_data->y = window_data->y - window_data->actual_y_offset;

	correct_window_size( window );

	DragAcceptFiles( window->handle, TRUE );

	{
		WinAPIInputDevice** iter = NULL;
		while ((iter = vector_next( &window_data->devices, iter )))
		{
			(*iter)->handle = window->handle;
			(*iter)->construct( (*iter) );
		}
	}

	window_data->mapped = 1;
}

static void destruct_impl( void* win )
{
	BOOL result;
	DECLARE_WINDOW_AND_DATA()

	if (!window_data->mapped)
		return;

	window_data->mapped = 0;

	result = DestroyWindow( window->handle );
	assert( result );

	/* We probably don't want to do this as class names are hard-coded and this could unregister
	   a class that is currently in use */
/*	result = UnregisterClass( "GameWindowClass", GetModuleHandle( NULL ) );
	assert( result ); */
}

LRESULT CALLBACK WindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	WinWindow* window;
	WinWindowData* window_data;
	WindowEvent event;

	window = (WinWindow*)GetWindowLongPtr( hwnd, GWLP_USERDATA );

	if (window)
	{
		window_data = (WinWindowData*)window->impl_data;
		assert( window_data );

		if (window_data->mapped)
		{
			WinAPIInputDevice** iter = NULL;
			while ((iter = vector_next( &window_data->devices, iter )))
			{
				LRESULT result;
				if ((*iter)->handle_winapi_message( (*iter), uMsg, wParam, lParam, &result ))
					return result;
			}
		}

		switch (uMsg)
		{
			case WM_MOVE:
				window_data->x = LOWORD( lParam ) - window_data->actual_x_offset;
				window_data->y = HIWORD( lParam ) - window_data->actual_y_offset;
				break;

			case WM_SIZE:
				window_data->width = LOWORD( lParam );
				window_data->height = HIWORD( lParam );
				break;

			case WM_SETCURSOR:
				SetCursor( window->cursor );
				return TRUE;


			case WM_CLOSE:
				event.type = LBWindowEventClose;
				vector_push_back( &window_data->events, &event );
				return TRUE;

			case WM_DROPFILES:
				{
					POINT point;
					int i;
					char filepath[MAX_FILE_PATH + 1];

					event.type = LBWindowEventFileDrop;

					DragQueryPoint( (HDROP)wParam, &point );
					event.x = point.x;
					event.y = point.y;

					event.files = DragQueryFile( (HDROP)wParam, 0xFFFFFFFF, NULL, 0 );
					assert( event.files > 0 );

					vector_clear( &window_data->filepaths );
					for (i = 0; i < event.files; i++)
					{
						UINT result;
						result = DragQueryFile( (HDROP)wParam, i, filepath, MAX_FILE_PATH );
						assert( result );

						vector_push_back( &window_data->filepaths, &filepath );
					}

					vector_push_back( &window_data->events, &event );
					DragFinish( (HDROP)wParam );
				}
				break;
		}
	}

	return DefWindowProc( hwnd, uMsg, wParam, lParam );
}

int lb_window_next_event( void* win )
{
	MSG msg;
	WindowEvent* event_ptr;
	DECLARE_WINDOW_AND_DATA()

	if (PeekMessage( &msg, window->handle, 0, 0, PM_REMOVE ))
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	event_ptr = (WindowEvent*)vector_next( &window_data->events, NULL );
	if (event_ptr == NULL)
		return 0;

	window_data->event = *event_ptr;
	vector_erase( &window_data->events, event_ptr );

	return 1;
}

enum LBWindowEvent lb_window_get_event_type( void* win )
{
	DECLARE_WINDOW_AND_DATA()
	return window_data->event.type;
}

#define DEFINE_WINDOW_EVENT_GETTER( param )\
	int lb_window_get_event_##param( void* win ) {\
		DECLARE_WINDOW_AND_DATA()\
		return window_data->event.param;\
	}

DEFINE_WINDOW_EVENT_GETTER( x )
DEFINE_WINDOW_EVENT_GETTER( y )
DEFINE_WINDOW_EVENT_GETTER( files )

const char* lb_window_get_event_file( void* win, int i )
{
	const char* file;
	DECLARE_WINDOW_AND_DATA()
	file = (const char*)vector_at( &window_data->filepaths, i );
	assert( file != NULL );
	return file;
}

void lb_window_add_input_device( void* win, void* d )
{
	WinAPIInputDevice* device = (WinAPIInputDevice*)d;
	DECLARE_WINDOW_AND_DATA()

	vector_push_back( &window_data->devices, &device );

	if (window_data->mapped)
	{
		device->handle = window->handle;
		device->construct( device );
	}
}
