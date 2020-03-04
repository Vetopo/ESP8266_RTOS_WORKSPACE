/*
 * hal_factory.c
 *
 *  Created on: Feb 26, 2019
 *      Author: lort
 */

#include "eloop.h"
#include "hal_uart.h"
#include "ATC_Task.h"
#include "ATC_Uart.h"

#define DBG_ATC_UART						0

#define HAL_UART_SOF_FLAG				0xFE
#define HAL_UART_SOF_FLAG1				0xFF
#define HAL_UART_RX_BUFFER_SIZE			(1024)
#define HAL_UART_MIN_PACKAGE_SIZE		(6)

#define ATC_UARTNUM							UART_NUM_0
#define UART_BUFFER_SIZE				(1024)

typedef struct
{
	uint16 	timeout;
	uint16 	cnt;
	uint8	data[HAL_UART_RX_BUFFER_SIZE];
}uart_rx_t;

static uart_rx_t				uart_rx;

static int 						read_package(uint8 *pbuf, int len);

uint8 ATC_UartInit(void)
{
	eloop_log(DBG_ATC_UART,"ATC_UartInit\r\n");
	hal_uart_init(ATC_UARTNUM);
	return ES_SUCCEED;
}
static uint8 uart_rx_buf[1024];
void ATC_UartPoll(void)
{
	ATC_PKG_t 	*pkg;
	int 	slen,dlen;
	uint8 	buf[128];
	eloop_log(0,"ATC_UartPoll\r\n");
	if((slen = hal_uart_read(ATC_UARTNUM, buf, 128))>0)
	{
		if((uart_rx.cnt+slen)<UART_BUFFER_SIZE)
		{
			eloop_memcpy(&uart_rx.data[uart_rx.cnt],buf,slen);
			uart_rx.cnt += slen;
			uart_rx.timeout = 10;
			eloop_logb(0,"ATC_UartPoll uart:receive:",uart_rx.data,uart_rx.cnt);
		}
		else
		{
			uart_rx.cnt = 0;
		}
		eloop_logb(0,"ATC_UartPoll uart:receive:",uart_rx.data,uart_rx.cnt);
	}
	pkg = (ATC_PKG_t*)&uart_rx_buf;//eloop_malloc(HAL_UART_RX_BUFFER_SIZE);
	if(pkg!=NULL)
	{
		eloop_log(0,"zb_uart_task:cmd0: try to read ATC_PackageMessage now\r\n");
		if((slen = read_package((uint8*)pkg,HAL_UART_RX_BUFFER_SIZE))>0)
		{
			dlen = ATC_PackageDateLength(pkg);
			eloop_log(DBG_ATC_UART,"zb_uart_thread read ATC_PackageMessage :cmd0:%02x cmd1:%02x ",pkg->CMD0,pkg->CMD1);
			eloop_logb(DBG_ATC_UART,"data:",pkg->DATA,dlen);
			ATC_ReceiveMessageCB(ATC_MSG_TYPE_UART,pkg->CMD0,pkg->CMD1,pkg->DATA,dlen);
		}
	}
}
uint8  ATC_Uart_Write(uint8 cmd0, uint8 cmd1, uint8 *pdata, int len)
{
	ATC_PKG_t 	*pkg;
	uint8			ret = ES_FAILED;
	int				slen;
	eloop_log(1,"zb_uart_write:cmd0:%02x cmd1:%02x len:%d \r\n",cmd0,cmd1,len);
	eloop_logb(DBG_ATC_UART,"data:",pdata,len);
	pkg = ATC_PackageMessage(cmd0,cmd1,pdata,len);
	if(pkg!=NULL)
	{
		slen = ATC_PackageLength(pkg);
		eloop_logb(DBG_ATC_UART,"zb_uart_write:",(uint8*)pkg,slen);
		if((hal_uart_write(ATC_UARTNUM,(uint8*)pkg,slen))==0)
		{
			ret = ES_SUCCEED;
		}
		eloop_free(pkg);
	}
	return ret;
}
int  ATC_PackageFindHead(uint8 *s, uint16 len, uint16 index)
{
	uint16 i;
	uint16 cnt=0;
	for(i=0;i<len;i++)
	{
		if((s[i]==HAL_UART_SOF_FLAG1)||(s[i]==HAL_UART_SOF_FLAG))
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
static int  read_package(uint8 *pbuf, int len)
{
	int 				headID = -1;
	int					slen;
	uint8				itry;
	//osEntercritical();
	if(uart_rx.timeout)
	{
		uart_rx.timeout--;
	}
	else if(uart_rx.cnt)
	{
		eloop_logb(DBG_ATC_UART, "message timeout:",uart_rx.data,uart_rx.cnt);
		uart_rx.cnt = 0;
	}
	slen = 0;
	if(uart_rx.cnt>0)
	{
		itry=3;
		while(itry--)
		{
			headID = ATC_PackageFindHead(uart_rx.data,uart_rx.cnt,0);//find first head index
			if(headID>0)
			{
				uart_rx.cnt -= headID;
				eloop_memcpy(uart_rx.data,&uart_rx.data[headID],uart_rx.cnt);
			}
			else
				break;
		}
		if(headID==0)
		{
			eloop_log(DBG_ATC_UART,"read_package find head id:%d\r\n",headID);
			eloop_logb(DBG_ATC_UART,"data:",uart_rx.data,uart_rx.cnt);
			if((slen = ATC_CheckPackage((void*)uart_rx.data,uart_rx.cnt))>0)
			{
				eloop_logb(DBG_ATC_UART,"read_package:",uart_rx.data,slen);
				slen = (slen>len)?len:slen;
				eloop_memcpy(pbuf,uart_rx.data,slen);
				uart_rx.cnt-=slen;
				eloop_memcpy(uart_rx.data,&uart_rx.data[slen],uart_rx.cnt);
				eloop_log(DBG_ATC_UART,"read_package ATC_CheckPackage OK: slen:%d len:%d\r\n",slen,len);
			}
			else
			{
				eloop_logb(DBG_ATC_UART,"unhandler data wait for next:",uart_rx.data,uart_rx.cnt);
			}
		}
	}
	//osExitcritical();
	return slen;
}
int  ATC_CheckPackage(ATC_PKG_t *pkg, int len)
{
	int slen = 0,dlen;
	uint8 sum;
	eloop_logb(0,"ATC_CheckPackage: ",(void*)pkg,len);
	if(len>=HAL_UART_MIN_PACKAGE_SIZE)
	{
		slen = ATC_PackageLength(pkg);
		if(slen<=len)
		{
			eloop_log(DBG_ATC_UART,"ATC_CheckPackage: slen:%d len:%d \r\n",slen,len);
			if(pkg->SOF==HAL_UART_SOF_FLAG)
			{
				dlen = ATC_PackageDateLength(pkg);
				sum = ATC_PackageGetXor(&pkg->LEN[0],(slen-2));
				if(sum==pkg->DATA[dlen])
					return (int)slen;
				else
				{
					eloop_log(0,"check pacakge: sum error msg sum:%02x cal:%02x \r\n",pkg->DATA[dlen],sum);
				}
			}
			else if(pkg->SOF==HAL_UART_SOF_FLAG1)
			{
				return (int)slen;
			}
		}
		else
		{
			eloop_log(0,"pacakge not complite:slen %d != %d \r\n",slen,len);
		}
	}
	else
	{
		eloop_log(DBG_ATC_UART,"pacakge less 6\r\n");
	}
	
	return 0;
}

ATC_PKG_t  *ATC_PackageMessage(uint8 cmd0, uint8 cmd1, uint8 *pdata, int len)
{
	ATC_PKG_t 	*pkg;
	int 	slen;
	slen = len + HAL_UART_MIN_PACKAGE_SIZE;
	pkg = (ATC_PKG_t*)eloop_malloc(slen);
	if(pkg!=NULL)
	{
		pkg->SOF = HAL_UART_SOF_FLAG;
		pkg->CMD0 = cmd0;
		pkg->CMD1 = cmd1;
		pkg->LEN[0] = (uint8)(len>>8);
		pkg->LEN[1] = (uint8)len;
		if((pdata!=NULL)&&(len!=0))
			eloop_memcpy(pkg->DATA,pdata,len);
		pkg->DATA[len] = ATC_PackageGetXor(&pkg->LEN[0],(slen-2));
		return pkg;
	}
	return NULL;
}
int  ATC_PackageLength(ATC_PKG_t *pkg)
{
	int 	len;
	uint16 	temp;
	temp = pkg->LEN[0];
	temp <<= 8;
	temp |= pkg->LEN[1];
	len = temp+HAL_UART_MIN_PACKAGE_SIZE;
	return len;
}
int  ATC_PackageDateLength(ATC_PKG_t *pkg)
{
	uint16 	temp;
	temp = pkg->LEN[0];
	temp <<= 8;
	temp |= pkg->LEN[1];
	return (int)temp;
}
uint8  ATC_PackageGetXor(uint8 *pbuf, int len)
{
	int i;
	uint8 sum;
	sum = 0;
	for(i=0;i<len;i++)
	{
		sum ^= pbuf[i];
	}
	return sum;
}