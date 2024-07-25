#include "labjack_ud.h"


/** All checksums are a “1’s complement checksum”. Both the 8-bit and 16-bit
 * checksum are unsigned. Sum all applicable bytes in an accumulator, 1 at a
 * time. Each time another byte is added, check for overflow (carry bit), and if
 * true add one to the accumulator.
 */
uint8_t ljud_checksum8(uint8_t *data, uint8_t len)
{
	uint8_t acc = 0;
	for (uint8_t i = 0; i < len; i++) {
		if (acc + data[i] > 0xFF) acc += 1;
		acc += data[i];
	}
	return acc;
}
uint16_t ljud_checksum16(uint8_t *data, uint8_t len)
{
	uint16_t acc = 0;
	for (uint8_t i = 0; i < len; i++) {
		// can't overflow since docs imply n <= 255
		acc += data[i];
	}
	return acc;
}

