#include <errno.h>
#include <math.h>
#include <string.h>

#include "labjack_u3.h"


int lju3_config_timer_clock(HANDLE dev,
	struct lju3_config_timer_clock *config,
	struct lju3_config_timer_clock_resp *config_resp
) {
	unsigned long n;
	const unsigned n_tx = sizeof(struct lju3_config_timer_clock);
	const unsigned n_rx = sizeof(struct lju3_config_timer_clock_resp);
	const unsigned n_head = sizeof(struct ljud_extended_header);

	config->header.command = 0xF8;
	config->header.n_data_words = ((n_tx - n_head) / 2);
	config->header.extended_command = 0x0A;

	config->header.checksum16 = ljud_checksum16(
		(uint8_t *)config + 6, n_tx - 6
	);
	config->header.checksum8 = ljud_checksum8(
		(uint8_t *)config + 1, n_head - 1
	);

	n = LJUSB_Write(dev, (uint8_t *)config, n_tx);
	if (n < n_tx) return -ECOMM;

	n = LJUSB_Read(dev, (uint8_t *)config_resp, n_rx);
	if (n < n_rx) {
		// LJ is telling us we have a bad checksum
		if (*(uint16_t *)config_resp == 0xB8B8) return -EBADMSG;
		return -EREMOTEIO;
	}

	if (
		config_resp->header.checksum16
		!= ljud_checksum16((uint8_t *)config_resp + 6, n_rx - 6)
	) {
		// LJ checksum failed
		return -EBADE;
	}

	if (config_resp->err) return config_resp->err;

	return 0;
}


int lju3_config_io(HANDLE dev,
	struct lju3_config_io *config, struct lju3_config_io_resp *config_resp
) {
	unsigned long n;
	const unsigned n_tx = sizeof(struct lju3_config_io);
	const unsigned n_rx = sizeof(struct lju3_config_io_resp);
	const unsigned n_head = sizeof(struct ljud_extended_header);

	config->header.command = 0xF8;
	config->header.n_data_words = ((n_tx - n_head) / 2);
	config->header.extended_command = 0x0B;

	config->header.checksum16 = ljud_checksum16(
		(uint8_t *)config + 6, n_tx - 6
	);
	config->header.checksum8 = ljud_checksum8(
		(uint8_t *)config + 1, n_head - 1
	);

	n = LJUSB_Write(dev, (uint8_t *)config, n_tx);
	if (n < n_tx) return -ECOMM;

	n = LJUSB_Read(dev, (uint8_t *)config_resp, n_rx);
	if (n < n_rx) {
		// LJ is telling us we have a bad checksum
		if (*(uint16_t *)config_resp == 0xB8B8) return -EBADMSG;
		return -EREMOTEIO;
	}

	if (
		config_resp->header.checksum16
		!= ljud_checksum16((uint8_t *)config_resp + 6, n_rx - 6)
	) {
		// LJ checksum failed
		return -EBADE;
	}

	return 0;
}


int lju3_feedback_timer_config(HANDLE dev,
	struct lju3_feedback_timer_config *config,
	struct lju3_feedback_resp_header *config_resp
) {
	unsigned long n;
	const unsigned n_tx = sizeof(struct lju3_feedback_timer_config);
	const unsigned n_rx = sizeof(struct lju3_feedback_resp_header);
	const unsigned n_head = sizeof(struct ljud_extended_header);

	// docs say this is "43,45"; idk i'm just gonna use the former
	config->io_type = 0x2B;
	config->header.echo = 0xAA;	// arbitrary
	config->header.header.command = 0xF8;
	config->header.header.n_data_words = ((n_tx - n_head) / 2);
	config->header.header.extended_command = 0x00;

	config->header.header.checksum16 = ljud_checksum16(
		(uint8_t *)config + 6, n_tx - 6
	);
	config->header.header.checksum8 = ljud_checksum8(
		(uint8_t *)config + 1, n_head - 1
	);

	n = LJUSB_Write(dev, (uint8_t *)config, n_tx);
	if (n < n_tx) return -ECOMM;

	n = LJUSB_Read(dev, (uint8_t *)config_resp, n_rx);
	if (n < n_rx) {
		// LJ is telling us we have a bad checksum
		if (*(uint16_t *)config_resp == 0xB8B8) return -EBADMSG;
		return -EREMOTEIO;
	}

	if (
		config_resp->header.checksum16
		!= ljud_checksum16((uint8_t *)config_resp + 6, n_rx - 6)
	) {
		// LJ checksum failed
		return -EBADE;
	}
	if (config_resp->echo != config->header.echo) return -EBADE;
	if (config_resp->err) return config_resp->err;

	return 0;
}


