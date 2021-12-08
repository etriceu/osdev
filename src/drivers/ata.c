#include "../include/ata.h"
#include "../include/malloc.h"

uint32_t diskSize = 0;
const uint16_t buses[] = {0x1f0, 0x1f0, 0x170, 0x170};
const uint8_t disks[] = {0xe0, 0xf0, 0xe0, 0xf0};

void ataRead(uint8_t device, uint32_t lba, uint8_t n, uint8_t *dest)
{
	uint16_t bus = buses[device];
	while(in(bus+7) & 0x80);
	
	out(bus+1, 0);
	out(bus+2, n);
	out(bus+3, (uint8_t)lba);
	out(bus+4, (uint8_t)(lba>>8));
	out(bus+5, (uint8_t)(lba>>16));
	out(bus+6, ((lba>>24) & 0x0f) | disks[device]);
	out(bus+7, 0x20);
	
	for(int a = 0; a < n; a++)
	{
		while(in(bus+7) & 0x80);
		
		for(uint8_t *end = dest+512; dest != end; dest+=2)
			*(uint16_t*)(dest) = in2(bus);
	}
}

void ataWrite(uint8_t device, uint32_t lba, uint8_t n, uint8_t *src)
{
	uint16_t bus = buses[device];
	while(in(bus+7) & 0x80);
		
	out(bus+1, 0x00);
	out(bus+2, n);
	out(bus+3, (uint8_t)lba);
	out(bus+4, (uint8_t)(lba >> 8));
	out(bus+5, (uint8_t)(lba >> 16));
	out(bus+6, ((lba>>24) & 0x0f) | disks[device]);
	out(bus+7, 0x30);

	for(int a = 0; a < n; a++)
	{
		while(in(bus+7) & 0x80);
		
		for(uint8_t *end = src+512; src != end; src+=2)
			out2(bus, *(uint16_t*)(src));
	}
}

uint32_t ataGetSize(uint8_t device)
{
	uint16_t bus = buses[device];
	out(bus+2, 0);
	out(bus+3, 0);
	out(bus+4, 0);
	out(bus+5, 0);
	out(bus+6, disks[device]-0x40);
	out(bus+7, 0xec);
	
	uint32_t ret = 1;
	
	uint16_t *buf = malloc(512);
	if(in(bus+7))
	{
		while(in(bus+7) & 0x80);
		
		for(int n = 0; n < 256; n++)
			buf[n] = in2(bus);
		
		ret = *(uint32_t*)&buf[60];
	}
	free(buf);
	
	return ret-1;
}
