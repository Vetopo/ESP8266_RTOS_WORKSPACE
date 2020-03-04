/*
 * MT_Uart.c
 *
 *  Created on: Jun 11, 2019
 *      Author: lort
 */
#include "eloop.h"
#include "MT_Conf.h"
#include "zigbee_conf.h"
#include "MT_Uart.h"
#include "hal_uart.h"

#define ZB_SOF_FLAG				0xFE
#define ZB_SOF_FLAG1			0xFF
#define ZB_RX_BUFFER_SIZE		(511)
#define ZB_MIN_PACKAGE_SIZE		(sizeof(MT_Package_t)+1)

typedef struct
{
	uint8 SOF;
	uint8 LEN;
	uint8 CMD0;
	uint8 CMD1;
	uint8 DATA[0];
}MT_Package_t;

typedef struct
{
	void				*next;
	MT_UartReceiveCB_t	cb;
	uint8 				uart;
	uint8				lock_poll;
	uint16 				timeout;
	uint16 				cnt;
	uint8	data[ZB_RX_BUFFER_SIZE];
}_MT_Uart_t;

static _MT_Uart_t				*pUart = NULL;

static _MT_Uart_t*				_find_uart(uint8 uart);
static int 						_find_head(uint8 *s, uint16 len, uint16 index);
static uint8 					_check_package(MT_Package_t *pkg, uint16 len);
static uint8 					_read_package(_MT_Uart_t *puart,uint8 *pbuf, uint8 len);

static MT_Package_t 			*_package(uint8 cmd0, uint8 cmd1, uint8 *pdata, uint8 len);
static uint8 					_package_length(MT_Package_t *pkg);

uint8 MT_UartInit(uint8 uart, MT_UartReceiveCB_t cb)
{
	_MT_Uart_t *pnew;
	uint8 ret = ES_FAILED;
	pnew =_find_uart(uart);
	if(pnew==NULL)
	{
		pnew = (_MT_Uart_t*)eloop_malloc(sizeof(_MT_Uart_t));
		if(pnew!=NULL)
		{
			eloop_memset(pnew,0,sizeof(_MT_Uart_t));
			pnew->uart = uart;
			pnew->cb = cb;
			pnew->lock_poll = ES_FALSE;
			eloop_list_insert((void**)&pUart,pnew);
			ret = hal_uart_init(uart);
		}
	}
	else
	{
		ret = ES_SUCCEED;
	}
	return ret;
}

