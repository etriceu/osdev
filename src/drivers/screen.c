#include "../include/screen.h"
#include "../include/functions.h"

#define VIDEO_BEG (uint16_t*)0xB8000
#define VIDEO_SIZE VIDEO_WIDTH*VIDEO_HEIGHT
#define VIDEO_END VIDEO_BEG+VIDEO_SIZE

static uint16_t textStyle = FG_GREY | BG_BLACK;
static uint16_t *current = VIDEO_BEG;
static int8_t cursor = 1;

void setStyle(uint16_t s)
{
	textStyle = s & 0xFF00;
}

uint16_t getStyle()
{
	return textStyle;
}

int8_t isCursor()
{
	return cursor;
}

void disableCursor()
{
	out(0x3D4, 0x0A);
	out(0x3D5, 0x20);
	cursor = 0;
}

void enableCursor(uint8_t start, uint8_t end)
{
	out(0x3D4, 0x0A);
	out(0x3D5, (in(0x3D5) & 0xC0) | start);
	out(0x3D4, 0x0B);
	out(0x3D5, (in(0x3D5) & 0xE0) | end);
	cursor = 1;
}

void moveCursor(int pos)
{
	out(0x3D4, 0x0F);
	out(0x3D5, (uint8_t) (pos & 0xFF));
	out(0x3D4, 0x0E);
	out(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
	current = VIDEO_BEG+pos;
}

int getCursorPos()
{
	return current-VIDEO_BEG;
}

void clear()
{
	for(current = VIDEO_BEG; current < VIDEO_END; current++)
		*current = textStyle;

	moveCursor(0);
}

void _print(uint16_t sc)
{
	if(current >= VIDEO_END)
	{
		for(current = VIDEO_BEG; current < VIDEO_END-VIDEO_WIDTH; current++)
			*current = *(current+VIDEO_WIDTH);
		
		for(; current < VIDEO_END; current++)
			*current = textStyle;
		
		current -= VIDEO_WIDTH;
	}
	if((sc & 0x00FF) == '\n')
		current = VIDEO_BEG+((current-VIDEO_BEG)/VIDEO_WIDTH+1)*VIDEO_WIDTH-1;
	else
		*current = sc;
}

void print(const char *str)
{
	for(; *str != '\0'; str++, current++)
		_print(textStyle | *str);
	
	moveCursor(current-VIDEO_BEG);
}

void printn(const char *str, size_t size)
{
	for(const char *end = str+size; str < end; str++, current++)
		_print(textStyle | *str);
	
	moveCursor(current-VIDEO_BEG);
}

void printRaw(const uint16_t *str, size_t size)
{
	for(const uint16_t *end = str+size; str < end; str++, current++)
		_print(*str);
	
	moveCursor(current-VIDEO_BEG);
}

void setChar(int pos, char c, uint8_t s)
{
	if(pos < VIDEO_WIDTH*VIDEO_HEIGHT)
		*((uint16_t*)VIDEO_BEG+pos) = s | c;
}
