#ifndef ATA_H
#define ATA_H

#include "functions.h"

void ataRead(uint32_t lba, uint8_t n, uint8_t *dest);
void ataWrite(uint32_t lba, uint8_t n, uint8_t *src);

#endif // ATA_H_INCLUDED
