#include <errno.h>
#include <string.h>

#include "ljtdac.h"


/** Read calibration memory into a struct ljtdac_cal_mem. */
int ljtdac_read_cal_mem(
	HANDLE dev,
	uint8_t sda_pin, uint8_t scl_pin,
	struct ljtdac_cal_mem *cal_mem
){
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

