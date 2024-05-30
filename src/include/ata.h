#ifndef ATA_H
#define ATA_H

#include <stdint.h>

#ifndef KERNEL
#include "userCall.h"
#endif

#define PRIMARY_MASTER 0b000
#define PRIMARY_SLAVE 0b001
#define SECONDARY_MASTER 0b010
#define SECONDARY_SLAVE 0b100

void ataRead(uint8_t device, uint32_t lba, uint8_t n, uint8_t *dest)
#ifndef KERNEL
{call(0, device, lba, n, (uint32_t)dest);}
#endif
;
void ataWrite(uint8_t device, uint32_t lba, uint8_t n, uint8_t *src)
#ifndef KERNEL
{call(1, device, lba, n, (uint32_t)src);}
#endif
;
uint32_t ataGetSize(uint8_t device)
#ifndef KERNEL
{return call(2, device, 0, 0, 0);}
#endif
;

#endif // ATA_H_INCLUDED
