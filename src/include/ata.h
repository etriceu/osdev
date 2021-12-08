#ifndef ATA_H
#define ATA_H

#include "functions.h"

#define PRIMARY_MASTER 0b000
#define PRIMARY_SLAVE 0b001
#define SECONDARY_MASTER 0b010
#define SECONDARY_SLAVE 0b100

void ataRead(uint8_t device, uint32_t lba, uint8_t n, uint8_t *dest);
void ataWrite(uint8_t device, uint32_t lba, uint8_t n, uint8_t *src);
uint32_t ataGetSize(uint8_t device);

#endif // ATA_H_INCLUDED
