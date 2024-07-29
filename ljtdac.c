#include <errno.h>
#include <string.h>

#include "ljtdac.h"

// see: analog.com/media/en/technical-documentation/data-sheets/26071727fa.pdf

// i2c addresses
const uint8_t LJTDAC_EEPROM_I2C = 0xA0;
const uint8_t LJTDAC_DAC_I2C = 0x24;

// address in i2c memory where cal mem starts
const uint8_t LJTDAC_CAL_MEM_START = 0x40;

struct ljtdac_input{
	ljtdac_output output;
	uint8_t value_high;
	uint8_t value_low;
}__attribute__((packed));
static_assert(sizeof(struct ljtdac_input) == 3, "bad ljtdac_input");


int ljtdac_read_cal_mem(
	HANDLE dev, struct ljtdac_cal_mem *cal_mem,
	uint8_t sda_pin, uint8_t scl_pin
) {
	unsigned long n;
	// we only have one I2C data byte to send, but command packets are
	// multiple of words long, so we have to add one extra zero of padding
	const unsigned n_tx = sizeof(struct ljud_i2c_header) + 2;
	const unsigned n_rx = (
		sizeof(struct ljud_i2c_resp_header)
		+ sizeof(struct ljtdac_cal_mem)
	);
	const unsigned n_head = sizeof(struct ljud_extended_header);
	uint8_t tx[sizeof(struct ljud_i2c_header) + 2] = {0};
	uint8_t rx[
		sizeof(struct ljud_i2c_resp_header)
		+ sizeof(struct ljtdac_cal_mem)
	];

	tx[sizeof(struct ljud_i2c_header)] = LJTDAC_CAL_MEM_START;

	struct ljud_i2c_header *head = (struct ljud_i2c_header *)tx;
	head->sda_pin = sda_pin;
	head->scl_pin = scl_pin;
	head->address_byte = LJTDAC_EEPROM_I2C;
	head->n_i2c_bytes_tx = 1;
	head->n_i2c_bytes_rx = sizeof(struct ljtdac_cal_mem);

	head->header.command = 0xF8;
	head->header.extended_command = 0x3B;
	head->header.n_data_words = (n_tx - n_head) / 2;
	head->header.checksum16 = ljud_checksum16(tx + 6, n_tx - 6);
	head->header.checksum8 = ljud_checksum8(tx + 1, n_head - 1);

	n = LJUSB_Write(dev, tx, n_tx);
	if (n < n_tx) return -ECOMM;

	n = LJUSB_Read(dev, rx, n_rx);
	if (n < n_rx) {
		// LJ is telling us we have a bad checksum
		if (*(uint16_t *)rx == 0xB8B8) return -EBADMSG;
		return -EREMOTEIO;
	}

	if (
		((struct ljud_i2c_resp_header *)rx)->header.checksum16
		!= ljud_checksum16(rx + 6, n_rx - 6)
	) {
		return -EBADE;
	}

	memcpy(
		cal_mem, rx + sizeof(struct ljud_i2c_resp_header),
		sizeof(struct ljtdac_cal_mem)
	);
	return 0;
}


int ljtdac_write_dac(
	HANDLE dev, struct ljtdac_cal_mem *cal_mem,
	uint8_t sda_pin, uint8_t scl_pin,
	bool fast, ljtdac_output output, double voltage
) {
	unsigned long n;
	// we only have three I2C data bytes to send, but command packets are
	// multiple of words long, so we have to add one extra zero of padding
	const unsigned n_tx = (
		sizeof(struct ljud_i2c_header) + sizeof(struct ljtdac_input) + 1
	);
	const unsigned n_rx = sizeof(struct ljud_i2c_resp_header);
	const unsigned n_head = sizeof(struct ljud_extended_header);
	uint8_t tx[
		sizeof(struct ljud_i2c_header) + sizeof(struct ljtdac_input) + 1
	] = {0};
	uint8_t rx[sizeof(struct ljud_i2c_resp_header)];

	struct ljtdac_input *input = (struct ljtdac_input *)(
		tx + sizeof(struct ljud_i2c_header)
	);
	input->output = output;
	switch (output) {
	case LJTDAC_WRITE_DACA:
		voltage *= fp642dbl(cal_mem->daca_slope);
		voltage += fp642dbl(cal_mem->daca_offset);
		break;
	case LJTDAC_WRITE_DACB:
		voltage *= fp642dbl(cal_mem->dacb_slope);
		voltage += fp642dbl(cal_mem->dacb_offset);
		break;
	default:
		return -EINVAL;
	}
	unsigned value = voltage;
	if ((value) > 0xFFFF) value = 0xFFFF;
	input->value_high = value >> 8;
	input->value_low = value & 0xFF;

	struct ljud_i2c_header *head = (struct ljud_i2c_header *)tx;
	head->sda_pin = sda_pin;
	head->scl_pin = scl_pin;
	head->address_byte = LJTDAC_DAC_I2C;
	head->n_i2c_bytes_tx = sizeof(struct ljtdac_input);
	head->n_i2c_bytes_rx = 0;

	head->header.command = 0xF8;
	head->header.extended_command = 0x3B;
	head->header.n_data_words = (n_tx - n_head) / 2;
	head->header.checksum16 = ljud_checksum16(tx + 6, n_tx - 6);
	head->header.checksum8 = ljud_checksum8(tx + 1, n_head - 1);

	n = LJUSB_Write(dev, tx, n_tx);
	if (n < n_tx) return -ECOMM;

	// reading things we don't need to know is slow!
	if (fast) return 0;

	n = LJUSB_Read(dev, rx, n_rx);
	if (n < n_rx) {
		// LJ is telling us we have a bad checksum
		if (*(uint16_t *)rx == 0xB8B8) return -EBADMSG;
		return -EREMOTEIO;
	}

	if (
		((struct ljud_i2c_resp_header *)rx)->header.checksum16
		!= ljud_checksum16(rx + 6, n_rx - 6)
	) {
		return -EBADE;
	}

	return 0;
}

