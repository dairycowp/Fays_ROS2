#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include <csignal>
#include <cstring>
#include "ftd3xx.h"
#include "bitbang.h"

using namespace std;
static bool do_exit;

static void sig_hdlr(int signum)
{
	switch (signum) {
	case SIGINT:
		do_exit = true;
		break;
	}
}

static void register_signals(void)
{
	signal(SIGINT, sig_hdlr);
}

int main(int argc, char *argv[])
{
	enum BITBANG_I2C {
		GPIO0_SCL = FT600_GPIO0,
		GPIO0_SDA = FT600_GPIO1,
	};

	/* Must be called before FT_Create is called */
	FT_BitBangInit();

	FT_HANDLE handle;

	FT_Create(0, FT_OPEN_BY_INDEX, &handle);

	if (!handle) {
		printf("Failed to create device\r\n");
		return -1;
	}

	uint8_t out[32768];
	uint32_t in[32768];
	uint8_t *ptr = out;
	bool clock = true;
	bool data = true;

	for (int i = 0; i < 32768 / 2; i++) {
		/* clock Low */
		data = !data;
		clock = !clock;
		*ptr++ = bitbang_set_level(clock, data, 0);
		/* clock High */
		clock = !clock;
		*ptr++ = bitbang_set_level(clock, data, 0);
	}

	printf("Bit Banging\r\n");
	int i = 0;
	while (!do_exit) {
		printf("%d\r\n", i++);
		FT_STATUS status = FT_BitBangOut(handle, out, 32768);
		//FT_STATUS status = FT_BitBangInAtBothLevel(handle, out, in, 32768);
		//FT_STATUS status = FT_BitBangInAtSingleLevel(handle, out, in, 32768);
		if (status != FT_OK) {
			printf("Err:%d\r\n", status);
			break;
		}
	}

	FT_Close(handle);
	return 0;
}
