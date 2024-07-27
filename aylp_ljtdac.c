#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <libaylp/anyloop.h>
#include <libaylp/logging.h>
#include <libaylp/xalloc.h>

#include "labjack_u3.h"
#include "ljtdac.h"
#include "aylp_ljtdac.h"


// U3-specific initialization
static int init_u3(struct aylp_ljtdac_data *data)
{
	int err;
	// decide on pins
	// TODO: parametrize
	data->square_pin = LJU3_FIO6;
	data->sda_pin = LJU3_FIO5;
	data->scl_pin = LJU3_FIO4;

	// get a handle
	size_t dev_count = LJUSB_GetDevCount(U3_PRODUCT_ID);
	if (!dev_count) {
		log_error("LJUSB_GetDevCount returned 0.");
		return -1;
	} else if (dev_count > 1) {
		log_info("I see %u U3s. Using the first.", dev_count);
	}
	data->dev = LJUSB_OpenDevice(1, 0, U3_PRODUCT_ID);
	if (!data->dev) {
		log_error("Failed to open U3: %s", strerror(errno));
		return -1;
	}

	// check that we can read startup config
	struct lju3_config_resp config_resp;
	err = lju3_read_config(data->dev, &config_resp);
	if (err) {
		log_error("lju3_config_resp returned %d: %s",
			err, strerror(-err)
		);
		log_error("errno was %d: %s",
			errno, strerror(errno)
		);
		return -1;
	}
	log_debug("U3 startup configuration:");
	log_debug("	firmware_version: %hhu.%hhu",
		config_resp.firmware_version >> 8,
		config_resp.firmware_version
	);
	log_debug("	bootloader_version: %hhu.%hhu",
		config_resp.bootloader_version >> 8,
		config_resp.bootloader_version
	);
	log_debug("	hardware_version: %hhu.%hhu",
		config_resp.hardware_version >> 8,
		config_resp.hardware_version
	);
	log_debug("	serial_number: %u", config_resp.serial_number);
	log_debug("	product_id: %u", config_resp.product_id);
	log_debug("	local_id: %u", config_resp.local_id);
	log_debug("	timer_counter_mask: 0x%hhX",
		config_resp.timer_counter_mask
	);
	log_debug("	fio_analog: %u", config_resp.fio_analog);
	log_debug("	fio_direction: %u", config_resp.fio_direction);
	log_debug("	fio_state: %u", config_resp.fio_state);
	log_debug("	eio_analog: %u", config_resp.eio_analog);
	log_debug("	eio_direction: %u", config_resp.eio_direction);
	log_debug("	eio_state: %u", config_resp.eio_state);
	log_debug("	cio_direction: %u", config_resp.cio_direction);
	log_debug("	cio_state: %u", config_resp.cio_state);
	log_debug("	dac1_enable: %u", config_resp.dac1_enable);
	log_debug("	dac0: %u", config_resp.dac0);
	log_debug("	dac1: %u", config_resp.dac1);
	log_debug("	clock_config: %u", config_resp.clock_config);
	log_debug("	clock_divisor: %u", config_resp.clock_divisor);
	log_debug("	compatibility: %u", config_resp.compatibility);
	log_debug("	version_info: 0x%hhX",
		config_resp.version_info
	);

	// configure IO ports
	struct lju3_configio configio = {0};
	struct lju3_configio_resp configio_resp;
	configio.write_mask |= 1 << 0;	// set counter_config
	configio.write_mask |= 1 << 1;	// set dac1_enable
	configio.write_mask |= 1 << 2;	// set fio_analog
	configio.counter_config = 0x40;	// disable counters, offset = 4
	configio.dac1_enable = 0;	// disable dac1
	configio.fio_analog = 0;	// set to digital
	err = lju3_configio(data->dev, &configio, &configio_resp);
	if (err) {
		log_error("lju3_configio returned %d: %s",
			err, strerror(-err)
		);
		log_error("errno was %d: %s",
			errno, strerror(errno)
		);
		return -1;
	}
	log_debug("U3 ConfigIO:");
	log_debug("	counter_config: %hhX",
		configio_resp.counter_config
	);
	log_debug("	dac1_enable: %u", configio_resp.dac1_enable);
	log_debug("	fio_analog: %u", configio_resp.fio_analog);
	log_debug("	eio_analog: %u", configio_resp.eio_analog);

	// output square wave if wanted
	if (data->square_hz) {
		log_info("You requested square_hz = %lu", data->square_hz);
		double hz_real;
		err = lju3_timer(
			data->dev, data->square_pin, LJU3_TIMER_OUT_SQUARE,
			data->square_hz, &hz_real
		);
		if (err) {
			log_error("lju3_timer returned %d: %s",
				err, strerror(-err)
			);
			log_error("errno was %d: %s", errno, strerror(errno));
			return err;
		}
		log_info("Best I could do: %G Hz", hz_real);
	}

	return 0;
}


