#ifndef _PLT_SOCKET_H__
#define _PLT_SOCKET_H__

#include "eloop.h"

#define SOCKET_RECEIVE_BUFFER_SIZE      (1500)

int socket_open(uint8 socket_type,uint32 ip, uint16 port);
int socket_close(uint8 socket_type);
int socket_output(uint8 socket_type,uint32 ip, uint16 port, uint8 *pdata, uint16 len);

#endif

