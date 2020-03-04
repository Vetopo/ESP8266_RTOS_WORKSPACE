#ifndef __MT_INPUT_H__
#define __MT_INPUT_H__

#include "eloop.h"

typedef void (*MT_RecCmdCb_t)(uint8 *cmd0, uint8 cmd1, uint8 *pdata, uint8 len);

typedef struct 
{
    uint8 cmd0;
    uint8 cmd1;
    MT_RecCmdCb_t cb;
}MT_ReceiveCommand_t;

void MT_InputMessageCB(uint8 cmd0, uint8 cmd1, uint8 *pdata, uint8 len);

#endif

