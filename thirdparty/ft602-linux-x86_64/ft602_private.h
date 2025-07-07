#ifndef FT602_PRIVATE_H_EKYVTQBG
#define FT602_PRIVATE_H_EKYVTQBG

#include "ft60x.h"

#define CHANNEL_COUNT		4
#define RESOLUTION_COUNT	14

enum I2C_SPEED {
	I2C_DISABLED,
	I2C_1MHZ,
	I2C_400KHZ,
	I2C_100KHZ,
};

enum FT602_STRING_DESC_TYPE {
	FT602_STR_DESC_LANGID_ARRAY,
	FT602_STR_DESC_MANUFACTURER,
	FT602_STR_DESC_PRODUCT,
	FT602_STR_DESC_CHANNEL1,
	FT602_STR_DESC_CHANNEL2,
	FT602_STR_DESC_CHANNEL3,
	FT602_STR_DESC_CHANNEL4,
	FT602_STR_DESC_SERIALNUMBER,
	FT602_STR_DESC_COUNT,
	FT602_STR_DESC_OTHER_COUNT = FT602_STR_DESC_CHANNEL4,
};

enum USB_SPEED_TYPE {
	USB_SPEED_FS,
	USB_SPEED_HS,
	USB_SPEED_SS,
	USB_SPEED_COUNT,
};

enum CONTROL_TYPE {
	/* Order at firmware side is default, resolution, minimal, maximum */
	TYPE_NONE = 0,
	TYPE_DEF = 1,
	TYPE_DEF_RES = 2,
	TYPE_DEF_RES_MIN_MAX = 4,
};

struct FRAME_RESOLUTION {
	WORD wWidth;
	WORD wHeight;
	DWORD dwPixelClock;
	DWORD dwFrameInterval; /* 10000000 / fps */
};

struct FRAME_INFO {
	struct FRAME_RESOLUTION res[RESOLUTION_COUNT];

	DWORD dwFourcc;

	BYTE byBitsPerPixel;
	/* Color matching, UVC 1.1 p68 */
	BYTE bColorPrimaries;
	BYTE bTransferCharacteristics;
	BYTE bMatrixCoefficients;
};

struct UVC_CONTROL_CONFIG {
	DWORD info_get_support; /* bitmap of enum UVC_REGS */
	DWORD info_set_support; /* bitmap of enum UVC_REGS */

	/* Camera Terminal bmControls Bitmaps:
		D0: Scanning Mode - not supported
		D1: Auto-Exposure Mode
		D2: Auto-Exposure Priority
		D3: Exposure Time (Absolute)
		D4: Exposure Time (Relative)
		D5: Focus (Absolute)
		D6 : Focus (Relative)
		D7: Iris (Absolute)
		D8 : Iris (Relative)
		D9: Zoom (Absolute)
		D10: Zoom (Relative)
		D11: PanTilt (Absolute)
		D12: PanTilt (Relative)
		D13: Roll (Absolute)
		D14: Roll (Relative)
		D15: Reserved
		D16: Reserved
		D17: Focus, Auto
		D18: Privacy - not supported */
	BYTE bmCameraTerminalControls[3];
	/* Process Unit bmControls Bitmaps:
		D0: Brightness
		D1: Contrast
		D2: Hue
		D3: Saturation
		D4: Sharpness
		D5: Gamma
		D6: White Balance Temperature
		D7: White Balance Component
		D8: Backlight Compensation
		D9: Gain
		D10: Power Line Frequency
		D11: Hue, Auto
		D12: White Balance Temperature, Auto
		D13: White Balance Component, Auto
		D14: Digital Multiplier
		D15: Digital Multiplier Limit
		D16: Analog Video Standard
		D17: Analog Video Lock Status - not supported */
	BYTE bmProcessUnitControls[3];
	/* bmVideoStandards bitmaps:
		D0: None
		D1: NTSC – 525/60
		D2: PAL – 625/50
		D3: SECAM – 625/50
		D4: NTSC – 625/50
		D5: PAL – 525/60 */
	BYTE bmVideoStandards;
	/* Start of default value of UVC controls */
	BYTE bAutoFocusMode;