int lju3_read_cal_mem(HANDLE dev, struct lju3_cal_mem *cal_mem)
{
	unsigned long n;
	struct lju3_readmem tx;
	struct lju3_readmem_resp *rx;
	const unsigned n_tx = sizeof(struct lju3_config);
	const unsigned n_rx = sizeof(struct lju3_config_resp);
	const unsigned n_head = sizeof(struct ljud_extended_header);

	tx.header.command = 0xF8;
	tx.header.n_data_words = 1;
	tx.header.extended_command = 0x2D;

	for (unsigned i = 0; i <= 4; i++) {
		tx.block_num = i;
		tx.header.checksum16 = ljud_checksum16(
			(uint8_t *)&tx + 6, n_tx - 6
		);
		tx.header.checksum8 = ljud_checksum8(
			(uint8_t *)&tx + 1, n_head - 1
		);

		n = LJUSB_Write(dev, (void *)&tx, n_tx);
		if (n < n_tx) return -ECOMM;

		n = LJUSB_Read(dev, (void *)&rx, n_rx);
		if (n < n_rx) {
			// LJ is telling us we have a bad checksum
			if (*(uint16_t *)rx == 0xB8B8) return -EBADMSG;
			return -EREMOTEIO;
		}

		if (
			rx->header.checksum16
			!= ljud_checksum16((uint8_t *)rx + 6, n_rx - 6)
		) {
			return -EBADE;
		}

		memcpy(
			(uint8_t *)cal_mem + sizeof(cal_mem->block0),
			rx, sizeof(cal_mem->block0)
		);
	}

	return 0;
}


int lju3_read_config(HANDLE dev, struct lju3_config_resp *config_resp)
{
	unsigned long n;
	uint8_t tx[sizeof(struct lju3_config)] = {0};
	uint8_t rx[sizeof(struct lju3_config_resp)];
	struct lju3_config *t = (struct lju3_config *)tx;

	t->header.command = 0xF8;
	t->header.n_data_words = (
		(sizeof(tx) - sizeof(struct ljud_extended_header)) / 2
	);
	static_assert(
		sizeof(tx) - sizeof(struct ljud_extended_header) == 2 * 0x0A,
		"bad n_data_words"
	);
	t->header.extended_command = 0x08;
	t->header.checksum16 = ljud_checksum16(tx + 6, sizeof(tx) - 6);
	t->header.checksum8 = ljud_checksum8(tx + 1, sizeof(tx) - 1);

	n = LJUSB_Write(dev, tx, sizeof(tx));
	if (n < sizeof(tx)) return -ECOMM;

	n = LJUSB_Read(dev, rx, sizeof(rx));
	if (n < sizeof(rx)) return -EREMOTEIO;

	if (
		((struct lju3_config_resp *)rx)->header.checksum16
		!= ljud_checksum16(rx + 6, sizeof(rx) - 6)
	) {
		return -EBADMSG;
	}

	memcpy(config_resp, rx, sizeof(struct lju3_config_resp));
	return 0;
}


