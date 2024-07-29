/** Low-level functions for the LabJack U3.
 * \todo Many functions are unimplemented.
 */
#ifndef LABJACK_U3_H_
#define LABJACK_U3_H_

#include "labjack_ud.h"

// pins
enum {
	LJU3_FIO0 = 0x00, LJU3_FIO1 = 0x01, LJU3_FIO2 = 0x02, LJU3_FIO3 = 0x03,
	LJU3_FIO4 = 0x04, LJU3_FIO5 = 0x05, LJU3_FIO6 = 0x06, LJU3_FIO7 = 0x07,
	LJU3_EIO0 = 0x08, LJU3_EIO1 = 0x09, LJU3_EIO2 = 0x0A, LJU3_EIO3 = 0x0B,
	LJU3_EIO4 = 0x0C, LJU3_EIO5 = 0x0D, LJU3_EIO6 = 0x0E, LJU3_EIO7 = 0x0F,
	LJU3_CIO0 = 0x10, LJU3_CIO1 = 0x11, LJU3_CIO2 = 0x12, LJU3_CIO3 = 0x13,
	LJU3_CIO4 = 0x14, LJU3_CIO5 = 0x15, LJU3_CIO6 = 0x16, LJU3_CIO7 = 0x17,
};

// https://support.labjack.com/docs/5-low-level-functionality-u3-datasheet
// this is the calibration area of the memory, not the user area
struct lju3_cal_mem {
	struct __attribute__((packed)) {
		fp64 lv_ain_se_slope;
		fp64 lv_ain_se_offset;
		fp64 lv_ain_diff_slope;
		fp64 lv_ain_diff_offset;
	} block0;
	struct __attribute__((packed)) {
		fp64 dac0_slope;
		fp64 dac0_offset;
		fp64 dac1_slope;
		fp64 dac1_offset;
	} block1 ;
	struct __attribute__((packed)) {
		fp64 temp_slope;
		fp64 vref_cal;
		fp64 reserved16;
		fp64 reserved24;
	} block2;
	struct __attribute__((packed)) {
		fp64 hv_ain0_slope;
		fp64 hv_ain1_slope;
		fp64 hv_ain2_slope;
		fp64 hv_ain3_slope;
	} block3;
	struct __attribute__((packed)) {
		fp64 hv_ain0_offset;
		fp64 hv_ain1_offset;
		fp64 hv_ain2_offset;
		fp64 hv_ain3_offset;
	} block4;
}__attribute__((packed));

typedef uint16_t lju3_config_write_mask;
enum {
	LJU3_COMPATIBILITY_DEFAULTS	= (1 << 8) << 5,
	LJU3_TIMER_DEFAUTLS		= (1 << 8) << 4,
	LJU3_LOCAL_ID			= (1 << 8) << 3,
	LJU3_DAC_DEFAULTS			= (1 << 8) << 2,
	LJU3_DIGITAL_IO_DEFAULTS		= (1 << 8) << 1,
};

typedef uint8_t lju3_timer_counter_config;
enum {
	// bits 4-7: timer counter pin offset
	LJU3_ENABLE_COUNTER1	= 1 << 3,
	LJU3_ENABLE_COUNTER0	= 1 << 2,
	// bits 0-1: number of timers enabled
};

typedef uint8_t lju3_clock_config;
enum {
	// bit 7: enable writing the clock config
	LJU3_WRITE_CLOCK_CONFIG	= 1 << 7,
	// bits 2~0: timer clock base (_DIV indicates clock_divisor is used)
	LJU3_CLOCK_4MHZ		= 0,
	LJU3_CLOCK_12MHZ	= 1,
	LJU3_CLOCK_48MHZ	= 2,
	LJU3_CLOCK_1MHZ_DIV	= 3,
	LJU3_CLOCK_4MHZ_DIV	= 4,
	LJU3_CLOCK_12MHZ_DIV	= 5,
	LJU3_CLOCK_48MHZ_DIV	= 6,
};

