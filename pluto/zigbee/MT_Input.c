#include "eloop.h"
#include "MT_Input.h"

static const MT_ReceiveCommand_t MT_ReceiveCommands[]={

};

void MT_InputMessageCB(uint8 cmd0, uint8 cmd1, uint8 *pdata, uint8 len)
{
    eloop_logb(3,"MT_InputMessageCB",pdata,len);
}