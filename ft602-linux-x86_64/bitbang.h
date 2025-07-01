#ifndef BITBANG_H_Q1OURUGL
#define BITBANG_H_Q1OURUGL
#include "ftd3xx.h"

/**********************************************************************
*                   Bit Bang Mode helper functions                   *
**********************************************************************/
#define BITBANG_CHANNEL 4

enum bitbang_id {
	BITBANG_OUT,
	BITBANG_IN_BOTH_LEVEL,
	BITBANG_IN_SINGLE_LEVEL,
};

struct bitbang_header {
	uint32_t len;
	uint8_t id;
} __attribute__((aligned(4)));

enum BITBANG_PIN {
	FT600_GPIO0, /* IO */
	FT600_GPIO1, /* IO */
	FT600_GPIO2, /* IO */
	FT600_OE_N, /* In */
	FT600_RD_N, /* In */
};

static inline uint8_t bitbang_set_level(
		bool gpio0_level,
		bool gpio1_level,
		bool gpio2_level)
{
	return (uint8_t)((gpio0_level << FT600_GPIO0) |
		(gpio1_level << FT600_GPIO1) |
		(gpio2_level << FT600_GPIO2));
}

static inline bool bitbang_get_level(
		uint32_t in,
		uint8_t pin)
{
	switch (pin) {
		case FT600_GPIO0:
		case FT600_GPIO1:
		case FT600_GPIO2:
			return !!(in & (1 << (pin + 4)));
		case FT600_OE_N:
			return !!(in & (1 << 16));
		case FT600_RD_N:
			return !!(in & (1 << 17));
	}
	return false;
}

static inline FT_STATUS send_bitbang_header(
		FT_HANDLE handle,
		uint8_t id,
		uint32_t len)
{
	struct bitbang_header hdr = {
		.len = len,
		.id = id,
	};
	DWORD dwRet;

	return FT_WritePipeEx(handle, BITBANG_CHANNEL, (PUCHAR)&hdr,
			sizeof(struct bitbang_header),
			&dwRet, 1000);
}

static void FT_BitBangInit(void)
{
	FT_TRANSFER_CONF conf;

	memset(&conf, 0, sizeof(FT_TRANSFER_CONF));
	conf.wStructSize = sizeof(FT_TRANSFER_CONF);
	conf.fBitBangMode = true;
	conf.pipe[0].dwURBBufferSize = 1024;
	conf.pipe[0].bURBCount = 3;

	for (int i = 0; i < 4; i++)
		FT_SetTransferParams(&conf, i);
}

static inline FT_STATUS FT_BitBangOut(
		FT_HANDLE handle,
		uint8_t *out,
		uint32_t len)
{
	uint32_t wrote;
	FT_STATUS status = send_bitbang_header(handle, BITBANG_OUT, len);

	if (status != FT_OK)
		return status;
	return FT_WritePipeEx(handle, BITBANG_CHANNEL, out, len,
			(PULONG)&wrote, -1U);
}

static inline FT_STATUS FT_BitBangInAtBothLevel(
		FT_HANDLE handle,
		uint8_t *out,
		uint32_t *in,
		uint32_t out_len)
{
	FT_STATUS status = send_bitbang_header(handle, BITBANG_IN_BOTH_LEVEL,
			out_len);

	if (status != FT_OK)
		return status;

	uint32_t in_len = out_len * sizeof(uint32_t);

	while (in_len) {
		if (out_len) {
			uint32_t wrote = 0;

			status = FT_WritePipeEx(handle, BITBANG_CHANNEL,
					out, out_len, (PULONG)&wrote, 1);
			if (status != FT_OK && status != FT_TIMEOUT)
				return status;
			out += wrote;
			out_len -= wrote;
		}
		uint32_t read = 0;

		status = FT_ReadPipeEx(handle, BITBANG_CHANNEL,
				(uint8_t *)in, in_len, (PULONG)&read, 1);
		if (status != FT_OK && status != FT_TIMEOUT)
			return status;
		in += read / sizeof(uint32_t);
		in_len -= read;
	}
	return FT_OK;
}

static inline FT_STATUS FT_BitBangInAtSingleLevel(
		FT_HANDLE handle,
		uint8_t *out,
		uint32_t *in,
		uint32_t out_len)
{
	FT_STATUS status = send_bitbang_header(handle, BITBANG_IN_SINGLE_LEVEL,
			out_len);

	if (status != FT_OK)
		return status;

	uint32_t in_len = (out_len / 2) * sizeof(uint32_t);

	while (in_len) {
		if (out_len) {
			uint32_t wrote = 0;

			status = FT_WritePipeEx(handle, BITBANG_CHANNEL,
					out, out_len, (PULONG)&wrote, 1);
			if (status != FT_OK && status != FT_TIMEOUT)
				return status;
			out += wrote;
			out_len -= wrote;
		}
		uint32_t read = 0;

		status = FT_ReadPipeEx(handle, BITBANG_CHANNEL,
				(uint8_t *)in, in_len, (PULONG)&read, 1);
		if (status != FT_OK && status != FT_TIMEOUT)
			return status;
		in += read / sizeof(uint32_t);
		in_len -= read;
	}
	return FT_OK;
}

#endif /* end of include guard: BITBANG_H_Q1OURUGL */
