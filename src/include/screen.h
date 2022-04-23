#ifndef SCREEN_H
#define SCREEN_H

#include "functions.h"

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

void setStyle(uint16_t s);
uint16_t getStyle();

int8_t isCursor();
void disableCursor();
void enableCursor(uint8_t start, uint8_t end);
void moveCursor(int pos);
int getCursorPos();

void clear();
void print(const char *str);
void printn(const char *str, size_t size);
void printRaw(const uint16_t *str, size_t size);
void setChar(int pos, char c, uint8_t s);

#endif // SCREEN_H_INCLUDED
