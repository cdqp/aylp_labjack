#ifndef AYLP_LJTDAC_H_
#define AYLP_LJTDAC_H_

#include "anyloop.h"


struct aylp_ljtdac_data {
	HANDLE dev;
	struct lju3_cal_mem cal_mem;
};

// initialize device
int aylp_ljtdac_init(struct aylp_device *self);

// process device once per loop
int aylp_ljtdac_u3_proc(struct aylp_device *self, struct aylp_state *state);

// close device when loop exits
int aylp_ljtdac_u3_fini(struct aylp_device *self);

#endif

