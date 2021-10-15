#include "include/keyboard.h"

uint8_t keyStatus[KEY_SIZE];
		
uint8_t isKeyEvent()
{
	return in(0x64) & 0x1;
}

uint8_t getKey()
{
	uint8_t key = in(0x60);
	if(key >= 0xE0)
		return KEY_SPECIAL;
	else if(key > KEY_SIZE)
	{
		key -= KEY_SIZE;
		keyStatus[key] = KEY_RELEASED;
	}
	else
		keyStatus[key] = KEY_PRESSED;
	
	return key;
}

int8_t getKeyStatus(uint8_t key)
{
	return keyStatus[key];
}
