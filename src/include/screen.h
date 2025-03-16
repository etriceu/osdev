#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>
#include <stddef.h>

#ifndef KERNEL
#include "userCall.h"
#define _PREFIX static inline
#else
#define _PREFIX
#endif

#define FG_BLACK			0x0000
#define FG_BLUE				0x0100
#define FG_GREEN			0x0200
#define FG_CYAN				0x0300
#define FG_RED				0x0400
#define FG_PURPLE			0x0500
#define FG_BROWN			0x0600
#define FG_GREY				0x0700
#define FG_DARK_GREY		0x0800
#define FG_LIGHT_BLUE		0x0900
#define FG_LIGHT_GREEN		0x0a00
#define FG_LIGHT_CYAN		0x0b00
#define FG_LIGHT_RED		0x0c00
#define FG_LIGHT_PURPLE		0x0d00
#define FG_YELLOW			0x0e00
#define FG_WHITE			0x0f00

#define BG_BLACK			0x0000
#define BG_BLUE				0x1000
#define BG_GREEN			0x2000
#define BG_CYAN				0x3000
#define BG_RED				0x4000
#define BG_PURPLE			0x5000
#define BG_BROWN			0x6000
#define BG_GREY				0x7000

#define BLINK				0x8000 //1 bbb ffff

#define VIDEO_WIDTH 80
#define VIDEO_HEIGHT 25

/**
 * Sets the global text style.
 *
 * @param s The style identifier to apply to the text.
 */
_PREFIX void setStyle(uint16_t s)
#ifndef KERNEL
{call(21, s, 0, 0, 0);}
#endif
;

/**
 * Retrieves the current global text style.
 *
 * @return The current text style identifier.
 */
_PREFIX uint16_t getStyle()
#ifndef KERNEL
{return call(22, 0, 0, 0, 0);}
#endif
;

/**
 * Checks if the cursor is currently enabled.
 *
 * @return 1 if the cursor is enabled, 0 if the cursor is disabled.
 */
_PREFIX int8_t isCursor()
#ifndef KERNEL
{return call(23, 0, 0, 0, 0);}
#endif
;

/**
 * Disables the cursor.
 *
 * This function turns off the cursor, making it invisible.
 */
_PREFIX void disableCursor()
#ifndef KERNEL
{call(24, 0, 0, 0, 0);}
#endif
;

/**
 * Enables the cursor with a specified start and end scan line.
 *
 * @param start The starting scan row for the cursor.
 * @param end The ending scan row for the cursor.
 */
_PREFIX void enableCursor(uint8_t start, uint8_t end)
#ifndef KERNEL
{call(25, start, end, 0, 0);}
#endif
;

/**
 * Moves the cursor to the specified position on the screen.
 *
 * @param pos The position to move the cursor to, represented as a linear offset from the top-left corner of the screen.
 */
_PREFIX void moveCursor(int pos)
#ifndef KERNEL
{call(26, pos, 0, 0, 0);}
#endif
;

/**
 * Retrieves the current position of the cursor on the screen.
 *
 * @return The current cursor position, represented as a linear offset from the top-left corner of the screen.
 */
_PREFIX int getCursorPos()
#ifndef KERNEL
{return call(27, 0, 0, 0, 0);}
#endif
;

/**
 * Clears the screen, removing all text and graphical content.
 */
_PREFIX void clear()
#ifndef KERNEL
{call(28, 0, 0, 0, 0);}
#endif
;

/**
 * Prints a string to the screen.
 *
 * @param str A pointer to the null-terminated string to be printed.
 */
_PREFIX void print(const char *str)
#ifndef KERNEL
{call(29, (uint32_t)str, 0, 0, 0);}
#endif
;

/**
 * Prints a specified number of characters from a string to the screen.
 *
 * @param str A pointer to the string to be printed.
 * @param size The number of characters to print from the string.
 */
_PREFIX void printn(const char *str, size_t size)
#ifndef KERNEL
{call(30, (uint32_t)str, size, 0, 0);}
#endif
;

/**
 * Prints a specified number of characters with their style information to the screen.
 *
 * @param str A pointer to an array where each element combines the character and its associated
 *            style information.
 * @param size The number of elements to print from the array.
 */
_PREFIX void printRaw(const uint16_t *str, size_t size)
#ifndef KERNEL
{call(31, (uint32_t)str, size, 0, 0);}
#endif
;

/**
 * Sets a character at a specified position with a given style.
 *
 * @param pos The position on the screen, represented as a linear offset from the top-left corner of the screen.
 * @param c The character to display at the specified position.
 * @param s The style to apply to the character.
 */
_PREFIX void setChar(int pos, char c, uint16_t s)
#ifndef KERNEL
{call(32, pos, c, s, 0);}
#endif
;

#endif // SCREEN_H_INCLUDED
