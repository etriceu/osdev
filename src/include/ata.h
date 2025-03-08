#ifndef ATA_H
#define ATA_H

#include <stdint.h>

#ifndef KERNEL
#include "userCall.h"
#define _PREFIX static inline
#else
#define _PREFIX
#endif

enum
{
	PRIMARY_MASTER,
	PRIMARY_SLAVE,
	SECONDARY_MASTER,
	SECONDARY_SLAVE
};

/**
 * Reads N sectors from the specified address on the hard disk to the destination.
 * 
 * @param device The hard disk identifier.
 * @param lba The address of first sector.
 * @param N The number of sectors to read.
 * @param dest Pointer to the array where the read sectors are stored.
 */
_PREFIX void ataRead(uint8_t device, uint32_t lba, uint8_t n, uint8_t *dest)
#ifndef KERNEL
{call(0, device, lba, n, (uint32_t)dest);}
#endif
;

/**
 * Writes N sectors from the source to the specified address on the hard disk.
 * 
 * @param device The hard disk identifier.
 * @param lba The address of first sector.
 * @param N The number of sectors to write.
 * @param src Pointer to the array containing the sectors to be written.
 */
_PREFIX void ataWrite(uint8_t device, uint32_t lba, uint8_t n, uint8_t *src)
#ifndef KERNEL
{call(1, device, lba, n, (uint32_t)src);}
#endif
;

/**
 * Returns the capacity of the specified hard disk.
 * 
 * @param device The hard disk identifier.
 * @return The number of sectors.
 */
_PREFIX uint32_t ataGetSize(uint8_t device)
#ifndef KERNEL
{return call(2, device, 0, 0, 0);}
#endif
;

/**
 * Fills SIZE sectors with VALUE to the specified address on the hard disk.
 * 
 * @param device The hard disk identifier.
 * @param lba The address of first sector.
 * @param size The number of sectors to write.
 * @param value Filling value.
 */
_PREFIX void ataFill(uint8_t device, uint32_t lba, uint8_t size, uint16_t value)
#ifndef KERNEL
{call(39, device, lba, size, value);}
#endif
;

#endif // ATA_H_INCLUDED
