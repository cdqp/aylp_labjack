#include <errno.h>
#include <stddef.h>
#include <string.h>

#include "anyloop.h"
#include "logging.h"
#include "xalloc.h"
#include "labjack_ud.h"
#include "labjack_u3.h"
#include "aylp_labjack_dac.h"


int aylp_labjack_dac_init(struct aylp_device *self)
{
	int err;
	self->process = &aylp_lju3_dac_process;
	self->close = &aylp_lju3_dac_close;
	self->device_data = xcalloc(1, sizeof(struct aylp_labjack_dac_data));
	struct aylp_labjack_dac_data *data = self->device_data;

	unsigned product_id = 0;

	if (!self->params) {
		log_error("No params object found.");
		return -1;
	}
	json_object_object_foreach(self->params, key, val) {
		// parse parameters
		if (key[0] == '_') {
			// keys starting with _ are comments
		} else if (!strcmp(key, "model")) {
			const char *model = json_object_get_string(val);
			if (!strcmp(model, "U3")) {
				product_id = U3_PRODUCT_ID;
			} else {
				log_warn("Unknown model: %s", model);
			}
		} else {
			log_warn("Unknown parameter \"%s\"", key);
		}
	}

	log_debug("liblabjackusb version %G", LJUSB_GetLibraryVersion());

	size_t dev_count;
	switch (product_id) {
	case U3_PRODUCT_ID:
		dev_count = LJUSB_GetDevCount(U3_PRODUCT_ID);
		if (!dev_count) {
			log_error("LJUSB_GetDevCount returned 0.");
			return -1;
		} else if (dev_count > 1) {
			log_info("I see %u U3s. Using the first.", dev_count);
		}
		data->dev = LJUSB_OpenDevice(1, 0, product_id);
		if (!data->dev) {
			log_error("Failed to open U3: %s", strerror(errno));
			return -1;
		}

		break;
	default:
		log_error("Didn't get a valid \"model\" param.");
		return -1;
	}

	// set types and units
	self->type_in = AYLP_T_ANY;
	self->units_in = AYLP_U_ANY;
	self->type_out = 0;
	self->units_out = 0;
	return 0;
}


int aylp_lju3_dac_process(struct aylp_device *self, struct aylp_state *state)
{
	struct aylp_labjack_dac_data *data = self->device_data;
	UNUSED(state);
	return 0;
}


int aylp_lju3_dac_close(struct aylp_device *self)
{
	struct aylp_labjack_dac_data *data = self->device_data;
	LJUSB_CloseDevice(data->dev);
	xfree(data);
	return 0;
}

