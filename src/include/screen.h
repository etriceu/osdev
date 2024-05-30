#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>
#include <stddef.h>

#ifndef KERNEL
#include "userCall.h"
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

void setStyle(uint16_t s)
#ifndef KERNEL
{call(21, s, 0, 0, 0);}
#endif
;
uint16_t getStyle()
#ifndef KERNEL
{return call(22, 0, 0, 0, 0);}
#endif
;
int8_t isCursor()
#ifndef KERNEL
{return call(23, 0, 0, 0, 0);}
#endif
;
void disableCursor()
#ifndef KERNEL
{call(24, 0, 0, 0, 0);}
#endif
;
void enableCursor(uint8_t start, uint8_t end)
#ifndef KERNEL
{call(25, start, end, 0, 0);}
#endif
;
void moveCursor(int pos)
#ifndef KERNEL
{call(26, pos, 0, 0, 0);}
#endif
;
int getCursorPos()
#ifndef KERNEL
{return call(27, 0, 0, 0, 0);}
#endif
;

void clear()
#ifndef KERNEL
{call(28, 0, 0, 0, 0);}
#endif
;
void print(const char *str)
#ifndef KERNEL
{call(29, (uint32_t)str, 0, 0, 0);}
#endif
;
void printn(const char *str, size_t size)
#ifndef KERNEL
{call(30, (uint32_t)str, size, 0, 0);}
#endif
;
void printRaw(const uint16_t *str, size_t size)
#ifndef KERNEL
{call(31, (uint32_t)str, size, 0, 0);}
#endif
;
void setChar(int pos, char c, uint16_t s)
#ifndef KERNEL
{call(32, pos, c, s, 0);}
#endif
;

#endif // SCREEN_H_INCLUDED
