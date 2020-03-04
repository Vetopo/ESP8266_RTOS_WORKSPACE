/*
 * atc_socket.c
 *
 *  Created on: Jul 22, 2016
 *      Author: lort
 */

#include "eloop.h"
#include "ATC_Config.h"
#include "ATC_UDP.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include <netdb.h>
#include <sys/socket.h>
#include "ATC_Uart.h"
#include "ATC_Task.h"
#include "esp_pthread.h"
#include <time.h>
#include <errno.h>
#include <pthread.h>
#define ATC_UDP_BUFFER_SIZE			1500
static int 		atc_socket_fd = -1;
static struct 	sockaddr_in atc_socket;
static uint8 	*atc_buffer = NULL;//receive buffer

static void  atc_create_socket(void);
static void atc_message_process(uint32 ip, uint16 port, uint8 *pdata,int len);
void *atc_socket_thread_rec(void *arg);
struct atc_send_nwk_t
{
	uint32	srcIP;
	uint16	srcPort;
	int		len;
	uint8 	pdata[0];
};
int  atc_socket_init(uint16 port)
{
	pthread_t id;
	eloop_log(1,"atc_socket_init: port:%d \r\n",pluto_adapter_ntohs(port));
	eloop_memset(&atc_socket,0,sizeof(struct sockaddr_in));
	atc_socket.sin_family = AF_INET;
	atc_socket.sin_addr.s_addr = INADDR_ANY;
	atc_socket.sin_port = port;
	//atc_socket.sin_len = sizeof(atc_socket);
	if(atc_buffer==NULL)
	{
		atc_buffer = (uint8*)eloop_malloc(ATC_UDP_BUFFER_SIZE);
		pthread_create(&id,NULL,atc_socket_thread_rec,NULL);
	}
	atc_create_socket();
	return 0;
}
int atc_socket_deinit(void)
{
	if(atc_socket_fd>0)
	{
		shutdown(atc_socket_fd,SHUT_RDWR);
		close(atc_socket_fd);
		atc_socket_fd = -1;
	}
	if(atc_buffer!=NULL)
	{
		eloop_free(atc_buffer);
		atc_buffer = NULL;
	}
	return ES_SUCCEED;
}
static void  atc_create_socket(void)
{
	int ret;
	int so_br = 1;
	eloop_log(DBG_ATC_SOCKET,"atc_socket_create_socket :got IP \r\n");
	if(atc_socket_fd<0)
	{
		atc_socket_fd = socket(AF_INET,SOCK_DGRAM,0);
		if(atc_socket_fd<0)
		{
			eloop_log(DBG_ATC_SOCKET,"atc_socket_create_socket creat socket_fd error \r\n");
			pluto_adapter_restart(3000);
			return;
		}
		else
			setsockopt(atc_socket_fd,SOL_SOCKET,SO_BROADCAST,&so_br,sizeof(so_br));
		eloop_log(DBG_ATC_SOCKET,"atc_socket_create_socket :create socket fd \r\n");
		ret = bind(atc_socket_fd,(struct sockaddr *)&(atc_socket),sizeof(atc_socket));
		if(ret==-1)
		{
			eloop_log(DBG_ATC_SOCKET,"atc_socket_create_socket bind socket_fd error \r\n");
			atc_socket_deinit();
		}
	}
	eloop_log(DBG_ATC_SOCKET,"atc_socket_create_socket :binded socket finished\r\n");
}
void  *atc_socket_thread_rec(void *arg)
{
	struct sockaddr_in fromSocket;
	socklen_t 	fromSlen = sizeof(fromSocket);
	uint32 ip;
	char *pip;
	uint16 port;  
	int ret;
	while(1)
	{
		if((atc_socket_fd>=0)&&(atc_buffer!=NULL))
		{
			ret = recvfrom(atc_socket_fd,atc_buffer,ATC_UDP_BUFFER_SIZE,0,(struct sockaddr *)&fromSocket,&fromSlen);
			if(ret>0)
			{
				port = (uint16)fromSocket.sin_port;//ntohs(fromSocket.sin_port);
				ip = (uint32)fromSocket.sin_addr.s_addr;//ntohl(fromSocket.sin_addr);
				pip = (char*)inet_ntoa(fromSocket.sin_addr);
				eloop_log(DBG_ATC_SOCKET,"atc_socket_thread_rec from :%s:%d  at timestamp: %d \r\n",pip,ntohs(port),eloop_get_timestamp());
				eloop_logb(DBG_ATC_SOCKET,NULL,atc_buffer,ret);
                atc_message_process(ip,port,atc_buffer,ret);
			}
			else
			{
				eloop_log(DBG_ATC_SOCKET,"atc_socket_thread_rec: error occur !\r\n ");
				atc_socket_deinit();
				atc_create_socket();
				eloop_sleep(20);

			}
		}
		else
		{
			eloop_log(DBG_ATC_SOCKET,"atc_socket_thread_rec: error occur 3!\r\n ");
			eloop_sleep(100);
		}
		//eloop_log("atc_socket_thread_rec",NULL,0);
	}
}
int  atc_socket_send(uint32 ip, uint16 port,uint8 *pdata, uint16 len)
{
	struct sockaddr_in mysocket;
	int addrlen = sizeof(mysocket);
	char *s_ip;
	int s_port;
	s_ip = pluto_adapter_ntoa(ip);
	s_port = pluto_adapter_ntohs(port);
	eloop_log(DBG_ATC_SOCKET,"atc_socket_send to :%s:%d  at timestamp: %d \r\n",s_ip,s_port,eloop_get_timestamp());
	eloop_logb(DBG_ATC_SOCKET,NULL,pdata,len);
	//jf_entercriticalSection();
	if(atc_socket_fd>=0)
	{
		eloop_memset(&mysocket,0,sizeof(struct sockaddr_in));
		mysocket.sin_family = AF_INET;
		mysocket.sin_addr.s_addr = ip;//;
		mysocket.sin_port = port;
		if(sendto(atc_socket_fd,pdata,len,0,(struct sockaddr *)&mysocket,addrlen)==0)
		{
			//jf_exitcriticalSection();
			return 0;
		}
	}
	else
	{
		eloop_log(DBG_ATC_SOCKET,"atc_socket_send: socket not open\r\n");
	}
	//jf_exitcriticalSection();
	return -1;
}
static void atc_message_process(uint32 ip, uint16 port, uint8 *pdata,int len)
{
    int slen,dlen;
    ATC_PKG_t 	*pkg = (ATC_PKG_t*)pdata;
    slen = ATC_CheckPackage(pkg,len);
    if(slen>0)
    {
        dlen = ATC_PackageDateLength(pkg);
        eloop_log(DBG_ATC_SOCKET,"atc_message_process read package :cmd0:%02x cmd1:%02x ",pkg->CMD0,pkg->CMD1);
        eloop_logb(DBG_ATC_SOCKET,"data:",pkg->DATA,dlen);
        ATC_ReceiveMessageCB(ATC_MSG_TYPE_WIFI,pkg->CMD0,pkg->CMD1,pkg->DATA,dlen);
    }
}
uint8  atc_udp_send_command(uint8 cmd0, uint8 cmd1, uint8 *pdata, int len)
{
	ATC_PKG_t 	*pkg;
	uint8			ret = ES_FAILED;
	int				slen;
	eloop_log(DBG_ATC_SOCKET,"atc_udp_send_command:cmd0:%02x cmd1:%02x len:%d \r\n",cmd0,cmd1,len);
	eloop_logb(DBG_ATC_SOCKET,"data:",pdata,len);
	pkg = ATC_PackageMessage(cmd0,cmd1,pdata,len);
	if(pkg!=NULL)
	{
		slen = ATC_PackageLength(pkg);
		eloop_logb(DBG_ATC_SOCKET,"atc_udp_send_command:",(uint8*)pkg,slen);
		if((atc_socket_send(0xFFFFFFFF,htons(PLUTO_ATC_SOCKET_PORT),(uint8*)pkg,slen))==0)
		{
			ret = ES_SUCCEED;
		}
		eloop_free(pkg);
	}
	return ret;
}
