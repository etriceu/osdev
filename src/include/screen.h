#ifndef SCREEN_H
#define SCREEN_H

#include "functions.h"

#define FG_BLACK			0x00
#define FG_BLUE				0x01
#define FG_GREEN			0x02
#define FG_CYAN				0x03
#define FG_RED				0x04
#define FG_PURPLE			0x05
#define FG_BROWN			0x06
#define FG_GREY				0x07
#define FG_DARK_GREY		0x08
#define FG_LIGHT_BLUE		0x09
#define FG_LIGHT_GREEN		0x0a
#define FG_LIGHT_CYAN		0x0b
#define FG_LIGHT_RED		0x0c
#define FG_LIGHT_PURPLE		0x0d
#define FG_YELLOW			0x0e
#define FG_WHITE			0x0f

#define BG_BLACK			0x00
#define BG_BLUE				0x10
#define BG_GREEN			0x20
#define BG_CYAN				0x30
#define BG_RED				0x40
#define BG_PURPLE			0x50
#define BG_BROWN			0x60
#define BG_GREY				0x70

#define BLINK				0x80 //1 bbb ffff

void setStyle(uint16_t s);
unsigned int getWidth();
unsigned int getHeight();

int8_t isCursor();
void disableCursor();
void enableCursor(uint8_t start, uint8_t end);
void moveCursor(int pos);

void clear();
void print(const char *str);
void printn(const char *str, size_t size);
void printRaw(const uint16_t *str, size_t size);
void setChar(int pos, char c, uint8_t s);

#endif // SCREEN_H_INCLUDED
