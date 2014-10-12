
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
#include "../sdl_input_device.h"

#include <limbus/keyboard.h>

#include <stdlib.h>
#include <assert.h>

typedef struct
{
	int type;
	int key;
	int unicode;
	char utf8[5];
} KeyboardEvent;

typedef struct SDLKeyboard
{
	SDLInputDevice base;
	Vector events;
	KeyboardEvent event;
} SDLKeyboard;

static void construct( void* kbd )
{
	SDLKeyboard* keyboard = (SDLKeyboard*)kbd;
}

static int translate_sdl_keycode( SDL_Keycode keycode )
{
	switch (keycode)
	{
		case SDLK_BACKSPACE: return LBKeyBackspace;
		case SDLK_TAB: return LBKeyTab;
		case SDLK_RETURN: return LBKeyReturn;
		case SDLK_ESCAPE: return LBKeyEscape;
		case SDLK_SPACE: return LBKeySpace;

		case SDLK_LEFT: return LBKeyLeft;
		case SDLK_RIGHT: return LBKeyRight;
		case SDLK_UP: return LBKeyUp;
		case SDLK_DOWN: return LBKeyDown;

		case SDLK_LCTRL: return LBKeyLCtrl;
		case SDLK_RCTRL: return LBKeyRCtrl;
		case SDLK_LSHIFT: return LBKeyLShift;
		case SDLK_RSHIFT: return LBKeyRShift;
		case SDLK_LALT: return LBKeyLAlt;
		case SDLK_RALT: return LBKeyRAlt;

		case SDLK_F1: return LBKeyF1;
		case SDLK_F2: return LBKeyF2;
		case SDLK_F3: return LBKeyF3;
		case SDLK_F4: return LBKeyF4;
		case SDLK_F5: return LBKeyF5;
		case SDLK_F6: return LBKeyF6;
		case SDLK_F7: return LBKeyF7;
		case SDLK_F8: return LBKeyF8;
		case SDLK_F9: return LBKeyF9;
		case SDLK_F10: return LBKeyF10;
		case SDLK_F11: return LBKeyF11;
		case SDLK_F12: return LBKeyF12;

		case SDLK_0: return LBKey0;
		case SDLK_1: return LBKey1;
		case SDLK_2: return LBKey2;
		case SDLK_3: return LBKey3;
		case SDLK_4: return LBKey4;
		case SDLK_5: return LBKey5;
		case SDLK_6: return LBKey6;
		case SDLK_7: return LBKey7;
		case SDLK_8: return LBKey8;
		case SDLK_9: return LBKey9;

		case SDLK_a: return LBKeyA;
		case SDLK_b: return LBKeyB;
		case SDLK_c: return LBKeyC;
		case SDLK_d: return LBKeyD;
		case SDLK_e: return LBKeyE;
		case SDLK_f: return LBKeyF;
		case SDLK_g: return LBKeyG;
		case SDLK_h: return LBKeyH;
		case SDLK_i: return LBKeyI;
		case SDLK_j: return LBKeyJ;
		case SDLK_k: return LBKeyK;
		case SDLK_l: return LBKeyL;
		case SDLK_m: return LBKeyM;
		case SDLK_n: return LBKeyN;
		case SDLK_o: return LBKeyO;
		case SDLK_p: return LBKeyP;
		case SDLK_q: return LBKeyQ;
		case SDLK_r: return LBKeyR;
		case SDLK_s: return LBKeyS;
		case SDLK_t: return LBKeyT;
		case SDLK_u: return LBKeyU;
		case SDLK_v: return LBKeyV;
		case SDLK_w: return LBKeyW;
		case SDLK_x: return LBKeyX;
		case SDLK_y: return LBKeyY;
		case SDLK_z: return LBKeyZ;

		default:
			return -1;
	}
}

static void handle_sdl_event( void* kbd, SDL_Event* event )
{
	SDLKeyboard* keyboard = (SDLKeyboard*)kbd;
    KeyboardEvent key_event;

	if (event->type == SDL_KEYDOWN)
	{
		key_event.type = LBKeyboardEventKeyPress;
	}

	if (event->type == SDL_KEYUP)
	{
		key_event.type = LBKeyboardEventKeyRelease;
	}

    key_event.key = translate_sdl_keycode( event->key.keysym.sym );
	vector_push_back( &keyboard->events, &key_event );
}


void* lb_keyboard_construct()
{
	SDLKeyboard* keyboard = (SDLKeyboard*)malloc( sizeof( SDLKeyboard ) );
	vector_construct( &keyboard->events, sizeof( KeyboardEvent ) );

	keyboard->base.construct = &construct;
	keyboard->base.handle_sdl_event = &handle_sdl_event;

	return keyboard;
}

void lb_keyboard_destruct( void* kbd )
{
	SDLKeyboard* keyboard = (SDLKeyboard*)kbd;
	vector_destruct( &keyboard->events );
	free( keyboard );
}

int lb_keyboard_constructed( void* keyboard )
{
	return (keyboard != NULL) ? 1 : 0;
}

int lb_keyboard_next_event( void* kbd )
{
	SDLKeyboard* keyboard = (SDLKeyboard*)kbd;
	KeyboardEvent* iter;

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
	SDLKeyboard* keyboard = (SDLKeyboard*)kbd;
	return keyboard->event.type;
}

enum LBKey lb_keyboard_get_event_key( void* kbd )
{
	SDLKeyboard* keyboard = (SDLKeyboard*)kbd;
	return keyboard->event.key;
}

int lb_keyboard_get_event_utf32( void* kbd )
{
	SDLKeyboard* keyboard = (SDLKeyboard*)kbd;
	return keyboard->event.unicode;
}

const char* lb_keyboard_get_event_utf8( void* kbd )
{
	SDLKeyboard* keyboard = (SDLKeyboard*)kbd;
	return keyboard->event.utf8;
}

