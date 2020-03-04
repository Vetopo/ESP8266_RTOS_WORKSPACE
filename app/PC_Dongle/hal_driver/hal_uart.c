/*
 * hal_uart.c
 *
 *  Created on: Feb 26, 2019
 *      Author: lort
 */
#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/errno.h>
#include <sys/unistd.h>
#include <sys/select.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_vfs_dev.h"
#include "pthread.h"
#include "driver/uart.h"
#include "eloop.h"
#include "hal_uart.h"

#define UART_BUFFER_SIZE				(1024)

typedef struct
{
	void		*next;
	uint8		init;
	uint8		uart;
	uint8 		len;
	uint8		pdata[UART_BUFFER_SIZE];
}_UART_T;

static _UART_T					*pUart = NULL;
static SemaphoreHandle_t		_Uart_Mutex;
static void*					_uart_thread(void *arg);
static _UART_T*					_find_uart(uart_port_t uart);
static _UART_T*					_find_uart(uart_port_t uart);
static void 					_enter_critical(void);
static void 					_exit_critical(void);
uint8 hal_uart_init(uart_port_t uart)
{
	pthread_t id;
	uint8 ret = ES_FAILED;
	_UART_T *pnew;
	eloop_log(0,"hal_uart_init:\r\n");
	if(pUart==NULL)
	{
		_Uart_Mutex = xSemaphoreCreateMutex();
		ret = pthread_create(&id,NULL,_uart_thread,NULL);
		//xTaskCreate(_uart_thread, "factory_thread", 1024*4, NULL, (tskIDLE_PRIORITY + 5), NULL);
	}
	pnew = _find_uart(uart);
	if(pnew==NULL)
	{
		pnew = (_UART_T*)eloop_malloc(sizeof(_UART_T));
		if(pnew!=NULL)
		{
			eloop_memset(pnew,0x00,sizeof(_UART_T));
			pnew->uart = uart;
			pnew->init = ES_FALSE;
			_enter_critical();
			eloop_list_insert((void**)&pUart,pnew);
			_exit_critical();
			ret = ES_SUCCEED;
		}
	}
	else
	{
		ret = ES_SUCCEED;
	}
	return ret;
}
uint8  hal_uart_write(uart_port_t uart, uint8 *pdata, uint16 len)
{
	_UART_T *pcur;
	uint8			ret = ES_FAILED;
	eloop_logb(1,"data:",pdata,len);
	_enter_critical();
	pcur = _find_uart(uart);
	if((pcur!=NULL)&&(pcur->init==ES_TRUE))
	{
		if((uart_write_bytes(pcur->uart,(const char *)pdata,len))==0)
		{
			ret = ES_SUCCEED;
		}
	}
	_exit_critical();
	return ret;
}
uint8 hal_uart_read(uart_port_t uart, uint8 *pdata, uint8 len)
{
	_UART_T *pcur;
	uint8 slen = 0;
	eloop_log(0,"hal_uart_read uart:%d \r\n",uart);
	_enter_critical();
	pcur = _find_uart(uart);
	if((pcur!=NULL)&&(pcur->len>0))
	{
		slen = (pcur->len>len)?len:pcur->len;
		eloop_memcpy(pdata,pcur->pdata,slen);
		pcur->len-=slen;
		eloop_memcpy(pcur->pdata,&pcur->pdata[slen],pcur->len);
	}
	_exit_critical();
	return slen;
}
static void _enter_critical(void)
{
	if(pUart!=NULL)
	{
		xSemaphoreTake(_Uart_Mutex, (portTickType)portMAX_DELAY);
	}
}
static void _exit_critical(void)
{
	if(pUart!=NULL)
	{
		xSemaphoreGive(_Uart_Mutex);
	}
}
static void _init_uart(_UART_T *puart)
{
	// Configure parameters of an UART driver,
    // communication pins and install the driver
    uart_config_t uart_config = {
        .baud_rate = 921600,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
	if(puart->init==ES_FALSE)
	{
		uart_param_config(puart->uart, &uart_config);
		uart_driver_install(puart->uart, UART_BUFFER_SIZE, 0, 0, NULL, 0);
		puart->init = ES_TRUE;
	}
}
static _UART_T *_find_uart(uart_port_t uart)
{
	_UART_T *pcur;
	pcur = pUart;
	while(pcur!=NULL)
	{
		if(pcur->uart==uart)
		{
			return pcur;
		}
		pcur = pcur->next;
	}
	return NULL;
}
static void *_uart_thread(void *arg)
{
	_UART_T *pcur;
	uint16 	slen;
	uint16	rlen;
	uint8 	buf[128];
	while(1)
	{
		pcur = pUart;
		while(pcur!=NULL)
		{
			_init_uart(pcur);
			eloop_log(0,"_uart_thread:%d:\r\n",pcur->uart);
			if((slen = uart_read_bytes(pcur->uart, buf, sizeof(buf),1))>0)
			{
				eloop_log(0,"_uart_thread:%d: slen:%d, dlen:%d\r\n",pcur->uart,slen,pcur->len);
				_enter_critical();
				if((pcur->len+slen)>UART_BUFFER_SIZE)
				{
					rlen = (pcur->len + slen) - UART_BUFFER_SIZE;
					pcur->len -= rlen;
					eloop_log(0,"_uart_thread:%d: rlen:%d, dlen:%d\r\n",pcur->uart,rlen,pcur->len);
					eloop_memcpy(pcur->pdata,&pcur->pdata[rlen],pcur->len);
				}
				eloop_memcpy(&pcur->pdata[pcur->len],buf,slen);
				pcur->len += slen;
				_exit_critical();
				eloop_log(0,"uart:%d:",pcur->uart);
				eloop_logb(0,"receive:",pcur->pdata,pcur->len);
			}
			eloop_log(0,"_uart_thread:%d:\r\n",pcur->uart);
			pcur = pcur->next;
		}
		eloop_sleep(20);
	}
	return NULL;
}


