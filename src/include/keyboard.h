#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#ifndef KERNEL
#include "userCall.h"
#define _PREFIX static inline
#else
#define _PREFIX
#endif

// US QWERTY
enum {KEY_NONE, KEY_ESC=128, KEY_BACKSPACE, KEY_TAB, KEY_ENTER, KEY_LCTRL,
	KEY_LSHIFT, KEY_RSHIFT, KEY_NUM_STAR, KEY_LALT, KEY_CAPSLOCK, KEY_F1,
	KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10,
	KEY_NUMLOCK, KEY_SCROLLLOCK, KEY_NUM7, KEY_NUM8, KEY_NUM9, KEY_NUM_MINUS,
	KEY_NUM4, KEY_NUM5, KEY_NUM6, KEY_NUM_PLUS, KEY_NUM1, KEY_NUM2, KEY_NUM3,
	KEY_NUM0, KEY_NUM_DOT, KEY_F11, KEY_F12, KEY_SPECIAL, KEY_UP, KEY_LEFT,
	KEY_RIGHT, KEY_DOWN};
	
#define KEY_RELEASED 0
#define KEY_PRESSED 1

static const uint8_t KEYS[] = {
	0, KEY_ESC, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
	KEY_BACKSPACE, KEY_TAB, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o',
	'p', '[', ']', KEY_ENTER, KEY_LCTRL, 'a', 's', 'd', 'f', 'g', 'h',
	'j', 'k', 'l', ';', '\'', '`', KEY_LSHIFT, '\\', 'z', 'x', 'c', 'v',
	'b', 'n', 'm', ',', '.', '/', KEY_RSHIFT, KEY_NUM_STAR, KEY_LALT, ' ',
	KEY_CAPSLOCK, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7,
	KEY_F8, KEY_F9, KEY_F10, KEY_NUMLOCK, KEY_SCROLLLOCK, 0, 0, 0, KEY_F11,
	KEY_F12};

#define KEY_SIZE sizeof(KEYS)/sizeof(KEYS[0])
	
struct {
	uint8_t id, key;
} static const specialKeys[] = {
	{0x48, KEY_UP}, {0x4B, KEY_LEFT}, {0x4D, KEY_RIGHT}, {0x50, KEY_DOWN}};
	
#define KEY_SPECIAL_ID 0xE0
#define KEY_PRE_REL 0x80
#define KEY_CACHE 32

#ifdef KERNEL
uint8_t isKeyEvent();
uint8_t getKey();
void keyboard();
void translateKeyCodes();
#endif

_PREFIX int8_t getKeyStatus(uint8_t key)
#ifndef KERNEL
{return call(5, key, 0, 0, 0);}
#endif
;
_PREFIX uint8_t pollKeys()
#ifndef KERNEL
{return call(3, 0, 0, 0, 0);}
#endif
;

_PREFIX uint8_t keyID(uint8_t key)
#ifndef KERNEL
{return call(4, key, 0, 0, 0);}
#endif
;

#endif // KEYBOARD_H_INCLUDED
