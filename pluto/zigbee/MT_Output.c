#include "eloop.h"
#include "MT_Conf.h"
#include "MT_Task.h"
#include "MT_Output.h"
#include "MT_Uart.h"

typedef struct
{
	void 	*next;
    uint8   stry;
    uint8   cmd0;
    uint8   cmd1;
    uint8   seq;
    uint16  addr;
    uint8   len;
	uint8   pdata[0];
}_MT_SEND_T;

static _MT_SEND_T*  pSendList = NULL;
static uint16       SendRuntime = 0;
static uint16       ListCount = 0;

static void _SetOutputEvent(uint32 delay);

uint8 MT_Output_Poll(void)
{
    _MT_SEND_T   *pcur; 
    if(SendRuntime)
    {
        SendRuntime--;
    }
    if(SendRuntime==0)
    {
        pcur=pSendList;
        if(pcur!=NULL)
        {
            pcur->stry--;
            MT_UartWrite(MT_ZB_UART,pcur->cmd0,pcur->cmd1,pcur->pdata,pcur->len);
            if(pcur->stry==0)
            {
                eloop_list_delete((void**)&pSendList,pcur);
                if(ListCount)
                {
                    ListCount--;
                }
                _SetOutputEvent(MT_OUTPUT_TICK);
            }
            else
            {
                _SetOutputEvent(MT_OUTPUT_TIMEOUT);
            }
        }
    }
    return ES_SUCCEED;
}
uint8 MT_ReqSend(uint8 stry,uint16 addr, uint8 cmd0, uint8 cmd1, uint8 *pdata, uint8 len)
{
    uint8 ret = ES_FAILED;
    _MT_SEND_T   *pnew; 
    pnew = (_MT_SEND_T *)eloop_malloc(sizeof(_MT_SEND_T)+len);
    if(pnew!=NULL)
    {
        pnew->next = NULL;
        pnew->stry = stry;
        pnew->addr = addr;
        pnew->cmd0 = cmd0;
        pnew->cmd1 = cmd1;
        pnew->len = len;
        pnew->seq = MT_GetSeq();
        eloop_memcpy(pnew->pdata,pdata,len);
        if(ListCount==0)
        {
            _SetOutputEvent(0);
        }
        else if((ListCount>=MT_OUTPUT_MAX_LIST)&&(pSendList->next!=NULL))
        {
            eloop_list_delete((void**)&pSendList,pSendList->next);
            ListCount--;
        }
        eloop_list_insert((void**)&pSendList,pnew);
        ListCount++;
        ret = ES_SUCCEED;
    }
    return ret;
}
uint8 MT_RemoveSendMessage(uint16 addr, uint8 seq)
{
    _MT_SEND_T   *pcur; 
    uint8 ret = ES_FAILED;
    pcur = pSendList;
    while(pcur!=NULL)
    {
        if((pcur->addr==addr)&&(pcur->seq==seq))
        {
            eloop_list_delete((void**)&pSendList,pcur);
            if(ListCount)
            {
                ListCount--;
            }
            ret = ES_SUCCEED;
            break;
        }
        pcur = pcur->next;
    }
    return ret;
}
static void _SetOutputEvent(uint32 delay)
{
    SendRuntime = delay/MT_OUTPUT_TICK;
    if(delay==0)
    {
        eloop_set_event(MT_Task,MT_TASK_EVENT_OUTPUT);
    }
    else
    {
        eloop_start_timer_task(MT_Task,MT_TASK_EVENT_OUTPUT,delay,ES_FALSE);
    }
}