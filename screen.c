#include "include/screen.h"
#include "include/functions.h"

uint8_t textStyle = FG_GREY | BG_BLACK;
unsigned int textPosition = 0;
uint16_t* videoMemory = (uint16_t*)0xb8000;
unsigned int textWidth = 80;
unsigned int textHeight = 25;

int8_t cursor = 1;

void setStyle(uint16_t s)
{
	textStyle = s;
}

unsigned int getWidth()
{
	return textWidth;
}

unsigned int getHeight()
{
	return textHeight;
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
}

void clear()
{
	for(int n = 0; n < textWidth*textHeight; n++)
		videoMemory[n] = (textStyle | 0x0000)<<8;
	
	textPosition = 0;
	moveCursor(0);
}

void _print()
{
	int size = textWidth*textHeight;
	if(textPosition >= size)
	{
		int n = 0;
		for(; n < size-textWidth; n++)
			videoMemory[n] = videoMemory[n+textWidth];
		
		for(; n < size; n++)
			videoMemory[n] = (textStyle | 0x0000)<<8;
		
		textPosition -= textWidth;
	}
	
	if(cursor)
		moveCursor(textPosition);
}

void print(const char *str)
{
	for(; *str != '\0'; str++, textPosition++)
	{
		_print();
		if(*str == '\n')
			textPosition = (textPosition/textWidth+1)*textWidth-1;
		else
			videoMemory[textPosition] = (textStyle | 0x0000)<<8 | *str;
	}
	_print();
}

void printn(const char *str, size_t size)
{
	for(int end = textPosition+size; textPosition < end; str++, textPosition++)
	{
		_print();
		if(*str == '\n')
			textPosition = (textPosition/textWidth+1)*textWidth-1;
		else
			videoMemory[textPosition] = (textStyle | 0x0000)<<8 | *str;
	}
	_print();
}

void printraw(const uint16_t *str, size_t size)
{
	for(int end = textPosition+size; textPosition < end; str++, textPosition++)
	{
		_print();
		if((*str & 0x00ff) == '\n')
			textPosition = (textPosition/textWidth+1)*textWidth-1;
		else
			videoMemory[textPosition] = *str;
	}
	_print();
}

void setchar(int pos, char c, uint8_t s)
{
	if(pos < textWidth*textHeight)
		videoMemory[pos] = (s | 0x0000)<<8 | c;
}
