#include "ft602.h"
#include <unistd.h>
#include <stdlib.h>
#include <cstring>
#include "ft602_private.h"
#include "ft60x.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <limits.h>
#include "ftd3xx.h"
#include "event_handle_async.hpp"

#include <unistd.h> // for usleep
#include <errno.h> // ETIMEDOUT
#include <sys/time.h> // gettimeofday


#define WAIT_FAILED         0xFFFFFFFF
// Special value for WaitForSingleObject dwMilliseconds parameter
#define INFINITE            0xFFFFFFFF  // Infinite timeout
#define SIGNAL_TIMEOUT 		100000

#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE    0
#endif
HANDLE hNotification;

#define FRAME_INFO_REG 0x6A
#define FRAME_REG_COUNT 6
#define I2C_ADDR 0x0D
#define NUM_CHANNELS 4
#define RES_COUNT 14


typedef struct FRAME_CFG_
{
	BYTE bStatus;
	BYTE FPS;
	WORD wWidth;
	WORD wHeight;
}FRAME_CFG, *PFRAME_CFG;

HANDLE  notification;
static VOID notificationCb(PVOID cbContext, E_FT_NOTIFICATION_CALLBACK_TYPE  cbType,PVOID cbInfo)
{
	(void)cbContext;
	(void)cbType;
	(void)cbInfo;
	(void)FT_W32_SetEvent(notification);
	printf("%s:%d: %s\n",__FILE__,__LINE__,__func__);
}

void display_usage(char* prog)
{
	printf("\nusage:\n    %s -a i2cAddr -o offset <options> \n", prog);
	printf("    %s -g \n",prog);

	printf("       : <options> \n");
	printf("       :         -r (for read) \n");
	printf("       :         -w (for write) \n");
	printf("       :         -l (size to read/write) \n");
	printf("       :         -g (for FT_GetChipConfiguration) \n");
	printf("       :         -i (for irq) \n");

}


int main(int argc, char* argv[])
{
	FT_HANDLE ftHandle;
	FT_STATUS ftStatus;
	bool bWrite = false, bGchip = false, bIrq = false;
	int i2caddr = 0, len = 2;
	int reg = 0;
	int rdMem[32];	
	int opt;
	if(argc <= 1)
	{
		display_usage(argv[0]);
		return 0;
	}

	while ((opt = getopt(argc, argv, "hrwi:g:a:o:l:i:")) != -1){

		switch (opt)
		{
			default:
			case 'h':
				display_usage(argv[0]);
				return 0;
			case 'l' : len = strtoul(optarg, NULL, 0);
				break;
			case 'a': i2caddr = strtoul(optarg, NULL, 0);
				break;
			case 'o': reg = strtoul(optarg, NULL, 0);
				break;
			case 'r': bWrite = false; 
				break;
			case 'w': bWrite = true;
				rdMem[0] = strtoul(optarg, NULL, 0);
				break;
			case 'g': bGchip = true;
				break;
			case 'i': bIrq = true;
				break;
			
			
		}
	}
			
	ftStatus = FT_Create(0, FT_OPEN_BY_INDEX, &ftHandle);
	if (FT_FAILED(ftStatus))
	{
		printf("FT_Create failed %d\n", ftStatus);
		return 0;
	}

	if(bIrq)
	{
		FT_STATUS       ftStatus = FT_OK;
		DWORD waitResult ;
   		notification = FT_W32_CreateEvent(
                             NULL,  // no security attributes
                             FALSE, // FALSE = auto-reset
                             FALSE, // FALSE = initially not signalled
                             NULL); // no name
		if(!notification)
		{
			printf("fail to create FT_W32_CreateEvent....\n");
			return 0;
		}
		printf("%s:%d: STart of FT_SetNotificationCallback\n",__FILE__,__LINE__);
		ftStatus = FT_SetNotificationCallback(ftHandle,notificationCb,NULL);
		if (FT_OK != ftStatus)
		{
			printf("%s:%d: ERROR: FT_SetNotificationCallback failed (%d)\n",__FILE__,__LINE__, ftStatus);
			return 0;
		}
		printf("%s:%d: End of FT_SetNotificationCallback\n",__FILE__,__LINE__);
		waitResult = FT_W32_WaitForSingleObject(notification,SIGNAL_TIMEOUT); 
		if (WAIT_FAILED == waitResult)
		{
			printf("%s:%d: ERROR Wait failed.\n",__FILE__,__LINE__);
			ftStatus = FT_NO_SYSTEM_RESOURCES;
			return 0;
		}
	 	printf("%s:%d:  Wait Done.\n",__FILE__,__LINE__);
	}
	else if(bGchip){
		printf("FT_GetChipConfiguration...\n");
		/*Get chip configuration*/ 
		struct _FT_602CONFIGURATION ft602 = {};
		//struct FT_60XCOMMON_CONFIGURATION common;
		memset(&ft602, 0, sizeof(ft602));
		ftStatus = FT_GetChipConfiguration(ftHandle,(void *)&ft602);
		if (FT_FAILED(ftStatus))
		{
			FT_Close(ftHandle);
			return 0;
		}
		printf("length %d\n",(uint16_t)ft602.common.length ); //1-FT600 0-245
		printf("max_power_cs %d\n",(uint8_t)ft602.common.max_power_cs );
		printf("max_power_ss %d\n",(uint8_t)ft602.common.max_power_ss );
		printf("vendor_id 0x%X\n",(uint16_t)ft602.common.vendor_id);
		printf("product_id 0x%X\n",(uint16_t)ft602.common.product_id);
		printf("version %s\n",ft602.common.version);
		FT_Close(ftHandle);
		printf("FT_GetChipConfiguration-----Done--\n");

	}		
	else
	{
		struct i2c_access i2c;

		/* initialize the i2c strutcure */
		i2c.addr = i2caddr;
		i2c.len = len;
		i2c.read_access = !bWrite;
		i2c.reg = reg;
		i2c.reg_is_16_bit = 0;

		FT_I2CAccess(ftHandle, &i2c, (uint8_t*)&rdMem[0]);
		if (!bWrite)
		{
			int i = 0;
			UCHAR *buff = (UCHAR *)&rdMem[0];
			for (i = 0; i < len; i++)
				printf("0x%x: 0x%x\n", reg+i, buff[i]);
		}
		else
		{
			printf("wrote 0x%x at 0x%x, len: %d \n", rdMem[0], reg, len );
		}
	}
	FT_Close(ftHandle);
	return 0;
}