	DWORD dwExposureTimeAbsolute[TYPE_DEF_RES_MIN_MAX];
	WORD wFocusAbsolute[TYPE_DEF_RES_MIN_MAX];
	struct {
		char bFocusRelatve;
		BYTE bSpeed;
	} stFocusRelative[TYPE_DEF_RES_MIN_MAX];
	WORD wIrisAbsolute[TYPE_DEF_RES_MIN_MAX];
	WORD wObjectiveFocalLength[TYPE_DEF_RES_MIN_MAX];
	struct {
		char bZoom;
		bool bDigitalZoom;
		BYTE bSpeed;
	} stZoomRelative[TYPE_DEF_RES_MIN_MAX];
	struct {
		int32_t dwPanAbsolute;
		int32_t dwTiltAbsolute;
	} stPanTiltAbsolute[TYPE_DEF_RES_MIN_MAX];
	struct {
		char bPanRelative;
		BYTE bPanSpeed;
		char bTiltRelatve;
		BYTE bTiltSpeed;
	} stPanTiltRelative[TYPE_DEF_RES_MIN_MAX];
	int16_t wRollAbsolute[TYPE_DEF_RES_MIN_MAX];
	struct {
		char bRollRelative;
		BYTE bSpeed;
	} stRollRelative[TYPE_DEF_RES_MIN_MAX];
	WORD wBacklightCompensation[TYPE_DEF_RES_MIN_MAX];
	int16_t wBrightness[TYPE_DEF_RES_MIN_MAX];
	WORD wContrast[TYPE_DEF_RES_MIN_MAX];
	WORD wGain[TYPE_DEF_RES_MIN_MAX];
	int16_t wHue[TYPE_DEF_RES_MIN_MAX];
	WORD wSaturation[TYPE_DEF_RES_MIN_MAX];
	WORD wSharpness[TYPE_DEF_RES_MIN_MAX];
	WORD wGamma[TYPE_DEF_RES_MIN_MAX];
	WORD wWhiteBalanceTemperature[TYPE_DEF_RES_MIN_MAX];
	struct {
		WORD wWhiteBalanceBlue;
		WORD wWhiteBalanceRed;
	} stWhiteBalanceComponent[TYPE_DEF_RES_MIN_MAX];
	WORD wMultiplierStep[TYPE_DEF_RES_MIN_MAX];
	WORD wMultiplierLimit[TYPE_DEF_RES_MIN_MAX];
	BYTE bAutoExposureMode[TYPE_DEF_RES];
	BYTE bWhiteBalanceComponentAuto;
	BYTE wWhiteBalanceTemperatureAuto;

	BYTE bHueAuto;
	BYTE bPowerLineFrequency;
	/* End of default value of UVC controls */
};

struct I2C_CONFIG {
	/* I2C must be enabled if:
	 * info_get_support != 0 || info_set_support != 0
	 * Defined more than 1 resolution in each FRAME_INFO */
	enum I2C_SPEED speed : 2;

	bool reserved : 1;

	/* 1 - 16, default 32ms (2^9-1)*125us */
	uint8_t interrupt_endpoint_interval : 5;

	uint8_t address;
} __attribute__((packed));

typedef struct _FT_602CONFIGURATION {
	struct FT_60XCOMMON_CONFIGURATION common;
	struct FT_USB_STRING_DESCRIPTOR desc[FT602_STR_DESC_COUNT];
	/* Second language only support manufacturer and product string */
	struct FT_USB_STRING_DESCRIPTOR desc_other[FT602_STR_DESC_OTHER_COUNT];

	struct FRAME_INFO frame[CHANNEL_COUNT][USB_SPEED_COUNT];
	struct UVC_CONTROL_CONFIG controls[CHANNEL_COUNT];
	struct I2C_CONFIG i2c;
} FT_602CONFIGURATION, *PFT_602CONFIGURATION;

#endif /* end of include guard: FT602_PRIVATE_H_EKYVTQBG */
