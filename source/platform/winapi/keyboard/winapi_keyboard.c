
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include "../../../util/vector.h"
#include "../../../util/unicode.h"
#include "../../keyboard.h"
#include "../winapi_input_device.h"

#include <windows.h>
#include <stdlib.h>
#include <assert.h>

typedef struct KeyboardEventTag
{
	int type;
	int key;
	int unicode;
} KeyboardEvent;

typedef struct WinAPIKeyboardTag
{
	WinAPIInputDevice base;
	Vector events;
	KeyboardEvent event;
} WinAPIKeyboard;

#define CAST_KEYBOARD()\
	WinAPIKeyboard* keyboard = (WinAPIKeyboard*)kbd;\
	assert( keyboard != NULL );

static void construct( void* kbd )
{
	CAST_KEYBOARD()
}

int translate_winapi_vkeycode( DWORD wParam )
{
	switch (wParam)
	{
		case VK_BACK: return KeyBackspace;
		case VK_TAB: return KeyTab;
		case VK_RETURN: return KeyReturn;
		case VK_ESCAPE: return KeyEscape;
		case VK_SPACE: return KeySpace;

		case VK_LEFT: return KeyLeft;
		case VK_RIGHT: return KeyRight;
		case VK_UP: return KeyUp;
		case VK_DOWN: return KeyDown;

		case VK_CONTROL: return KeyLCtrl;
		case VK_LSHIFT: return KeyLShift;
		case VK_RSHIFT: return KeyRShift;
		case VK_LMENU: return KeyLAlt;
		case VK_RMENU: return KeyRAlt;

		case VK_F1: return KeyF1;
		case VK_F2: return KeyF2;
		case VK_F3: return KeyF3;
		case VK_F4: return KeyF4;
		case VK_F5: return KeyF5;
		case VK_F6: return KeyF6;
		case VK_F7: return KeyF7;
		case VK_F8: return KeyF8;
		case VK_F9: return KeyF9;
		case VK_F10: return KeyF10;
		case VK_F11: return KeyF11;
		case VK_F12: return KeyF12;

		case '0': return Key0;
		case '1': return Key1;
		case '2': return Key2;
		case '3': return Key3;
		case '4': return Key4;
		case '5': return Key5;
		case '6': return Key6;
		case '7': return Key7;
		case '8': return Key8;
		case '9': return Key9;

		case 'A': return KeyA;
		case 'B': return KeyB;
		case 'C': return KeyC;
		case 'D': return KeyD;
		case 'E': return KeyE;
		case 'F': return KeyF;
		case 'G': return KeyG;
		case 'H': return KeyH;
		case 'I': return KeyI;
		case 'J': return KeyJ;
		case 'K': return KeyK;
		case 'L': return KeyL;
		case 'M': return KeyM;
		case 'N': return KeyN;
		case 'O': return KeyO;
		case 'P': return KeyP;
		case 'Q': return KeyQ;
		case 'R': return KeyR;
		case 'S': return KeyS;
		case 'T': return KeyT;
		case 'U': return KeyU;
		case 'V': return KeyV;
		case 'W': return KeyW;
		case 'X': return KeyX;
		case 'Y': return KeyY;
		case 'Z': return KeyZ;

		default:
			return -1;
	}
}

static int handle_winapi_message( void* kbd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* result )
{
	KeyboardEvent event;
	CAST_KEYBOARD()

	if (uMsg == WM_KEYDOWN)
	{
		event.type = KeyboardEventKeyPress;
		event.key = translate_winapi_vkeycode( wParam );
		vector_push_back( &keyboard->events, &event );
	}
	else if (uMsg == WM_KEYUP)
	{
		event.type = KeyboardEventKeyRelease;
		event.key = translate_winapi_vkeycode( wParam );
		vector_push_back( &keyboard->events, &event );
	}
	else if (uMsg == WM_CHAR)
	{
		event.type = KeyboardEventUnicode;
		event.unicode = wParam; /* TODO: Actually convert to UTF-32 */
		vector_push_back( &keyboard->events, &event );
		*result = FALSE;
		return 1;
	}

	return 0;
}

void* keyboard_construct()
{
	WinAPIKeyboard* keyboard = (WinAPIKeyboard*)malloc( sizeof( WinAPIKeyboard ) );
	vector_construct( &keyboard->events, sizeof( KeyboardEvent ), 3 );

	keyboard->base.construct = &construct;
	keyboard->base.handle_winapi_message = &handle_winapi_message;

	return keyboard;
}

void keyboard_destruct( void* kbd )
{
	CAST_KEYBOARD()
	vector_destruct( &keyboard->events );
	free( keyboard );
}

int keyboard_constructed( void* keyboard )
{
	return (keyboard != NULL) ? 1 : 0;
}

int keyboard_next_event( void* kbd )
{
	KeyboardEvent* iter;
	CAST_KEYBOARD()

	iter = (KeyboardEvent*)vector_next( &keyboard->events, NULL );
	if (iter != NULL)
	{
		keyboard->event = *iter;
		vector_erase( &keyboard->events, iter );
		return 1;
	}
	else
	{
		return 0;
	}
}

enum KeyboardEvent keyboard_get_event_type( void* kbd )
{
	CAST_KEYBOARD()
	return keyboard->event.type;
}

enum Key keyboard_get_event_key( void* kbd )
{
	CAST_KEYBOARD()
	return keyboard->event.key;
}

int keyboard_get_event_utf32( void* kbd )
{
	CAST_KEYBOARD()
	return keyboard->event.unicode;
}

const char* keyboard_get_event_utf8( void* kbd )
{
	CAST_KEYBOARD()
	return "";
}

