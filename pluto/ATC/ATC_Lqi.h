#ifndef __ATC_LQI_H__
#define __ATC_LQI_H__

#include "eloop.h"
#include "ATC_Config.h"

uint8 ATC_Lqi_Init(void);
void ATC_Lqi_StartConnectWifi(void);
void ATC_LqiRecieveLqiProcess(uint8 cmd0,uint8 cmd1,uint8 *pdata,int len);
void ATC_Lqi_ReqSendSelfRssi(void);
void ATC_Lqi_ResponseSendSelfRssi(void);
#endif

