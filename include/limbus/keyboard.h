
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef LB_KEYBOARD_H
#define LB_KEYBOARD_H
#ifdef __cplusplus
extern "C"
{
#endif

typedef void* LBKeyboard;

/** An enumeration of the different keyboard event types. */
enum LBKeyboardEvent
{
	LBKeyboardEventKeyPress,
	LBKeyboardEventKeyRelease,
	LBKeyboardEventUnicode
};
typedef enum LBKeyboardEvent LBKeyboardEvent;


/* TODO: Add caps-lock */
/** An enumeration of the different keys. */
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
typedef enum LBKey LBKey;

/** Constructs a new keyboard object.
  * @return a pointer to the newly created keyboard object if successful,
  * else returns a pointer identifying a non-constructed keyboard object.
  * Use lb_keyboard_constructed to find out if a keyboard object was
  * constructed or not.
  */
LBKeyboard lb_keyboard_construct( void );

/** Destructs a previously constructed keyboard object.
  * Destructing a keyboard object will deallocate all the resources
  * associated with it.
  * @param keyboard a pointer to the keyboard object to destruct.
  */
void lb_keyboard_destruct( LBKeyboard keyboard );

/** Check for successfully constructed keyboard objects.
  * @param keyboard a pointer returned by lb_keyboard_construct
  * or lb_keyboard_destruct
  * @return 1 if keyboard points to a successfully constructed
  * keyboard object or 0 if it doesn't.
  */
int lb_keyboard_constructed( LBKeyboard keyboard );

/** Advances to the next event in the event stream if there is one.
  * @param keyboard a pointer to a successfully constructed keyboard object.
  * @return 1 if a new event is available else returns 0.
  */
int lb_keyboard_next_event( LBKeyboard keyboard );

/** Get the type of the current event.
  * @param keyboard a pointer to a successfully constructed keyboard object.
  * @return the type of the current event.
  */
LBKeyboardEvent lb_keyboard_get_event_type( LBKeyboard keyboard );

/** Get the key associated with the current event.
  * @param keyboard a pointer to a successfully constructed keyboard object.
  * @return on an LBKeyboardEventKeyPress or LBKeyboardEventKeyRelease event,
  * returns the key being pressed or released.
  */
LBKey lb_keyboard_get_event_key( LBKeyboard keyboard );

/** Get the UTF-32 code associated with the current event.
  * @param keyboard a pointer to a successfully constructed keyboard object.
  * @return on an LBKeyboardEventUnicode event, returns the Unicode codepoint
  * encoded in UTF-32.
  */
int lb_keyboard_get_event_utf32( LBKeyboard keyboard );

/** Get the UTF-8 code associated with the current event.
  * @param keyboard a pointer to a successfully constructed keyboard object.
  * @return on an LBKeyboardEventUnicode event, returns the Unicode codepoint
  * encoded in UTF-8 as a zero-terminated string.
  */
const char* lb_keyboard_get_event_utf8( LBKeyboard keyboard );

#ifdef __cplusplus
}
#endif
#endif