typedef uint8_t lju3_io_type;
enum {
	AIN			= 1,
	AIN24			= 2,
	AIN24AR			= 3,
	WAIT_SHORT		= 5,
	WAIT_LONG		= 6,
	LED			= 9,
	BIT_STATE_READ		= 10,
	BIT_STATE_WRITE		= 11,
	BIT_DIR_READ		= 12,
	BIT_DIR_WRITE		= 13,
	PORT_STATE_READ		= 26,
	PORT_STATE_WRITE	= 27,
	PORT_DIR_READ		= 28,
	PORT_DIR_WRITE		= 29,
	DAC0_8			= 34,
	DAC1_8			= 35,
	DAC0_16			= 38,
	DAC1_16			= 39,
	TIMER0			= 42,
	TIMER0_CONFIG		= 43,
	TIMER1			= 44,
	TIMER1_CONFIG		= 45,
	TIMER2			= 46,
	TIMER2_CONFIG		= 47,
	TIMER3			= 48,
	TIMER3_CONFIG		= 49,
	COUNTER0		= 54,
	COUNTER1		= 55,
	BUZZER			= 63,
};

typedef uint8_t lju3_timer_mode;
enum {
	// 16-bit PWM output
	LJU3_TIMER_OUT_PWM16	= 0,
	// 8-bit PWM output
	LJU3_TIMER_OUT_PWM8	= 1,
	// period input (32-bit, rising edges)
	LJU3_TIMER_IN_P32R	= 2,
	// period input (32-bit, falling edges)
	LJU3_TIMER_IN_P32F	= 3,
	// duty cycle input
	LJU3_TIMER_IN_DUTY	= 4,
	// firmware counter input
	LJU3_TIMER_IN_COUNT	= 5,
	// firmware counter input (with debounce)
	LJU3_TIMER_IN_COUNTD	= 6,
	// frequency output
	LJU3_TIMER_OUT_SQUARE	= 7,
	// quadrature input
	LJU3_TIMER_IN_QUAD	= 8,
	// timer stop input (odd timers only)
	LJU3_TIMER_IN_STOP	= 9,
	// system timer low read (default mode)
	LJU3_TIMER_READ_LOW	= 10,
	// system timer high read
	LJU3_TIMER_READ_HIGH	= 11,
	// period input (16-bit, rising edges)
	LJU3_TIMER_IN_P16R	= 12,
	// period input (16-bit, falling edges)
	LJU3_TIMER_IN_P16F	= 13,
	// line-to-line input
	LJU3_TIMER_IN_LINE	= 14,
};

struct lju3_config {
	struct ljud_extended_header header;
	lju3_config_write_mask write_mask;
	uint8_t local_id;
	lju3_timer_counter_config timer_counter_config;
	uint8_t fio_analog;
	uint8_t fio_direction;
	uint8_t fio_state;
	uint8_t eio_analog;
	uint8_t eio_direction;
	uint8_t eio_state;
	uint8_t cio_direction;
	uint8_t cio_state;
	uint8_t dac1_enable;
	uint8_t swdt_dac0_response;
	uint8_t swdt_dac1_response;
	lju3_clock_config clock_config;
	uint8_t clock_divisor;
	uint8_t compatibility;
	uint8_t reserved24;
	uint8_t reserved25;
}__attribute__((packed));
static_assert(sizeof(struct lju3_config) == 26, "bad lju3_config");

struct lju3_config_resp {
	struct ljud_extended_header header;
	ljud_err err;
	uint8_t reserved7;
	uint8_t reserved8;
	uint16_t firmware_version;
	uint16_t bootloader_version;
	uint16_t hardware_version;
	uint32_t serial_number;
	uint16_t product_id;
	uint8_t local_id;
	uint8_t timer_counter_mask;
	uint8_t fio_analog;
	uint8_t fio_direction;
	uint8_t fio_state;
	uint8_t eio_analog;
	uint8_t eio_direction;
	uint8_t eio_state;
	uint8_t cio_direction;
	uint8_t cio_state;
	uint8_t dac1_enable;
	uint8_t dac0;
	uint8_t dac1;
	uint8_t clock_config;
	uint8_t clock_divisor;
	uint8_t compatibility;
	uint8_t version_info;
}__attribute__((packed));
static_assert(sizeof(struct lju3_config_resp) == 38, "bad lju3_config_resp");

