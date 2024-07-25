/** Low-level functions for LabJack UD devices.
 * See https://support.labjack.com/docs/low-level-functions-ud-devices-only for
 * documentation. Depends on liblabjackusb.
 * \warning This is only tested on the U3-HV!
 * \todo Many functions are unimplemented.
 */
#ifndef AYLP_LABJACK_UD_H_
#define AYLP_LABJACK_UD_H_

#include <stdint.h>
#include "labjackusb.h"

#ifndef static_assert
	#define static_assert _Static_assert
#endif


/** Error codes. */
typedef uint8_t ljud_err;
enum {
	LJ_SCRATCH_WRT_FAIL		= 0x01,
	LJ_SCRATCH_ERASE_FAIL		= 0x02,
	LJ_DATA_BUFFER_OVERFLOW		= 0x03,
	LJ_ADC0_BUFFER_OVERFLOW		= 0x04,
	LJ_FUNCTION_INVALID		= 0x05,
	LJ_SWDT_TIME_INVALID		= 0x06,
	LJ_XBR_CONFIG_ERROR		= 0x07,
	LJ_FLASH_WRITE_FAIL		= 0x10,
	LJ_FLASH_ERASE_FAIL		= 0x11,
	LJ_FLASH_JMP_FAIL		= 0x12,
	LJ_FLASH_PSP_TIMEOUT		= 0x13,
	LJ_FLASH_ABORT_RECIEVED		= 0x14,
	LJ_FLASH_PAGE_MISMATCH		= 0x15,
	LJ_FLASH_BLOCK_MISMATCH		= 0x16,
	LJ_FLASH_PAGE_NOT_IN_CODE_AREA	= 0x17,
	LJ_MEM_ILLEGAL_ADDRESS		= 0x18,
	LJ_FLASH_LOCKED			= 0x19,
	LJ_INVALID_BLOCK		= 0x1A,
	LJ_FLASH_ILLEGAL_PAGE		= 0x1B,
	LJ_FLASH_TOO_MANY_BYTES		= 0x1C,
	LJ_FLASH_INVALID_STRING_NUM	= 0x1D,
	LJ_SMBUS_INQ_OVERFLOW		= 0x20,
	LJ_SMBUS_OUTQ_UNDERFLOW		= 0x21,
	LJ_SMBUS_CRC_FAILED		= 0x22,
	LJ_SHT1x_COMM_TIME_OUT		= 0x28,
	LJ_SHT1x_NO_ACK			= 0x29,
	LJ_SHT1x_CRC_FAILED		= 0x2A,
	LJ_SHT1X_TOO_MANY_W_BYTES	= 0x2B,
	LJ_SHT1X_TOO_MANY_R_BYTES	= 0x2C,
	LJ_SHT1X_INVALID_MODE		= 0x2D,
	LJ_SHT1X_INVALID_LINE		= 0x2E,
	LJ_STREAM_IS_ACTIVE		= 0x30,
	LJ_STREAM_TABLE_INVALID		= 0x31,
	LJ_STREAM_CONFIG_INVALID	= 0x32,
	LJ_STREAM_BAD_TRIGGER_SOURCE	= 0x33,
	LJ_STREAM_NOT_RUNNING		= 0x34,
	LJ_STREAM_INVALID_TRIGGER	= 0x35,
	LJ_STREAM_ADC0_BUFFER_OVERFLOW	= 0x36,
	LJ_STREAM_SCAN_OVERLAP		= 0x37,
	LJ_STREAM_SAMPLE_NUM_INVALID	= 0x38,
	LJ_STREAM_BIPOLAR_GAIN_INVALID	= 0x39,
	LJ_STREAM_SCAN_RATE_INVALID	= 0x3A,
	LJ_STREAM_AUTORECOVER_ACTIVE	= 0x3B,
	LJ_STREAM_AUTORECOVER_REPORT	= 0x3C,
	LJ_STREAM_SOFTPWM_ON		= 0x3D,
	LJ_STREAM_INVALID_RESOLUTION	= 0x3F,
	LJ_PCA_INVALID_MODE		= 0x40,
	LJ_PCA_QUADRATURE_AB_ERROR	= 0x41,
	LJ_PCA_QUAD_PULSE_SEQUENCE	= 0x42,
	LJ_PCA_BAD_CLOCK_SOURCE		= 0x43,
	LJ_PCA_STREAM_ACTIVE		= 0x44,
	LJ_PCA_PWMSTOP_MODULE_ERROR	= 0x45,
	LJ_PCA_SEQUENCE_ERROR		= 0x46,
	LJ_PCA_LINE_SEQUENCE_ERROR	= 0x47,
	LJ_TMR_SHARING_ERROR		= 0x48,
	LJ_EXT_OSC_NOT_STABLE		= 0x50,
	LJ_INVALID_POWER_SETTING	= 0x51,
	LJ_PLL_NOT_LOCKED		= 0x52,
	LJ_INVALID_PIN			= 0x60,
	LJ_PIN_CONFIGURED_FOR_ANALOG	= 0x61,
	LJ_PIN_CONFIGURED_FOR_DIGITAL	= 0x62,
	LJ_IOTYPE_SYNCH_ERROR		= 0x63,
	LJ_INVALID_OFFSET		= 0x64,
	LJ_IOTYPE_NOT_VALID		= 0x65,
	LJ_INVALID_CODE			= 0x66,
	LJ_UART_TIMEOUT			= 0x70,
	LJ_UART_NOTCONNECTED		= 0x71,
	LJ_UART_NOTENALBED		= 0x72,
	LJ_I2C_BUS_BUSY			= 0x74,
	LJ_TOO_MANY_BYTES		= 0x76,
	LJ_TOO_FEW_BYTES		= 0x77,
	LJ_DSP_PERIOD_DETECTION_ERROR	= 0x80,
	LJ_DSP_SIGNAL_OUT_OF_RANGE	= 0x81,
	LJ_MODBUS_RSP_OVERFLOW		= 0x90,
	LJ_MODBUS_CMD_OVERFLOW		= 0x91,
};

