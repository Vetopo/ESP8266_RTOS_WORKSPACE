#ifndef __MT_OUTPUT_H__
#define __MT_OUTPUT_H__

#define MT_OUTPUT_MAX_LIST          (100)
#define MT_OUTPUT_TIMEOUT           (1000)
#define MT_OUTPUT_TICK              (50)

uint8 MT_Output_Poll(void);
uint8 MT_ReqSend(uint8 stry,uint16 addr,uint8 cmd0, uint8 cmd1, uint8 *pdata, uint8 len);
uint8 MT_RemoveSendMessage(uint16 addr,uint8 seq);

#endif