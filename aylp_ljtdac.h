#ifndef AYLP_LJTDAC_H_
#define AYLP_LJTDAC_H_

#include <libaylp/anyloop.h>


struct aylp_ljtdac_data {
	HANDLE dev;
	struct ljtdac_cal_mem cal_mem;
	unsigned long square_hz;

	uint8_t clock_config;
	uint8_t clock_divisor;
	uint8_t square_pin;	// pin to write square wave on
	uint8_t sda_pin;	// sda for ljtick i2c
	uint8_t scl_pin;	// scl for ljtick i2c
};

// initialize device
int aylp_ljtdac_init(struct aylp_device *self);

// process device once per loop
int aylp_ljtdac_u3_proc(struct aylp_device *self, struct aylp_state *state);

// close device when loop exits
int aylp_ljtdac_u3_fini(struct aylp_device *self);

#endif

