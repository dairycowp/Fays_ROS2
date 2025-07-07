#ifndef FT60X_H_PI674CRI
#define FT60X_H_PI674CRI
#include "ftd3xx.h"
#define FT60X_SUPPORT

enum FT_BURST_BUFFER {
	FT_BURST_OFF = 0,
	FT_BURST_2K = 1,
	FT_BURST_4K = 2,
	FT_BURST_8K = 4,
	FT_BURST_16K = 8,
};

enum FT_FIFO_BUFFER {
	FT_FIFO_CLOSED = 0,
	FT_FIFO_2K,
	FT_FIFO_4K,
	FT_FIFO_6K,
	FT_FIFO_8K,
	FT_FIFO_10K,
	FT_FIFO_12K,
	FT_FIFO_14K,
	FT_FIFO_16K,
};

struct FT_CHIP_FLAGS {
	CONFIGURATION_FIFO_CLK fifo_clk : 2;
	BOOL fifo_600mode : 1;
	BOOL usb_self_powered : 1;
	BOOL enable_remote_wakeup : 1;
	BOOL enable_fifo_clock_during_sleep : 1;
	BOOL disable_chip_powerdown : 1;

	BOOL reserved1 : 1;

	BOOL gpio0_is_output_pin : 1;
	enum FT_GPIO_PULL gpio0_pull_control : 2;
	BOOL gpio0_schmitt_trigger : 1;

	BOOL gpio1_is_output_pin : 1;
	enum FT_GPIO_PULL gpio1_pull_control : 2;
	BOOL gpio1_schmitt_trigger : 1;

	BOOL gpio2_is_output_pin : 1;
	enum FT_GPIO_PULL gpio2_pull_control : 2;
	BOOL gpio2_schmitt_trigger : 1;
	BOOL gpio2_interrupt_enabled : 1;

	/* GPIO[0:1] output leve when enable_battery_charging is set
	 * Not connected: 00
	 * Standard downstream port (SDP): 01
	 * Charging downstream port (CDP): 10
	 * Dedicated charging port (DCP): 11
	*/
	BOOL enable_battery_charging : 1;

	uint8_t reserved2 : 2;
	uint8_t reserved3;
};

struct FT_USB_STRING_DESCRIPTOR {
	BYTE length;
	BYTE descriptor_type; /* Must set to 0x3 */
	WORD string[31];
};

struct FT_60XCOMMON_CONFIGURATION {
	WORD length;
	/* Generic USB configs */
	BYTE max_power_cs;
	BYTE max_power_ss;
	WORD vendor_id;
	WORD product_id;

	struct FT_CHIP_FLAGS chip;

	/* FIFO & EPC settings */
	/* enum FT_FIFO_BUFFER */
	BYTE fifo[FT_PIPE_DIR_COUNT][4];
	/* enum FT_BURST_BUFFER */
	BYTE epc[FT_PIPE_DIR_COUNT][4];
	char version[16];
};

#endif /* end of include guard: FT60X_H_PI674CRI */
