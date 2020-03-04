/*
 * ATC_Uart.h
 *
 *  Created on: Feb 26, 2019
 *      Author: lort
 */

#ifndef __ATC_UART_H_
#define __ATC_UART_H_

#include "ATC_Config.h"

typedef enum {
  ATC_RPC_CMD_POLL = 0x00,
  ATC_RPC_CMD_SREQ = 0x20,
  ATC_RPC_CMD_AREQ = 0x40,
  ATC_RPC_CMD_SRSP = 0x60,
} mt_cmd0_h;

typedef enum {
  ATC_RPC_SYS_RES0,   /* Reserved. */
  ATC_RPC_SYS_SYS,
  ATC_RPC_SYS_MAC,
  ATC_RPC_SYS_NWK,
  ATC_RPC_SYS_APS,
  ATC_RPC_SYS_PDO,
  ATC_RPC_SYS_SAPI,   /* Simple API. */
  ATC_RPC_SYS_UTIL,
  ATC_RPC_SYS_DBG,
  ATC_RPC_SYS_APP,
  ATC_RPC_SYS_OTA,
  ATC_RPC_SYS_MAX     /* Maximum value, must be last */
  /* 10-32 available, not yet assigned. */
}mt_cmd0_l;

typedef enum
{
	ATC_SYS_TEST =1,
	ATC_SYS_RESET,//2
	ATC_SYS_READ_INFO,//3
	ATC_SYS_READ_SN,//4
	ATC_SYS_WRITE_SN,//5
  ATC_SYS_TEST_LQI,//6
  ATC_SYS_TEST_GPIO,//7
  ATC_SYS_READ_HARDWARE_INFO,//8
  ATC_SYS_UPDATE_FIRMWARE//9
}mt_sys_cmd_t;

typedef struct
{
	uint8 SOF;
	uint8 LEN[2];
	uint8 CMD0;
	uint8 CMD1;
	uint8 DATA[0];
}ATC_PKG_t;

uint8   ATC_UartInit(void);
void    ATC_UartPoll(void);
uint8   ATC_Uart_Write(uint8 cmd0, uint8 cmd1, uint8 *pdata, int len);

int 	  ATC_PackageLength(ATC_PKG_t *pkg);
int     ATC_PackageDateLength(ATC_PKG_t *pkg);
uint8 	ATC_PackageGetXor(uint8 *pbuf, int len);
int     ATC_PackageFindHead(uint8 *s, uint16 len, uint16 index);
int     ATC_CheckPackage(ATC_PKG_t *pkg, int len);
ATC_PKG_t  *ATC_PackageMessage(uint8 cmd0, uint8 cmd1, uint8 *pdata, int len);

#endif /* APP_GWZB_ESP32_V2_0_HAL_DRIVER_HAL_FACTORY_H_ */
