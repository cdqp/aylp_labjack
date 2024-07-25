#ifndef AYLP_LABJACK_DAC_H_
#define AYLP_LABJACK_DAC_H_

#include "anyloop.h"


struct aylp_labjack_dac_data {
	HANDLE dev;
	struct lju3_cal_mem cal_mem;
};

// initialize device
int aylp_labjack_dac_init(struct aylp_device *self);

// process device once per loop
int aylp_lju3_dac_process(struct aylp_device *self, struct aylp_state *state);

// close device when loop exits
int aylp_lju3_dac_close(struct aylp_device *self);

#endif

