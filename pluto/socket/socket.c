#include "eloop.h"
#include "socket_udp.h"
#include "socket_tcp.h"
#include "socket.h"
#include "pluto.h"
#include "pluto_adapter.h"
#include "pluto_entry.h"
static uint8 TCP_Buffer[SOCKET_RECEIVE_BUFFER_SIZE];
static uint8 UDP_Buffer[SOCKET_RECEIVE_BUFFER_SIZE];
int socket_open(uint8 socket_type,uint32 ip, uint16 port)
{
    int ret = SOCKET_STATE_FAILED;
    eloop_log(DBG_PLT_SOCKET,"socket_close\r\n");
    udp_registe_receive_buffer(UDP_Buffer,sizeof(UDP_Buffer));
    tcp_registe_receive_buffer(TCP_Buffer,sizeof(TCP_Buffer));
    if(socket_type==SOCKET_TYPE_UDP)
    {
        if(udp_socket_init(port)==0)
            ret = SOCKET_STATE_SUCCEED;
    }
    else if(socket_type==SOCKET_TYPE_TCP)
    {
        if(tcp_socket_init(ip,port)==0)
            ret = SOCKET_STATE_SUCCEED;
    }
    return ret;
}
int socket_close(uint8 socket_type)
{
    int ret = SOCKET_STATE_FAILED;
    eloop_log(DBG_PLT_SOCKET,"socket_close\r\n");
    if(socket_type==SOCKET_TYPE_UDP)
    {
        if(udp_socket_deinit()==0)
            ret = SOCKET_STATE_SUCCEED;
    }
    else if(socket_type==SOCKET_TYPE_TCP)
    {
        if(tcp_socket_deinit()==0)
            ret = SOCKET_STATE_SUCCEED;
    }
    return ret;
}
int socket_output(uint8 socket_type,uint32 ip, uint16 port, uint8 *pdata, uint16 len)
{
    char* s_ip;
    int s_port;
    int ret = SOCKET_STATE_FAILED;
    s_ip = pluto_adapter_ntoa(ip);
    s_port = pluto_adapter_ntohs(port);
    eloop_log(DBG_PLT_SOCKET,"socket_output:ip:%s: port:%d \r\n",s_ip,s_port);
    if(socket_type==SOCKET_TYPE_UDP)
    {
        if(udp_socket_send(ip,port,pdata,len)==0)
            ret = SOCKET_STATE_SUCCEED;
    }
    else if(socket_type==SOCKET_TYPE_TCP)
    {
        if(tcp_socket_send(pdata,len)==0)
            ret = SOCKET_STATE_SUCCEED;
    }
    return ret;
}