int lju3_square(HANDLE dev, uint8_t pin, unsigned long hz_req, double *hz_real)
{
	int err;

	// LJ docs: To configure timers, write to the following:
	//	NumberTimersEnabled TimerClockBase TimerClockDivisor
	//	TimerCounterPinOffset TimerValue TimerMode
	// this involves communicating the following structs:
	// config_timer_clock: base, divisor
	// config_io.timer_counter_config: number enabled, pin offset
	// feedback_timer_config: value, mode

	// let's do the timer clock config first
	struct lju3_config_timer_clock config_timer_clock = {0};
	struct lju3_config_timer_clock_resp config_timer_clock_resp;
	config_timer_clock.clock_config = LJU3_WRITE_CLOCK_CONFIG;

	// LJ docs: frequency = TimerClockBase/(TimerClockDivisor*2*TimerValue)
	// base in {1,4,12,48} MHz, divisor <= 0xFF, value <= 0xFF
	// (0x0 maps to 0x100 for divisor and value)

	// pick the fastest base possible (not always optimal but whatevs)
	unsigned base;
	if (hz_req > (48000000 >> 0x11)) {
		base = 48000000;
		config_timer_clock.clock_config |= LJU3_CLOCK_48MHZ_DIV;
	} else if (hz_req > (12000000 >> 0x11)) {
		base = 12000000;
		config_timer_clock.clock_config |= LJU3_CLOCK_12MHZ_DIV;
	} else if (hz_req > (4000000 >> 0x11)) {
		base = 4000000;
		config_timer_clock.clock_config |= LJU3_CLOCK_4MHZ_DIV;
	} else {
		base = 1000000;
		config_timer_clock.clock_config |= LJU3_CLOCK_1MHZ_DIV;
	}

	// we want divisor and value to multiply close to this
	double product = (double)base / 2 / hz_req;
	// WLOG, let divisor <= value. then divisor is at least product / 0x100
	unsigned divisor_min = product / 0x100;
	// and at most ceil(sqrt(product))
	unsigned divisor_max = sqrt(product) + 1;
	// so brute force the divisors
	double err_best = fabs(1 - product);
	unsigned divisor_best = 1;
	unsigned value_best = 1;
	for (unsigned d = divisor_min; d <= divisor_max; d++) {
		// and binary search for the best value for this divisor
		unsigned v_max = 0x100;
		unsigned v_min = 0x001;
		unsigned v;
		while (v_max - v_min > 1) {
			v = (v_max + v_min) / 2;
			if (d * v < product) v_min = v;
			if (d * v > product) v_max = v;
		}
		if (fabs(d * v_max - product) < err_best) {
			divisor_best = d;
			value_best = v_max;
			err_best = fabs(d * v_max - product);
		}
		if (fabs(d * v_min - product) < err_best) {
			divisor_best = d;
			value_best = v_min;
			err_best = fabs(d * v_min - product);
		}
	}

	*hz_real = (double)base / (divisor_best * 2 * value_best);
	config_timer_clock.clock_divisor = divisor_best;

	err = lju3_config_timer_clock(dev,
		&config_timer_clock, &config_timer_clock_resp
	);
	if (err) return err;

	// now, config_io.timer_counter_config
	struct lju3_config_io config_io = {0};
	struct lju3_config_io_resp config_io_resp;
	config_io.write_mask |= 1 << 0;		// set timer_counter_config
	config_io.timer_counter_config = 1;	// enable 1 timer
	// pin offset to the requested pin
	config_io.timer_counter_config |= pin << 4;
	err = lju3_config_io(dev, &config_io, &config_io_resp);
	if (err) return err;

	// finally, feedback
	struct lju3_feedback_timer_config feedback_timer_config;
	struct lju3_feedback_resp_header feedback_timer_config_resp;
	feedback_timer_config.timer_mode = LJU3_TIMER_OUT_SQUARE;
	feedback_timer_config.value = value_best;
	err = lju3_feedback_timer_config(
		dev, &feedback_timer_config, &feedback_timer_config_resp
	);
	if (err) return err;

	return 0;
}

