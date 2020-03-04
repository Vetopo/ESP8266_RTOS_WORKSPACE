#ifndef __MT_TASK_H__
#define __MT_TASK_H__

typedef enum
{
    MT_TASK_EVENT_OUTPUT     = 1,
    MT_TASK_EVENT_UART_POLL  = 2, 
}MT_TASK_EVENTS;

uint8   MT_TaskInit(void);
void    MT_Task(uint32 event, void *arg, uint32 len);
uint8   MT_GetSeq(void);

#endif


