/** Interface for the LJTick-DAC.
 * see: Examples/ljTickDacSimple.py at https://github.com/labjack/LabJackPython
 */
#ifndef LJTDAC_H_
#define LJTDAC_H_

#include <stdbool.h>
#include "labjack_ud.h"

// https://support.labjack.com/docs/ljtick-dac-datasheet
struct ljtdac_cal_mem {
	fp64 daca_slope;
	fp64 daca_offset;
	fp64 dacb_slope;
	fp64 dacb_offset;
	uint32_t serial_number;
}__attribute__((packed));

// command bytes for each output
typedef uint8_t ljtdac_output;
enum {
	LJTDAC_WRITE_DACA	= 0x30,
	LJTDAC_WRITE_DACB	= 0x31,
};

/** Read calibration memory into a struct lju3_cal_mem. */
int ljtdac_read_cal_mem(HANDLE dev,
	struct ljtdac_cal_mem *cal_mem, uint8_t sda_pin, uint8_t scl_pin
);

/** Set (calibration-adjusted) voltage of DACA or DACB. */
int ljtdac_write_dac(
	HANDLE dev, struct ljtdac_cal_mem *cal_mem,
	uint8_t sda_pin, uint8_t scl_pin,
	bool fast, ljtdac_output output, double voltage
);


#endif