uint8 MT_UartWrite(uint8 uart, uint8 cmd0, uint8 cmd1, uint8 *pdata, uint8 len)
{
	MT_Package_t 		*pkg;
	uint8				ret = ES_FAILED;
	uint8				slen;
	eloop_log(DBG_MT_UART,"zb_uart_write:cmd0:%02x cmd1:%02x ",cmd0,cmd1);
	eloop_logb(DBG_MT_UART,"data:",pdata,len);
	pkg = _package(cmd0,cmd1,pdata,len);
	if(pkg!=NULL)
	{
		slen = _package_length(pkg);
		eloop_logb(DBG_MT_COMMON,"zb_uart_write:",(uint8*)pkg,slen);
		if((hal_uart_write(uart,(uint8 *)pkg,slen))>0)
		{
			ret = ES_SUCCEED;
		}
		eloop_free(pkg);
	}
	return ret;
}
uint8 MT_UartRead(uint8 uart, uint8 cmd0, uint8 cmd1, uint8 *pdata, uint8 len)
{
	_MT_Uart_t 			*pcur;
	uint8 				buf[255];
	MT_Package_t 		*pkg = (MT_Package_t*)buf;
	uint8				slen = 0;
	uint8 				stry = 60;
	eloop_log(1,"zb_uart_read at :cmd0:%02x cmd1:%02x \r\n",cmd0,cmd1);
	pcur = _find_uart(uart);
	if(pcur!=NULL)
	{
		pcur->lock_poll = ES_TRUE;
		while(stry>0)
		{
			if((slen = _read_package(pcur,buf,255))>0)
			{
				if((pkg->CMD0==cmd0)&&(pkg->CMD1==cmd1))
				{
					if(len<pkg->LEN)
						slen = len;
					else
						slen = pkg->LEN;
					eloop_memcpy(pdata,pkg->DATA,slen);
					break;
				}
				else
				{
					if(pcur->cb!=NULL)
					{
						pcur->cb(pkg->CMD0,pkg->CMD1,pkg->DATA,pkg->LEN);
					}
				}
			}
			else
			{
				eloop_sleep(10);
			}
			stry--;
		}
		pcur->lock_poll = ES_FALSE;
	}
	return slen;
}
void MT_Uart_Poll(void)
{
	_MT_Uart_t 			*pcur;
	uint8 				buf[255];
	MT_Package_t 		*pkg = (MT_Package_t*)buf;
	uint8				slen;
	pcur = pUart;
	while(pcur!=NULL)
	{
		if(pcur->lock_poll==ES_FALSE)
		{
			eloop_log(0,"mt_uart_task:uart:%d try to read _package now\r\n",pcur->uart);
			if((slen = _read_package(pcur,buf,255))>0)
			{
				eloop_log(DBG_MT_UART,"zb_uart_thread read _package :cmd0:%02x cmd1:%02x ",pkg->CMD0,pkg->CMD1);
				eloop_logb(DBG_MT_UART,"data:",pkg->DATA,pkg->LEN);
				if(pcur->cb!=NULL)
				{
					pcur->cb(pkg->CMD0,pkg->CMD1,pkg->DATA,pkg->LEN);
				}
			}
		}
		pcur = pcur->next;
	}
}
static _MT_Uart_t *_find_uart(uint8 uart)
{
	_MT_Uart_t *pcur;
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
static int _find_head(uint8 *s, uint16 len, uint16 index)
{
	uint16 i;
	uint16 cnt=0;
	for(i=0;i<len;i++)
	{
		if((s[i]==ZB_SOF_FLAG1)||(s[i]==ZB_SOF_FLAG))
		{
			if(cnt>=index)
			{
				return i;
			}
			cnt++;
		}
	}
	return -1;
}
static uint8 _read_package(_MT_Uart_t *puart, uint8 *pbuf, uint8 len)
{
	MT_Package_t		*pkg = (MT_Package_t*)pbuf;
	int 				headID = -1;
	uint16				slen;
	uint16 				rlen;
	uint8				itry;
	uint8				buf[128];
	//DBG_vPrintf(TRUE, "hal_uartRead4: \r\n ");
	if((slen = hal_uart_read(puart->uart,buf,128))>0)
	{
		//osEntercritical();
		if(ZB_RX_BUFFER_SIZE<(puart->cnt+slen))
			puart->cnt = 0;
		rlen = ZB_RX_BUFFER_SIZE - puart->cnt;//ʣ��ռ�
		if(rlen>0)
		{
			if(rlen>=ZB_RX_BUFFER_SIZE)//error occur
			{
				rlen = ZB_RX_BUFFER_SIZE;
				puart->cnt = 0;
			}
			eloop_memcpy(&puart->data[puart->cnt],buf,slen);
			puart->cnt += slen;
			puart->timeout = 10;
		}
		//osExitcritical();
		eloop_logb(3,"_read_package:",puart->data,puart->cnt);
	}
	//osEntercritical();
	if(puart->timeout)
	{
		puart->timeout--;
	}
	else if(puart->cnt)
	{
		eloop_logb(DBG_MT_ERROR, "_read_package: message timeout:",puart->data,puart->cnt);
		puart->cnt = 0;
	}
	slen = 0;
	if(puart->cnt>0)
	{
		itry=3;
		while(itry--)
		{
			headID = _find_head(puart->data,puart->cnt,0);//find first head index
			if(headID>0)
			{
				puart->cnt -= headID;
				eloop_memcpy(puart->data,&puart->data[headID],puart->cnt);
			}
			else
				break;
		}
		if(headID==0)
		{
			if((slen = _check_package((void*)puart->data,puart->cnt))>0)
			{
				eloop_logb(DBG_MT_COMMON,"_read_package:",puart->data,slen);
				slen = (slen>len)?len:slen;
				eloop_memcpy(pkg,puart->data,slen);
				puart->cnt-=slen;
				eloop_memcpy(puart->data,&puart->data[slen],puart->cnt);
			}
		}
	}
	//osExitcritical();
	return slen;
}
static uint8 _check_package(MT_Package_t *pkg, uint16 len)
{
	uint16 slen = 0;
	uint8 sum;
	eloop_logb(0,"_check_package: ",(void*)pkg,len);
	if(len>ZB_MIN_PACKAGE_SIZE)
	{
		slen = _package_length(pkg);
		if(slen<=len)
		{
			eloop_log(3,"_check_package: slen:%d len:%d \r\n",slen,len);
			if(pkg->SOF==ZB_SOF_FLAG)
			{
				sum = eloop_get_xor(&pkg->LEN,(slen-2));
				if(sum==pkg->DATA[pkg->LEN])
					return (uint8)slen;
				else
					eloop_log(DBG_MT_ERROR,"check pacakge: sum error msg sum:%02x cal:%02x \r\n",pkg->DATA[pkg->LEN],sum);
			}
			else if(pkg->SOF==ZB_SOF_FLAG1)
			{
				return (uint8)slen;
			}
		}

	}
	return 0;
}

static MT_Package_t *_package(uint8 cmd0, uint8 cmd1, uint8 *pdata, uint8 len)
{
	MT_Package_t 	*pkg;
	uint16 	slen;
	slen = len + ZB_MIN_PACKAGE_SIZE;
	pkg = (MT_Package_t*)eloop_malloc(slen);
	if(pkg!=NULL)
	{
		pkg->SOF = ZB_SOF_FLAG;
		pkg->CMD0 = cmd0;
		pkg->CMD1 = cmd1;
		pkg->LEN = len;
		if((pdata!=NULL)&&(len!=0x00))
			eloop_memcpy(pkg->DATA,pdata,len);
		pkg->DATA[len] = eloop_get_xor(&pkg->LEN,(slen-2));
		return pkg;
	}
	return NULL;
}
static uint8 _package_length(MT_Package_t *pkg)
{
	uint8 len;
	len = pkg->LEN+ZB_MIN_PACKAGE_SIZE;
	return len;
}

