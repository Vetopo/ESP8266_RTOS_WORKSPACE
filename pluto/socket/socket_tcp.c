/*
 * tcp_socket.c
 *
 *  Created on: 2019.9.1
 *      Author: lort
 */

#include "eloop.h"

#include "socket_tcp.h"
#include "pluto.h"
#include "pluto_adapter.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include <netdb.h>
#include <sys/socket.h>

#define TCP_CONNECT_TRY		(2)
struct tcp_send_nwk_t
{
	uint32	srcIP;
	uint16	srcPort;
	int		len;
	uint8 	pdata[0];
};

extern void nwk_socket_input(uint8 socket_type, uint32 ip, uint16 port, uint8 *pdata, uint16 len);

static uint8	tcp_task_init = ES_FALSE;
static uint8	tcp_connect_flag = ES_FAILED;
static int 		tcp_socket_fd = -1;
static struct 	sockaddr_in mysocket;
static uint8 	*tcp_buffer=NULL;//receive 
static int		tcp_buffer_size = 0;

static void tcp_create_socket(void);
static void tcp_release_socket(void);
static void tcp_socket_thread_rec(void *arg);
struct udp_send_nwk_t
{
	uint32	srcIP;
	uint16	srcPort;
	int		len;
	uint8 	pdata[0];
};

int tcp_socket_init(uint32 ip, uint16 port)
{
	char *s_ip;
	tcp_release_socket();
	s_ip = pluto_adapter_ntoa(ip);
	eloop_log(DBG_PLT_SOCKET,"tcp_socket_init:ip:%s: port:%d \r\n",s_ip,pluto_adapter_ntohs(port));
	eloop_memset(&mysocket,0,sizeof(struct sockaddr_in));
	mysocket.sin_family = AF_INET;
	mysocket.sin_addr.s_addr = ip;
	mysocket.sin_port = port;
	tcp_connect_flag = ES_FAILED;
	//udp_socket.sin_len = sizeof(udp_socket);
	if(tcp_task_init==ES_FALSE)
	{
		xTaskCreate(tcp_socket_thread_rec, "tcp_socket_thread_rec", 2048, NULL, (tskIDLE_PRIORITY + 2), NULL);
		tcp_task_init = ES_TRUE;
	}
	return 0;
}
int tcp_registe_receive_buffer(uint8 *buf, int size)
{
	tcp_buffer = buf;
	tcp_buffer_size = size;
	return 0;
}
sint8 tcp_socket_deinit(void)
{
	tcp_release_socket();
	tcp_buffer = NULL;
	tcp_buffer_size = 0;
	return ES_SUCCEED;
}
static void tcp_release_socket(void)
{
	if(tcp_socket_fd>0)
	{
		//shutdown(tcp_socket_fd,SHUT_RDWR);
		tcp_connect_flag = ES_FAILED;
		close(tcp_socket_fd);
		tcp_socket_fd = -1;
		eloop_sleep(100);
	}
}
static void tcp_create_socket(void)
{
	int ret = -1;
	eloop_log(DBG_PLT_SOCKET,"tcp_create_socket \r\n");
	tcp_socket_fd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(tcp_socket_fd<0)
	{
		eloop_log(DBG_PLT_SOCKET,"tcp_create_socket :create socket fd error :%d\r\n",tcp_socket_fd);
		pluto_adapter_restart(3000);
		return;
	}
	eloop_log(DBG_PLT_SOCKET,"tcp_create_socket :create socket fd :%d \r\n",tcp_socket_fd);
	ret = connect(tcp_socket_fd,(struct sockaddr *)&(mysocket),sizeof(mysocket));
	if(ret<0)
	{
		eloop_log(DBG_PLT_SOCKET,"tcp_create_socket tcp connect error fd:%d \r\n",tcp_socket_fd);
		tcp_release_socket();
	}
	else
	{
		tcp_connect_flag = ES_SUCCEED;
		eloop_log(DBG_PLT_SOCKET,"tcp_create_socket tcp connect server succeed fd:%d \r\n",tcp_socket_fd);
	}
}
static void tcp_socket_thread_rec(void *arg)
{
	char *s_ip;
	int s_port;
	int ret;
	tcp_create_socket();
	while(1)
	{
		//ret = recvfrom(udp_socket_fd,pudp_socket_buf,defTcpSocketBufferSize,0,(struct sockaddr *)&fromSocket,&fromSlen);
		while(tcp_socket_fd>=0)
		{
			ret = recv(tcp_socket_fd,tcp_buffer,tcp_buffer_size,0);
			if(ret>0)
			{
				af_led_blink(1,10,0);
				s_ip = pluto_adapter_ntoa(mysocket.sin_addr.s_addr);
				s_port = pluto_adapter_ntohs(mysocket.sin_port);
				eloop_log(DBG_PLT_SOCKET,"socket receive from:%s:%d\r\n",s_ip,s_port);
				eloop_logb(DBG_PLT_SOCKET,NULL,tcp_buffer,ret);
				nwk_socket_input(SOCKET_TYPE_TCP,mysocket.sin_addr.s_addr,mysocket.sin_port,tcp_buffer,ret);
			}
			else
			{
				eloop_log(DBG_PLT_ERROR,"tcp_socket_thread_rec: error occur and break loop !\r\n ");
				tcp_connect_flag = ES_FAILED;
				break;
			}
		}
		eloop_log(DBG_PLT_ERROR,"tcp_socket_thread_rec: socket error  and reninit  again\r\n ");
		tcp_release_socket();
		eloop_sleep(3000);
		tcp_create_socket();
		eloop_sleep(100);
		//eloop_log("udp_socket_thread_rec",NULL,0);
	}
}
int tcp_socket_send(uint8 *pdata, uint16 len)
{
	eloop_log(DBG_PLT_SOCKET,"tcp_socketSend to :fd:%d  at timestamp: %d \r\n",tcp_socket_fd,eloop_get_timestamp());
	eloop_logb(1,NULL,pdata,len);
	if(tcp_connect_flag==ES_SUCCEED)
	{
		if(tcp_socket_fd>=0)
		{
			af_led_blink(1,10,0);
			if(send(tcp_socket_fd,pdata,len,0)==0)
				return 0;
		}
		else
		{
			eloop_log(DBG_PLT_ERROR,"tcp_socketSend: socket do not connect !\r\n ");
		}
	}
	return -1;
}