/** 2-byte normal response given when LJ detects bad checksum. */
#define LJ_BAD_CHECKSUM 0xB8B8

// https://support.labjack.com/docs/general-protocol-ud-devices-only
struct ljud_normal_header {
	uint8_t checksum8;
	uint8_t command;
}__attribute__((packed));
struct ljud_extended_header {
	uint8_t checksum8;
	uint8_t command;
	uint8_t n_data_words;
	uint8_t extended_command;
	uint16_t checksum16;
}__attribute__((packed));

typedef uint8_t ljud_i2c_options;
enum {
	// bits 7-4: reserved
	LJ_I2C_ENABLE_CLOCK_STRETCHING	= 1 << 3,
	LJ_I2C_NO_STOP_WHEN_RESTART	= 1 << 2,
	LJ_I2C_RESET_AT_START		= 1 << 1,
	// bit 0: reserved
};

struct ljud_i2c_header {
	struct ljud_extended_header header;
	ljud_i2c_options i2c_options;
	uint8_t speed_adjust;
	uint8_t sdap_pin_num;
	uint8_t scl_pin_num;
	uint8_t address_byte;
	uint8_t reserved11;
	uint8_t n_i2c_bytes_tx;
	uint8_t n_i2c_bytes_rx;
}__attribute__((packed));
static_assert(sizeof(struct ljud_i2c_header) == 14, "bad ljud_i2c_header");


/** Perform the LJ UD 8-bit checksum on some data.
 * \warning the exodriver example code does this differently.
 */
uint8_t ljud_checksum8(uint8_t *data, uint8_t len);

/** Perform the LJ UD 16-bit checksum on some data. */
uint16_t ljud_checksum16(uint8_t *data, uint8_t len);


#endif

