
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
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
static void update_decorations( void* win );

typedef struct
{
	enum LBWindowEvent type;
	int files;
	int x, y;
	int width, height;
} WindowEvent;

typedef struct
{
	WinWindow base;

	LBScreen screen;
	LBScreen initial_screen;
	int mapped;
	char* caption;
	int width, height;
	int x, y;
	int actual_x_offset, actual_y_offset;
	
	DWORD style;
	int styled;
	int resizable;

	WindowEvent event;
	Vector events;
	Vector filepaths;

	Vector devices;
} WinWindowImpl;

void* lb_window_construct()
{
	WinWindowImpl* window;

	window = (WinWindowImpl*)malloc( sizeof *window );
	assert( window );

	window->base.construct_impl = &construct_impl;

	window->screen = lb_screen_construct( LBScreenDefault );
	window->initial_screen = window->screen;

	window->mapped = 0;

	window->x = CW_USEDEFAULT;
	window->y = CW_USEDEFAULT;
	window->actual_x_offset = 0;
	window->actual_y_offset = 0;

	window->width = 640;
	window->height = 480;

	vector_construct( &window->events, sizeof( WindowEvent ) );
	vector_construct( &window->devices, sizeof( WinAPIInputDevice* ) );
	vector_construct( &window->filepaths, MAX_FILE_PATH + 1 );

	window->caption = 0;
	lb_window_set_caption( window, "" );
	
	window->styled = 1;
	window->resizable = 0;
	update_decorations( window );

	return window;
}

void* lb_window_destruct( void* win )
{
	WinWindowImpl* window = (WinWindowImpl*)win;

	if (window->mapped)
		destruct_impl( window );

	if (window->caption)
		free( window->caption );

	vector_destruct( &window->filepaths );
	vector_destruct( &window->events );
	vector_destruct( &window->devices );
	
	lb_screen_destruct( window->initial_screen );

	free( window );

	return NULL;
}

int lb_window_constructed( LBWindow window )
{
	return window != NULL;
}

void lb_window_set_screen( void* win, LBScreen screen )
{
	WinWindowImpl* window = (WinWindowImpl*)win;
	window->screen = screen;
}

LBScreen lb_window_get_screen( void* win )
{
	WinWindowImpl* window = (WinWindowImpl*)win;
	return window->screen;
}

void update_decorations( void* win )
{
	WinWindowImpl* window = (WinWindowImpl*)win;
	
	window->style = WS_OVERLAPPEDWINDOW ^ WS_SIZEBOX ^ WS_MAXIMIZEBOX;
	
	if (window->resizable)
	{
		window->style |= WS_SIZEBOX | WS_MAXIMIZEBOX;
	}
	
	if (!window->styled)
	{
		window->style = WS_POPUP;
	}

	if (window->mapped)
	{
		/* Assume this works */
		SetWindowLongPtr( window->base.handle, GWL_STYLE, window->style );
	}
}

void lb_window_set_styled( void* win, int value )
{
	WinWindowImpl* window = (WinWindowImpl*)win;
	window->styled = value;
	update_decorations( win );
}

int lb_window_get_styled( void* win )
{
	WinWindowImpl* window = (WinWindowImpl*)win;
	return window->styled;
}

void lb_window_set_resizable( void* win, int value )
{
	WinWindowImpl* window = (WinWindowImpl*)win;
	window->resizable = value;
	update_decorations( win );
}

int lb_window_get_resizable( void* win )
{
	WinWindowImpl* window = (WinWindowImpl*)win;
	return window->resizable;
}

void lb_window_set_caption( void* win, const char* caption )
{
	int size;
	WinWindowImpl* window = (WinWindowImpl*)win;

	if (window->caption)
		free( window->caption );

	size = strlen( caption ) + 1;
	window->caption = (char*)malloc( size );
	if (!window->caption)
		return;
	strcpy( window->caption, caption );

	if (window->mapped)
		SetWindowText( window->base.handle, window->caption );
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
	WinWindowImpl* window = (WinWindowImpl*)win;

	handle = window->base.handle;

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
	WinWindowImpl* window = (WinWindowImpl*)win;
	pos = &window->x;
	MoveWindow( window->base.handle, window->x,
								window->y,
								window->width,
								window->height,
								TRUE );
 	correct_window_size( window );
}

