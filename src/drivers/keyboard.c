#include "../include/keyboard.h"

uint8_t keyStatus[KEY_SIZE];
uint8_t cache[KEY_CACHE];
uint8_t kbdLast = 0;
uint8_t kbdFirst = 0;
uint8_t key2id[256];
		
uint8_t isKeyEvent()
{
	return in(0x64) & 0x1;
}

uint8_t getKey()
{
	uint8_t key = in(0x60);
	
	if(key >= KEY_SPECIAL_ID)
	{
		key = in(0x60);
		uint8_t skey = 0, status = KEY_PRESSED;
		if(key > KEY_PRE_REL)
		{
			key -= KEY_PRE_REL;
			status = KEY_RELEASED;
		}
		
		for(int n = 0; n < sizeof(specialKeys)/sizeof(specialKeys[0]); n++)
			if(key == specialKeys[n].id)
			{
				skey = specialKeys[n].key;
				keyStatus[skey] = status;
				break;
			}
		//skey = 0;
		return skey ? skey : KEY_SPECIAL;
	}
	else if(key > KEY_PRE_REL)
	{
		key -= KEY_PRE_REL;
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

void keyboard()
{
	if(isKeyEvent())
	{
		uint8_t key = getKey();
		if(getKeyStatus(key) == KEY_PRESSED)
		{
			cache[kbdLast] = key;
			kbdLast = ++kbdLast%KEY_CACHE;
			
			if(kbdLast == kbdFirst)
				kbdFirst = ++kbdFirst%KEY_CACHE;
		}
	}
}

uint8_t pollKeys()
{
	if(kbdFirst == kbdLast)
		return KEY_NONE;
	
	uint8_t key = cache[kbdFirst];
	kbdFirst = ++kbdFirst%KEY_CACHE;
	return key;
}

void translateKeyCodes()
{
	for(int n = 0; n < KEY_SIZE; n++)
		key2id[keys[n]] = n;
}

uint8_t keyID(uint8_t key)
{
	return key2id[key];
}
