/*
 * udp_socket.c
 *
 *  Created on: Jul 22, 2016
 *      Author: lort
 */

#include "eloop.h"

#include "pluto.h"
#include "pluto_adapter.h"
#include "socket_udp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include <netdb.h>
#include <sys/socket.h>

extern void nwk_socket_input(uint8 socket_type, uint32 ip, uint16 port, uint8 *pdata, uint16 len);

static uint8	udp_task_init = ES_FALSE;
static uint8	udp_connect_flag = ES_FAILED;
static int 		udp_socket_fd = -1;
static struct 	sockaddr_in udp_socket;
static uint8 	*udp_buffer = NULL;//receive buffer
static int		udp_buffer_size = 0;

static void  udp_create_socket(void);
static void udp_release_socket(void);
void udp_socket_thread_rec(void *arg);
struct udp_send_nwk_t
{
	uint32	srcIP;
	uint16	srcPort;
	int		len;
	uint8 	pdata[0];
};

int  udp_socket_init(uint16 port)
{
	eloop_log(DBG_PLT_SOCKET,"udp_socket_init: port:%d \r\n",pluto_adapter_ntohs(port));
	udp_release_socket();
	eloop_memset(&udp_socket,0,sizeof(struct sockaddr_in));
	udp_socket.sin_family = AF_INET;
	udp_socket.sin_addr.s_addr = INADDR_ANY;
	udp_socket.sin_port = port;
	udp_connect_flag = ES_FAILED;
	//udp_socket.sin_len = sizeof(udp_socket);
	if(udp_task_init==ES_FALSE)
	{
		xTaskCreate(udp_socket_thread_rec, "udp_socket_thread_rec", 2048, NULL, (tskIDLE_PRIORITY + 2), NULL);
		udp_task_init = ES_TRUE;
	}
	return 0;
}
int udp_registe_receive_buffer(uint8 *buf, int size)
{
	udp_buffer = buf;
	udp_buffer_size = size;
	return 0;
}
int udp_socket_deinit(void)
{
	udp_release_socket();
	udp_buffer = NULL;
	udp_buffer_size = 0;
	return ES_SUCCEED;
}
static void udp_release_socket(void)
{
	if(udp_socket_fd>0)
	{
		//shutdown(udp_socket_fd,SHUT_RDWR);
		udp_connect_flag = ES_FAILED;
		close(udp_socket_fd);
		udp_socket_fd = -1;
		eloop_sleep(100);
	}
}
static void  udp_create_socket(void)
{
	int ret;
	int so_br = 1;
	eloop_log(DBG_PLT_SOCKET,"udp_socket_create_socket :got IP \r\n");
	if(udp_socket_fd<0)
	{
		udp_socket_fd = socket(AF_INET,SOCK_DGRAM,0);
		if(udp_socket_fd<0)
		{
			eloop_log(DBG_PLT_SOCKET,"udp_socket_create_socket creat socket_fd error \r\n");
			pluto_adapter_restart(3000);
			return;
		}
		else
			setsockopt(udp_socket_fd,SOL_SOCKET,SO_BROADCAST,&so_br,sizeof(so_br));
		eloop_log(DBG_PLT_SOCKET,"udp_socket_create_socket :create socket fd \r\n");
		ret = bind(udp_socket_fd,(struct sockaddr *)&(udp_socket),sizeof(udp_socket));
		if(ret<0)
		{
			eloop_log(DBG_PLT_SOCKET,"udp_socket_create_socket bind socket_fd error \r\n");
			udp_release_socket();
		}
		else
		{
			udp_connect_flag = ES_SUCCEED;
			eloop_log(DBG_PLT_SOCKET,"udp_create_socket bind socket succeed fd:%d \r\n",udp_socket_fd);
		}
		
	}
	eloop_log(DBG_PLT_SOCKET,"udp_socket_create_socket :binded socket finished\r\n");
}
void  udp_socket_thread_rec(void *arg)
{
	struct sockaddr_in fromSocket;
	socklen_t 	fromSlen = sizeof(fromSocket);
	uint32 ip;
	char *pip;
	uint16 port;
	int ret;
	udp_create_socket();
	while(1)
	{
		while((udp_socket_fd>=0)&&(udp_buffer!=NULL))
		{
			ret = recvfrom(udp_socket_fd,udp_buffer,udp_buffer_size,0,(struct sockaddr *)&fromSocket,&fromSlen);
			if(ret>0)
			{
				af_led_blink(1,10,0);
				port = (uint16)fromSocket.sin_port;//ntohs(fromSocket.sin_port);
				ip = (uint32)fromSocket.sin_addr.s_addr;//ntohl(fromSocket.sin_addr);
				pip = (char*)inet_ntoa(fromSocket.sin_addr);
				eloop_log(DBG_PLT_SOCKET,"udp_socket_thread_rec from :%s:%d  at timestamp: %d \r\n",pip,ntohs(port),eloop_get_timestamp());
				eloop_logb(DBG_PLT_SOCKET,NULL,udp_buffer,ret);
				nwk_socket_input(SOCKET_TYPE_UDP,ip,port,udp_buffer,ret);
			}
			else
			{
				udp_connect_flag = ES_FAILED;
				eloop_log(DBG_PLT_ERROR,"udp_socket_thread_rec: error occur !\r\n ");
				break;
			}
		}
		udp_release_socket();
		eloop_sleep(3000);
		udp_create_socket();
		eloop_sleep(100);
		//eloop_log("udp_socket_thread_rec",NULL,0);
	}
}
int  udp_socket_send(uint32 ip, uint16 port,uint8 *pdata, uint16 len)
{
	struct sockaddr_in mysocket;
	int addrlen = sizeof(mysocket);
	char *s_ip;
	int s_port;
	s_ip = pluto_adapter_ntoa(ip);
	s_port = pluto_adapter_ntohs(port);
	eloop_log(DBG_PLT_SOCKET,"udp_socket_send to :%s:%d  at timestamp: %d \r\n",s_ip,s_port,eloop_get_timestamp());
	eloop_logb(DBG_PLT_SOCKET,NULL,pdata,len);
	if(udp_connect_flag == ES_SUCCEED)
	{
		if(udp_socket_fd>=0)
		{
			af_led_blink(1,10,0);
			eloop_memset(&mysocket,0,sizeof(struct sockaddr_in));
			mysocket.sin_family = AF_INET;
			mysocket.sin_addr.s_addr = ip;//;
			mysocket.sin_port = port;
			if(sendto(udp_socket_fd,pdata,len,0,(struct sockaddr *)&mysocket,addrlen)==0)
			{
				return 0;
			}
		}
		else
		{
			eloop_log(DBG_PLT_ERROR,"udp_socket_send: socket not open\r\n");
		}
	}
	return -1;
}
