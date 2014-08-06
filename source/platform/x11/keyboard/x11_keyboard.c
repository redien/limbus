
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
#include "../x11_input_device.h"

#include <limbus/keyboard.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <assert.h>

typedef struct KeyboardEventTag
{
	int type;
	int key;
	int unicode;
	char utf8[5];
	Time time;
} KeyboardEvent;

typedef struct X11KeyboardTag
{
	X11InputDevice base;
	Vector events;
	KeyboardEvent event;
} X11Keyboard;

#define CAST_KEYBOARD()\
	X11Keyboard* keyboard = (X11Keyboard*)kbd;\
	assert( keyboard != NULL );

static void construct( void* kbd )
{
	CAST_KEYBOARD()
}

static int translate_x11_keysym( KeySym keysym )
{
	switch (keysym)
	{
		case XK_BackSpace: return LBKeyBackspace;
		case XK_Tab: return LBKeyTab;
		case XK_Return: return LBKeyReturn;
		case XK_Escape: return LBKeyEscape;
		case XK_space: return LBKeySpace;

		case XK_Left: return LBKeyLeft;
		case XK_Right: return LBKeyRight;
		case XK_Up: return LBKeyUp;
		case XK_Down: return LBKeyDown;

		case XK_Control_L: return LBKeyLCtrl;
		case XK_Control_R: return LBKeyRCtrl;
		case XK_Shift_L: return LBKeyLShift;
		case XK_Shift_R: return LBKeyRShift;
		case XK_Alt_L: return LBKeyLAlt;
		case XK_Alt_R: return LBKeyRAlt;

		case XK_F1: return LBKeyF1;
		case XK_F2: return LBKeyF2;
		case XK_F3: return LBKeyF3;
		case XK_F4: return LBKeyF4;
		case XK_F5: return LBKeyF5;
		case XK_F6: return LBKeyF6;
		case XK_F7: return LBKeyF7;
		case XK_F8: return LBKeyF8;
		case XK_F9: return LBKeyF9;
		case XK_F10: return LBKeyF10;
		case XK_F11: return LBKeyF11;
		case XK_F12: return LBKeyF12;

		case XK_0: return LBKey0;
		case XK_1: return LBKey1;
		case XK_2: return LBKey2;
		case XK_3: return LBKey3;
		case XK_4: return LBKey4;
		case XK_5: return LBKey5;
		case XK_6: return LBKey6;
		case XK_7: return LBKey7;
		case XK_8: return LBKey8;
		case XK_9: return LBKey9;

		case XK_A: case XK_a: return LBKeyA;
		case XK_B: case XK_b: return LBKeyB;
		case XK_C: case XK_c: return LBKeyC;
		case XK_D: case XK_d: return LBKeyD;
		case XK_E: case XK_e: return LBKeyE;
		case XK_F: case XK_f: return LBKeyF;
		case XK_G: case XK_g: return LBKeyG;
		case XK_H: case XK_h: return LBKeyH;
		case XK_I: case XK_i: return LBKeyI;
		case XK_J: case XK_j: return LBKeyJ;
		case XK_K: case XK_k: return LBKeyK;
		case XK_L: case XK_l: return LBKeyL;
		case XK_M: case XK_m: return LBKeyM;
		case XK_N: case XK_n: return LBKeyN;
		case XK_O: case XK_o: return LBKeyO;
		case XK_P: case XK_p: return LBKeyP;
		case XK_Q: case XK_q: return LBKeyQ;
		case XK_R: case XK_r: return LBKeyR;
		case XK_S: case XK_s: return LBKeyS;
		case XK_T: case XK_t: return LBKeyT;
		case XK_U: case XK_u: return LBKeyU;
		case XK_V: case XK_v: return LBKeyV;
		case XK_W: case XK_w: return LBKeyW;
		case XK_X: case XK_x: return LBKeyX;
		case XK_Y: case XK_y: return LBKeyY;
		case XK_Z: case XK_z: return LBKeyZ;

		default:
			return -1;
	}
}

static void handle_x11_event( void* kbd, XEvent event )
{
	XKeyEvent* xkey_event = (XKeyEvent*)&event;
	int printf(char*,...);
	CAST_KEYBOARD()

	if (event.type == KeyPress)
	{
		KeyboardEvent key_event,
					  unicode_event,
					  *iter = NULL,
					  *last_release_event = NULL;
		int len, i;
		KeySym keysym;
		Status temp_status;

		len = Xutf8LookupString( keyboard->base.input_context, xkey_event,
								 unicode_event.utf8, 4, &keysym, &temp_status );
		unicode_event.utf8[len] = 0;

		/* Filter out release events on key-repeat */
		for (i = vector_size( &keyboard->events ) - 1;
			 i >= 0;
			 i--)
		{
			iter = (KeyboardEvent*)vector_at( &keyboard->events, (unsigned int)i );
			if (iter->type == LBKeyboardEventKeyRelease)
			{
				last_release_event = iter;
				break;
			}
		}
		if (last_release_event
		&& last_release_event->key == translate_x11_keysym( keysym ))
		{
			if ((xkey_event->time - last_release_event->time) <= 1)
			{
				vector_erase( &keyboard->events, last_release_event );
			}
		}

		key_event.type = LBKeyboardEventKeyPress;
		key_event.key = translate_x11_keysym( keysym );
		vector_push_back( &keyboard->events, &key_event );

		unicode_event.type = LBKeyboardEventUnicode;
		utf8to32( unicode_event.utf8, &unicode_event.unicode, 1 );
		if (unicode_event.unicode != 0)
			vector_push_back( &keyboard->events, &unicode_event );
	}

	if (event.type == KeyRelease)
	{
		KeyboardEvent key_event;
		key_event.type = LBKeyboardEventKeyRelease;
		key_event.key = translate_x11_keysym( XLookupKeysym( xkey_event, 0 ) );
		key_event.time = xkey_event->time;
		vector_push_back( &keyboard->events, &key_event );
	}
}


void* lb_keyboard_construct()
{
	X11Keyboard* keyboard = (X11Keyboard*)malloc( sizeof( X11Keyboard ) );
	vector_construct( &keyboard->events, sizeof( KeyboardEvent ) );

	keyboard->base.construct = &construct;
	keyboard->base.handle_x11_event = &handle_x11_event;

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
	return keyboard->event.unicode;
}

const char* lb_keyboard_get_event_utf8( void* kbd )
{
	CAST_KEYBOARD()
	return keyboard->event.utf8;
}

