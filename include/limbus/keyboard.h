
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef _LB_KEYBOARD_H_
#define _LB_KEYBOARD_H_
#ifdef __cplusplus
extern "C"
{
#endif

enum LBKeyboardEvent
{
	LBKeyboardEventKeyPress,
	LBKeyboardEventKeyRelease,
	LBKeyboardEventUnicode
};

enum LBKey
{
	LBKeyReturn, LBKeyEscape, LBKeyTab, LBKeySpace, LBKeyBackspace,
	LBKeyLeft, LBKeyRight, LBKeyUp, LBKeyDown,
	LBKeyLCtrl, LBKeyRCtrl, LBKeyLShift, LBKeyRShift, LBKeyLAlt, LBKeyRAlt,

	LBKeyA, LBKeyB, LBKeyC, LBKeyD, LBKeyE, LBKeyF, LBKeyG, LBKeyH, LBKeyI,
	LBKeyJ, LBKeyK, LBKeyL, LBKeyM, LBKeyN, LBKeyO, LBKeyP, LBKeyQ, LBKeyR,
	LBKeyS, LBKeyT, LBKeyU, LBKeyV, LBKeyW, LBKeyX, LBKeyY, LBKeyZ,

	LBKey0, LBKey1, LBKey2, LBKey3, LBKey4, LBKey5, LBKey6, LBKey7, LBKey8,
	LBKey9, LBKeyF1, LBKeyF2, LBKeyF3, LBKeyF4, LBKeyF5, LBKeyF6, LBKeyF7,
	LBKeyF8, LBKeyF9, LBKeyF10, LBKeyF11, LBKeyF12
};

void* lb_keyboard_construct( void );
void lb_keyboard_destruct( void* keyboard );
int lb_keyboard_constructed( void* keyboard );

int lb_keyboard_next_event( void* keyboard );
enum LBKeyboardEvent lb_keyboard_get_event_type( void* keyboard );
enum LBKey lb_keyboard_get_event_key( void* keyboard );
int lb_keyboard_get_event_utf32( void* keyboard );
const char* lb_keyboard_get_event_utf8( void* keyboard );

#ifdef __cplusplus
}
#endif
#endif

