#include "eloop.h"
#include "MT_Conf.h"
#include "MT_Task.h"
#include "MT_Uart.h"
#include "MT_Output.h"
#include "MT_Input.h"

uint8 MT_TaskInit(void)
{
    MT_UartInit(MT_ZB_UART,MT_InputMessageCB);
    eloop_create_task(MT_Task,1);
    eloop_start_timer_task(MT_Task,MT_TASK_EVENT_UART_POLL,20,ES_TRUE);
    return ES_SUCCEED;
}
void MT_Task(uint32 event, void *arg, uint32 len)
{
    if(event&MT_TASK_EVENT_OUTPUT)
    {
        MT_Output_Poll();
    }
    if(event&MT_TASK_EVENT_UART_POLL)
    {
        MT_Uart_Poll();
    }
}
uint8 MT_GetSeq(void)
{
    static uint8 seq = 0;
    return seq++;
}