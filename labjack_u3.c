#include <errno.h>
#include <string.h>

#include "labjack_u3.h"


/** \warning UNTESTED CODE */
int lju3_read_cal_mem(HANDLE dev, struct lju3_cal_mem *cal_mem)
{
	unsigned long n;
	struct lju3_readmem tx;
	struct lju3_readmem_resp *rx;

	tx.header.command = 0xF8;
	tx.header.n_data_words = 1;
	tx.header.extended_command = 0x2D;

	for (unsigned i = 0; i <= 4; i++) {
		tx.block_num = i;
		tx.header.checksum8 = ljud_checksum8(
			(uint8_t *)&tx + 1, sizeof(struct lju3_readmem) - 1
		);
		tx.header.checksum16 = ljud_checksum16(
			(uint8_t *)&tx + 6, sizeof(struct lju3_readmem) - 6
		);

		n = LJUSB_Write(dev, (void *)&tx, sizeof(struct lju3_readmem));
		if (n < sizeof(struct lju3_readmem)) return errno;

		n = LJUSB_Read(
			dev, (void *)&rx, sizeof(struct lju3_readmem_resp)
		);
		if (n < sizeof(struct lju3_readmem_resp)) return errno;

		memcpy(
			(uint8_t *)cal_mem + sizeof(cal_mem->block0),
			rx, sizeof(cal_mem->block0)
		);
	}

	return 0;
}