#define DEFINE_WINDOW_INT_SETTER( property )\
	void lb_window_set_##property( void* win, int property ) {\
		WinWindowImpl* window = (WinWindowImpl*)win;\
		window->property = property;\
		if (window->mapped)\
			update_window_rect( win );\
	}

DEFINE_WINDOW_INT_SETTER( width )
DEFINE_WINDOW_INT_SETTER( height )
DEFINE_WINDOW_INT_SETTER( x )
DEFINE_WINDOW_INT_SETTER( y )

const char* lb_window_get_caption( void* win )
{
	WinWindowImpl* window = (WinWindowImpl*)win;
	return (const char*)window->caption;
}

#define DEFINE_WINDOW_INT_GETTER( property )\
	int lb_window_get_##property( void* win ) {\
		WinWindowImpl* window = (WinWindowImpl*)win;\
		return window->property;\
	}

DEFINE_WINDOW_INT_GETTER( width )
DEFINE_WINDOW_INT_GETTER( height )
DEFINE_WINDOW_INT_GETTER( x )
DEFINE_WINDOW_INT_GETTER( y )

static void construct_impl( void* win )
{
	RECT window_rect;
	WinWindowImpl* window = (WinWindowImpl*)win;

	window->base.cursor = LoadCursor( NULL, IDC_ARROW );
	assert( window->base.cursor != NULL );

	window->base.window_class.cbSize =			sizeof( WNDCLASSEX );
	window->base.window_class.style =			CS_HREDRAW | CS_VREDRAW;
	window->base.window_class.lpfnWndProc =		&WindowProc;
	window->base.window_class.cbClsExtra =		0;
	window->base.window_class.cbWndExtra =		0;
	window->base.window_class.hInstance =		GetModuleHandle( NULL );
	window->base.window_class.hIcon =			LoadIcon( GetModuleHandle( NULL ), IDI_APPLICATION );
	window->base.window_class.hCursor =			window->base.cursor;
	window->base.window_class.hbrBackground =	NULL;
	window->base.window_class.lpszMenuName =		NULL;
	window->base.window_class.lpszClassName =	"GameWindowClass";
	window->base.window_class.hIconSm =			NULL;
	window->base.class_atom = RegisterClassEx( &window->base.window_class );

	window->base.handle = CreateWindowEx( WS_EX_APPWINDOW,
									 "GameWindowClass",
									 window->caption,
									 window->style,
									 window->x, window->y,
									 window->width, window->height,
									 NULL, NULL,
									 GetModuleHandle( NULL ),
									 NULL );
	assert( window->base.handle );

	SetWindowLongPtr( window->base.handle, GWLP_USERDATA, (LONG)window );

	ShowWindow( window->base.handle, SW_SHOW );

	GetWindowRect( window->base.handle, &window_rect );
	window->actual_x_offset = window->x - window_rect.left;
	window->actual_y_offset = window->y - window_rect.top;
	/* TODO: Fix this mess */
	window->x = window->x - window->actual_x_offset;
	window->y = window->y - window->actual_y_offset;

	correct_window_size( window );

	DragAcceptFiles( window->base.handle, TRUE );

	{
		WinAPIInputDevice** iter = NULL;
		while ((iter = (WinAPIInputDevice**)vector_next( &window->devices, iter )))
		{
			(*iter)->handle = window->base.handle;
			(*iter)->construct( (*iter) );
		}
	}

	window->mapped = 1;
}

