#ifndef FT602_H_KICXBTFF
#define FT602_H_KICXBTFF
#include "ftd3xx.h"

#define UVC_SLAVE_ADDRESS 0x0D
#define GET_UVC_REG_INDEX(channel, reg) ((uint16_t)(0x40 + 0x30 * (channel) + (reg)))

enum DEVICES_REGS {
	REG_DEV_CONFIGURED, /* struct dev_configured */
	REG_DEV_POWER_EVENT, /* 0 = wakeup, 1 = suspend, 2 = power off */
};

enum UVC_REGS {
	REG_CT_AE_MODE,
	REG_CT_AE_PRIORITY,
	REG_CT_EXPOSURE_TIME_ABSOLUTE,
	REG_CT_EXPOSURE_TIME_RELATIVE,
	REG_CT_FOCUS_ABSOLUTE,
	REG_CT_FOCUS_RELATIVE,
	REG_CT_FOCUS_AUTO,
	REG_CT_IRIS_ABSOLUTE,
	REG_CT_IRIS_RELATIVE,
	REG_CT_ZOOM_ABSOLUTE,
	REG_CT_ZOOM_RELATIVE,
	REG_CT_PANTILT_ABSOLUTE,
	REG_CT_PANTILT_RELATIVE,
	REG_CT_ROLL_ABSOLUTE,
	REG_CT_ROLL_RELATIVE,
	REG_PU_BACKLIGHT_COMPENSATION,
	REG_PU_BRIGHTNESS,
	REG_PU_CONTRAST,
	REG_PU_GAIN,
	REG_PU_POWER_LINE_FREQUENCY,
	REG_PU_HUE,
	REG_PU_SATURATION,
	REG_PU_SHARPNESS,
	REG_PU_GAMMA,
	REG_PU_WHITE_BALANCE_TEMPERATURE,
	REG_PU_WHITE_BALANCE_TEMPERATURE_AUTO,
	REG_PU_WHITE_BALANCE_COMPONENT,
	REG_PU_WHITE_BALANCE_COMPONENT_AUTO,
	REG_PU_DIGITAL_MULTIPLIER,
	REG_PU_DIGITAL_MULTIPLIER_LIMIT,
	REG_PU_HUE_AUTO,
	REG_PU_ANALOG_VIDEO_STANDARD,
	UVC_REG_COUNT,
	REG_STREAM_START = UVC_REG_COUNT, /* struct stream_start */
};

struct dev_configured {
	int ft600_mode : 1;
	int usb_speed : 2;
	int i2c_interrupt_enabled : 1;
};

struct stream_start {
	/* All members are big endian */
	uint16_t width;
	uint16_t height;
	uint32_t pixel_clock;
	uint8_t resolution_index; /* zero based */
};

struct i2c_access {
	uint8_t addr;
	uint8_t len;
	uint16_t reg;

	bool reg_is_16_bit; /* 16bit register */
	bool read_access; /* false means write access */
};

static inline FT_STATUS FT_I2CAccess(FT_HANDLE ftHandle,
	const struct i2c_access *i2c, uint8_t *buf)
{
	FT_SETUP_PACKET fsp;

	if (!i2c || !buf)
		return FT_INVALID_PARAMETER;
	if (i2c->len > 128)
		return FT_INSUFFICIENT_RESOURCES;
	fsp.RequestType = (2 << 5);
	fsp.Request = 0x5;
	fsp.Index = *(uint16_t *)i2c;
	fsp.Value = *((uint16_t *)i2c + 1);

	if (i2c->read_access)
		fsp.RequestType |= 0x80;

	if (i2c->reg_is_16_bit)
		fsp.Index |= 0x80;
	else
		fsp.Index &= ~0x80;
	fsp.Length = i2c->len;

	return FT_ControlTransfer(ftHandle, fsp, buf, i2c->len, NULL);
}

static inline FT_STATUS FT_UVCRegAccess(FT_HANDLE ftHandle, uint8_t channel,
		uint8_t reg, uint8_t len, bool read_access, uint8_t *buf)
{
	const struct i2c_access ia = {
		UVC_SLAVE_ADDRESS,
		len,
		GET_UVC_REG_INDEX(channel, reg),
		false,
		read_access
	};

	return FT_I2CAccess(ftHandle, &ia, buf);
}

#endif /* end of include guard: FT602_H_KICXBTFF */
