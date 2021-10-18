#include "include/ata.h"

void ataRead(uint32_t lba, uint8_t n, uint8_t *dest)
{
	while(in(0x1F7) & 0x80);
	
	out(0x1f1, 0);
	out(0x1f2, n);
	out(0x1f3, (uint8_t)lba);
	out(0x1f4, (uint8_t)(lba>>8));
	out(0x1f5, (uint8_t)(lba>>16));
	out(0x1f6, ((lba>>24) & 0x0f) | 0xe0);
	out(0x1f7, 0x20);
	
	for(int a = 0; a < n; a++)
	{
		while(in(0x1f7) & 0x80);
		
		for(uint8_t *end = dest+512; dest != end; dest+=2)
			*(uint16_t*)(dest) = in2(0x1f0);
	}
}

void ataWrite(uint32_t lba, uint8_t n, uint8_t *src)
{
	while(in(0x1f7) & 0x80);
		
	out(0x1f1, 0x00);
	out(0x1f2, 0x01);
	out(0x1f3, (uint8_t)lba);
	out(0x1f4, (uint8_t)(lba >> 8));
	out(0x1f5, (uint8_t)(lba >> 16));
	out(0x1f6, ((lba>>24) & 0x0f) | 0xe0);
	out(0x1f7, 0x30);

	for(int a = 0; a < n; a++)
	{
		while(in(0x1f7) & 0x80);
		
		for(uint8_t *end = src+512; src != end; src+=2)
			out2(0x1f0, *(uint16_t*)(src));
	}
}