int aylp_ljtdac_init(struct aylp_device *self)
{
	int err;
	self->device_data = xcalloc(1, sizeof(struct aylp_ljtdac_data));
	struct aylp_ljtdac_data *data = self->device_data;

	unsigned product_id = 0;

	if (!self->params) {
		log_error("No params object found.");
		return -1;
	}
	json_object_object_foreach(self->params, key, val) {
		// parse parameters
		if (key[0] == '_') {
			// keys starting with _ are comments
		} else if (!strcmp(key, "host")) {
			const char *host = json_object_get_string(val);
			if (!strcasecmp(host, "U3")) {
				log_trace("host = U3");
				product_id = U3_PRODUCT_ID;
			} else {
				log_warn("Unknown host: %s", host);
			}
		} else if (!strcmp(key, "square_hz")) {
			data->square_hz = json_object_get_uint64(val);
			log_trace("square_hz = %lu", data->square_hz);
		} else {
			log_warn("Unknown parameter \"%s\"", key);
		}
	}

	log_debug("liblabjackusb version %G", LJUSB_GetLibraryVersion());

	switch (product_id) {
	case U3_PRODUCT_ID:
		err = init_u3(data);
		if (err) return err;
		self->proc = &aylp_ljtdac_u3_proc;
		self->fini = &aylp_ljtdac_u3_fini;
		break;
	default:
		log_error("Didn't get a valid \"host\" param.");
		return -1;
	}

	// read ljtick-dac calibration memory
	err = ljtdac_read_cal_mem(
		data->dev, data->sda_pin, data->scl_pin, &data->cal_mem
	);
	if (err) {
		log_error("ljtdac_read_cal_mem returned %d: %s",
			err, strerror(-err)
		);
		log_error("errno was %d: %s",
			errno, strerror(errno)
		);
		return -1;
	}
	log_debug("LJTick calibration:");
	log_debug("	daca_slope: %G", fp642dbl(data->cal_mem.daca_slope));
	log_debug("	daca_offset: %G", fp642dbl(data->cal_mem.daca_offset));
	log_debug("	dacb_slope: %G", fp642dbl(data->cal_mem.dacb_slope));
	log_debug("	dacb_offset: %G", fp642dbl(data->cal_mem.dacb_offset));
	log_debug("	serial_number: %lu", data->cal_mem.serial_number);

	// set types and units
	self->type_in = AYLP_T_VECTOR;
	self->units_in = AYLP_U_V;
	self->type_out = 0;
	self->units_out = 0;
	return 0;
}


int aylp_ljtdac_u3_proc(struct aylp_device *self, struct aylp_state *state)
{
	struct aylp_ljtdac_data *data = self->device_data;
	if (state->vector->size > 0) {
		log_trace("Wrote %G V to DACA.");
	}
	if (state->vector->size > 1) {
		log_trace("Wrote %G V to DACB.");
	}
	UNUSED(data);
	UNUSED(state);
	return 0;
}


int aylp_ljtdac_u3_fini(struct aylp_device *self)
{
	struct aylp_ljtdac_data *data = self->device_data;
	LJUSB_CloseDevice(data->dev);
	xfree(data);
	return 0;
}