static void destruct_impl( void* win )
{
	BOOL result;
	WinWindowImpl* window = (WinWindowImpl*)win;

	if (!window->mapped)
		return;

	window->mapped = 0;

	result = DestroyWindow( window->base.handle );
	assert( result );

	/* We probably don't want to do this as class names are hard-coded and this could unregister
	   a class that is currently in use */
/*	result = UnregisterClass( "GameWindowClass", GetModuleHandle( NULL ) );
	assert( result ); */
}

LRESULT CALLBACK WindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	WinWindowImpl* window;
	WindowEvent event;

	window = (WinWindowImpl*)GetWindowLongPtr( hwnd, GWLP_USERDATA );

	if (window)
	{
		if (window->mapped)
		{
			WinAPIInputDevice** iter = NULL;
			while ((iter = (WinAPIInputDevice**)vector_next( &window->devices, iter )))
			{
				LRESULT result;
				if ((*iter)->handle_winapi_message( (*iter), uMsg, wParam, lParam, &result ))
					return result;
			}
		}

		switch (uMsg)
		{
			case WM_MOVE:
				window->x = LOWORD( lParam ) - window->actual_x_offset;
				window->y = HIWORD( lParam ) - window->actual_y_offset;
				return 0;

			case WM_SIZE:
				event.type = LBWindowEventResize;
				event.width = window->width;
				event.height = window->height;
				vector_push_back( &window->events, &event );

				window->width = LOWORD( lParam );
				window->height = HIWORD( lParam );
				return 0;

			case WM_SETCURSOR:
				SetCursor( window->base.cursor );
				return TRUE;

			case WM_CLOSE:
				event.type = LBWindowEventClose;
				vector_push_back( &window->events, &event );
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

					vector_clear( &window->filepaths );
					for (i = 0; i < event.files; i++)
					{
						UINT result;
						result = DragQueryFile( (HDROP)wParam, i, filepath, MAX_FILE_PATH );
						assert( result );

						vector_push_back( &window->filepaths, &filepath );
					}

					vector_push_back( &window->events, &event );
					DragFinish( (HDROP)wParam );
				}
				return 0;
		}
	}

	return DefWindowProc( hwnd, uMsg, wParam, lParam );
}

int lb_window_next_event( void* win )
{
	MSG msg;
	WindowEvent* event_ptr;
	WinWindowImpl* window = (WinWindowImpl*)win;

	while (PeekMessage( &msg, window->base.handle, 0, 0, PM_REMOVE ))
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	event_ptr = (WindowEvent*)vector_next( &window->events, NULL );
	if (event_ptr == NULL)
		return 0;

	window->event = *event_ptr;
	vector_erase( &window->events, event_ptr );

	return 1;
}

enum LBWindowEvent lb_window_get_event_type( void* win )
{
	WinWindowImpl* window = (WinWindowImpl*)win;
	return window->event.type;
}

#define DEFINE_WINDOW_EVENT_GETTER( param )\
	int lb_window_get_event_##param( void* win ) {\
		WinWindowImpl* window = (WinWindowImpl*)win;\
		return window->event.param;\
	}

DEFINE_WINDOW_EVENT_GETTER( x )
DEFINE_WINDOW_EVENT_GETTER( y )
DEFINE_WINDOW_EVENT_GETTER( files )

const char* lb_window_get_event_file( void* win, int i )
{
	const char* file;
	WinWindowImpl* window = (WinWindowImpl*)win;
	file = (const char*)vector_at( &window->filepaths, i );
	assert( file != NULL );
	return file;
}

int lb_window_get_event_width( void* win )
{
	WinWindowImpl* window = (WinWindowImpl*)win;
	return window->event.width;
}

int lb_window_get_event_height( void* win )
{
	WinWindowImpl* window = (WinWindowImpl*)win;
	return window->event.height;
}

void lb_window_add_input_device( void* win, void* d )
{
	WinAPIInputDevice* device = (WinAPIInputDevice*)d;
	WinWindowImpl* window = (WinWindowImpl*)win;

	vector_push_back( &window->devices, &device );

	if (window->mapped)
	{
		device->handle = window->base.handle;
		device->construct( device );
	}
}
