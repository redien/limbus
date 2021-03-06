
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include "../../../util/vector.h"
#include "../../../util/unicode.h"
#include <limbus/keyboard.h>
#include "../winapi_input_device.h"

#include <windows.h>
#include <stdlib.h>
#include <assert.h>

typedef struct KeyboardEventTag
{
	int type;
	int key;
	WCHAR unicode;
} KeyboardEvent;

typedef struct WinAPIKeyboardTag
{
	WinAPIInputDevice base;
	Vector events;
	KeyboardEvent event;
	char utf8[5];
} WinAPIKeyboard;

#define CAST_KEYBOARD()\
	WinAPIKeyboard* keyboard = (WinAPIKeyboard*)kbd;\
	assert( keyboard != NULL );

static void construct( void* kbd )
{
	CAST_KEYBOARD()
}

#define IS_EXTENDED( lParam ) (0x1000000 & lParam)
	int printf( const char*, ... );

int translate_winapi_vkeycode( DWORD wParam, DWORD lParam )
{
	switch (wParam)
	{
		case VK_BACK: return LBKeyBackspace;
		case VK_TAB: return LBKeyTab;
		case VK_RETURN: return LBKeyReturn;
		case VK_ESCAPE: return LBKeyEscape;
		case VK_SPACE: return LBKeySpace;

		case VK_LEFT: return LBKeyLeft;
		case VK_RIGHT: return LBKeyRight;
		case VK_UP: return LBKeyUp;
		case VK_DOWN: return LBKeyDown;

		case VK_CONTROL:
			if (IS_EXTENDED( lParam ))
				return LBKeyRCtrl;
			else
				return LBKeyLCtrl;

		case VK_SHIFT:
			if (IS_EXTENDED( lParam ))
				return LBKeyRShift;
			else
				return LBKeyLShift;

		case VK_MENU:
			if (IS_EXTENDED( lParam ))
				return LBKeyRAlt;
			else
				return LBKeyLAlt;

		case VK_F1: return LBKeyF1;
		case VK_F2: return LBKeyF2;
		case VK_F3: return LBKeyF3;
		case VK_F4: return LBKeyF4;
		case VK_F5: return LBKeyF5;
		case VK_F6: return LBKeyF6;
		case VK_F7: return LBKeyF7;
		case VK_F8: return LBKeyF8;
		case VK_F9: return LBKeyF9;
		case VK_F10: return LBKeyF10;
		case VK_F11: return LBKeyF11;
		case VK_F12: return LBKeyF12;

		case '0': return LBKey0;
		case '1': return LBKey1;
		case '2': return LBKey2;
		case '3': return LBKey3;
		case '4': return LBKey4;
		case '5': return LBKey5;
		case '6': return LBKey6;
		case '7': return LBKey7;
		case '8': return LBKey8;
		case '9': return LBKey9;

		case 'A': return LBKeyA;
		case 'B': return LBKeyB;
		case 'C': return LBKeyC;
		case 'D': return LBKeyD;
		case 'E': return LBKeyE;
		case 'F': return LBKeyF;
		case 'G': return LBKeyG;
		case 'H': return LBKeyH;
		case 'I': return LBKeyI;
		case 'J': return LBKeyJ;
		case 'K': return LBKeyK;
		case 'L': return LBKeyL;
		case 'M': return LBKeyM;
		case 'N': return LBKeyN;
		case 'O': return LBKeyO;
		case 'P': return LBKeyP;
		case 'Q': return LBKeyQ;
		case 'R': return LBKeyR;
		case 'S': return LBKeyS;
		case 'T': return LBKeyT;
		case 'U': return LBKeyU;
		case 'V': return LBKeyV;
		case 'W': return LBKeyW;
		case 'X': return LBKeyX;
		case 'Y': return LBKeyY;
		case 'Z': return LBKeyZ;

		default:
			return -1;
	}
}

static int handle_winapi_message( void* kbd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* result )
{
	KeyboardEvent event;
	CAST_KEYBOARD()

	if (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN)
	{
		event.type = LBKeyboardEventKeyPress;
		event.key = translate_winapi_vkeycode( wParam, lParam );
		vector_push_back( &keyboard->events, &event );
		*result = 0;
		return 1;
	}
	else if (uMsg == WM_KEYUP || uMsg == WM_SYSKEYUP)
	{
		event.type = LBKeyboardEventKeyRelease;
		event.key = translate_winapi_vkeycode( wParam, lParam );
		vector_push_back( &keyboard->events, &event );
		*result = 0;
		return 1;
	}
	else if (uMsg == WM_CHAR)
	{
		event.type = LBKeyboardEventUnicode;
		event.unicode = wParam;
		vector_push_back( &keyboard->events, &event );
		*result = 0;
		return 1;
	}

	return 0;
}

void* lb_keyboard_construct()
{
	WinAPIKeyboard* keyboard = (WinAPIKeyboard*)malloc( sizeof( WinAPIKeyboard ) );
	vector_construct( &keyboard->events, sizeof( KeyboardEvent ) );

	keyboard->base.construct = &construct;
	keyboard->base.handle_winapi_message = &handle_winapi_message;

	return keyboard;
}

void lb_keyboard_destruct( void* kbd )
{
	CAST_KEYBOARD()
	vector_destruct( &keyboard->events );
	free( keyboard );
}

int lb_keyboard_constructed( void* keyboard )
{
	return (keyboard != NULL) ? 1 : 0;
}

int lb_keyboard_next_event( void* kbd )
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

enum LBKeyboardEvent lb_keyboard_get_event_type( void* kbd )
{
	CAST_KEYBOARD()
	return keyboard->event.type;
}

enum LBKey lb_keyboard_get_event_key( void* kbd )
{
	CAST_KEYBOARD()
	return keyboard->event.key;
}

int lb_keyboard_get_event_utf32( void* kbd )
{
	CAST_KEYBOARD()
	return keyboard->event.unicode; /* TODO: Actually convert to UTF-32.... or use WM_UNICHAR? */
}

const char* lb_keyboard_get_event_utf8( void* kbd )
{
	size_t written;
	CAST_KEYBOARD();
	written = WideCharToMultiByte( CP_UTF8, 0, &keyboard->event.unicode, 1, keyboard->utf8, 4, NULL, NULL );
	keyboard->utf8[written] = '\0';
	return keyboard->utf8;
}
