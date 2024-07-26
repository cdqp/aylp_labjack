/** Interface for the LJTick-DAC.
 * see: Examples/ljTickDacSimple.py at https://github.com/labjack/LabJackPython
 */
#ifndef LJTDAC_H_
#define LJTDAC_H_

#include "labjack_ud.h"

// i2c addresses
#define LJTDAC_EEPROM_I2C	0xA0
#define LJTDAC_DAC_I2C		0x24

// address in i2c memory where cal mem starts
#define LJTDAC_CAL_MEM_START	0x40

// https://support.labjack.com/docs/ljtick-dac-datasheet
struct ljtdac_cal_mem {
	fp64 daca_slope;
	fp64 daca_offset;
	fp64 dacb_slope;
	fp64 dacb_offset;
	uint32_t serial_number;
}__attribute__((packed));


/** Read calibration memory into a struct lju3_cal_mem. */
int ljtdac_read_cal_mem(
	HANDLE dev,
	uint8_t sda_pin, uint8_t scl_pin,
	struct ljtdac_cal_mem *cal_mem
);


#endif

