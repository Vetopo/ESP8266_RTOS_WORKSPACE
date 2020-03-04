/*
 * ATC_Task.h
 *
 *  Created on: 2017��1��12��
 *      Author: lort
 */

#ifndef __ATC_TASK_H__
#define __ATC_TASK_H__

#include "eloop.h"
#include "ATC_Config.h"

#define     DBG_FACTORY	        3

typedef enum{
    ATC_TASK_UART_POLL_EVENT = 1,
}_ATC_TASK_EVENT;

uint8 ATC_TaskInit(uint8 dev_type);
void ATC_ReqSendLqi(uint8 *rec_mac, uint8 rec_rssi);
void  ATC_ReceiveMessageCB(uint8 type,uint8 cmd0, uint8 cmd1, uint8 *pdata, int len);

#endif /* USER_FACTORY_SETTINGS_H_ */