struct lju3_config_io {
	struct ljud_extended_header header;
	uint8_t write_mask;
	uint8_t reserved7;
	lju3_timer_counter_config timer_counter_config;
	uint8_t dac1_enable;
	uint8_t fio_analog;
	uint8_t eio_analog;
}__attribute__((packed));
static_assert(sizeof(struct lju3_config_io) == 12, "bad lju3_config_io");

struct lju3_config_io_resp {
	struct ljud_extended_header header;
	ljud_err err;
	uint8_t reserved7;
	lju3_timer_counter_config timer_counter_config;
	uint8_t dac1_enable;
	uint8_t fio_analog;
	uint8_t eio_analog;
}__attribute__((packed));
static_assert(
	sizeof(struct lju3_config_io_resp) == 12, "bad lju3_config_io_resp"
);

struct lju3_config_timer_clock {
	struct ljud_extended_header header;
	uint8_t reserved6;
	uint8_t reserved7;
	uint8_t clock_config;
	uint8_t clock_divisor;
}__attribute__((packed));
static_assert(sizeof(struct lju3_config_timer_clock) == 10,
	"bad lju3_config_timer_clock"
);

struct lju3_config_timer_clock_resp {
	struct ljud_extended_header header;
	ljud_err err;
	uint8_t reserved7;
	uint8_t clock_config;
	uint8_t clock_divisor;
}__attribute__((packed));
static_assert(sizeof(struct lju3_config_timer_clock_resp) == 10,
	"bad lju3_config_timer_clock_resp"
);

struct lju3_feedback_header {
	struct ljud_extended_header header;
	uint8_t echo;
}__attribute__((packed));
static_assert(
	sizeof(struct lju3_feedback_header) == 7, "bad lju3_feedback_header"
);

struct lju3_feedback_resp_header {
	struct ljud_extended_header header;
	ljud_err err;
	uint8_t error_frame;
	uint8_t echo;
	uint8_t _padding;
}__attribute__((packed));
static_assert(sizeof(struct lju3_feedback_resp_header) == 9 + 1,
	"bad lju3_feedback_resp_header"
);

struct lju3_feedback_timer_config {
	struct lju3_feedback_header header;
	lju3_io_type io_type;
	lju3_timer_mode timer_mode;
	uint16_t value;
	uint8_t _padding;
}__attribute__((packed));
static_assert(sizeof(struct lju3_feedback_timer_config) == 7 + 4 + 1,
	"bad lju3_feedback_timer_config"
);

struct lju3_readmem {
	struct ljud_extended_header header;
	uint8_t reserved6;
	uint8_t block_num;
}__attribute__((packed));
static_assert(sizeof(struct lju3_readmem) == 8, "bad lju3_readmem");

struct lju3_readmem_resp {
	struct ljud_extended_header header;
	ljud_err err;
	uint8_t reserved7;
	uint8_t data[32];
}__attribute__((packed));
static_assert(sizeof(struct lju3_readmem_resp) == 40, "bad lju3_readmem_resp");

/** Set and get the timer clock configuration using a ConfigTimerClock
 * command. Will set header and check checksums for you.
 */
int lju3_config_timer_clock(HANDLE dev,
	struct lju3_config_timer_clock *config,
	struct lju3_config_timer_clock_resp *config_resp
);

/** Set and get the IO configuration using a ConfigIO command.
 * Will set header and check checksums for you.
 */
int lju3_config_io(HANDLE dev,
	struct lju3_config_io *config, struct lju3_config_io_resp *config_resp
);

/** Read calibration memory into a struct lju3_cal_mem.
 * \warning This function is untested!
 */
int lju3_read_cal_mem(HANDLE dev, struct lju3_cal_mem *cal_mem);

/** Get the current device configuration using a ConfigU3 command. */
int lju3_read_config(HANDLE dev, struct lju3_config_resp *config_resp);

/** Start outputting a square wave on the specified pin.
 * \todo: only supports one timer at any given time.
 */
int lju3_square(
	HANDLE dev, ljud_pin pin, unsigned long hz_req, double *hz_real
);

#endif

