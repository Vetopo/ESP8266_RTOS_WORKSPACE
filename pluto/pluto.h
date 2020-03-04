/*
 * pluto_service.h
 *
 *  Created on: 2016��8��31��
 *      Author: lort
 */

#ifndef PLUTO_PLUTO_ENTRY_H_
#define PLUTO_PLUTO_ENTRY_H_

#include "aps.h"
#include "af.h"

#define DBG_PLT_SN			0
#define	DBG_PLT_SOCKET		0
#define DBG_PLT_ERROR		3

/************socket adapter callback functions*************************/
typedef int     (*socket_open_t)(uint8 socket_type,uint32 service_ip, uint16 service_port);
typedef int     (*socket_close_t)(uint8 socket_type);
typedef int     (*socket_output_t)(uint8 socket_type,uint32 ip, uint16 port, uint8 *pdata, uint16 len);
/*************led indicate callback functions**************************/
typedef void    (*led_blink_cb_t)(int num,int htime, int ltime);

typedef enum
{
	SOCKET_TYPE_INVALID = 0,
	SOCKET_TYPE_UDP = 1,
	SOCKET_TYPE_TCP = 2,
    SOCKET_TYPE_LOOP = 3,
}PLUTO_SOCKET_TYPE;

typedef enum
{
    SOCKET_STATE_SUCCEED = 0,
    SOCKET_STATE_FAILED = 1,
    SOCKET_STATE_NOT_OPEN = 3,
    SOCKET_STATE_ERROR = 4
}PLUTO_SOCKET_STATUS;

typedef enum
{
	OP_STATE_SUCCEED 		= 0x00,
	OP_STATE_FAILED		= 1,//1
	OP_STATE_ERROR		=2,//2
	OP_STATE_INVALID		=3,//3
	OP_STATE_PERMIT_DENIED =4,//4
	OP_STATE_REPEAT		=5,//5
	OP_STATE_NO_PORT		=6,//6
	OP_STATE_NO_FILED		=7,//7
	OP_STATE_READ_ERROR	=8,//8
	OP_STATE_WRITE_ERROR 	=9,//9

	SCENE_STATE_INVALID	=10,//10
    SCENE_STATE_RUN		=11,//11
	SCENE_STATE_PAUSE	=12,//12
	SCENE_STATE_STOP		=13,//13
	SCENE_STATE_FINISHED	=14,//14
	SCENE_STATE_NO_TASK	=15,//15
	SCENE_STATE_TICK	=16,//16
	SCENE_STATE_TEST		=17,//17
	SCENE_STATE_JOIN	=18,//18
	SCENE_STATE_SET_PARAM	=19,//19

	OP_UPGRADE_STOP  = 0x20,
	OP_UPGRADE_START = 0x21,
	OP_UPGRADE_FINISHED = 0x22,

	LOGIN_STATE_IDLE = 0,
	LOGIN_STATE_STOP = 1,
	LOGIN_STATE_START = 2,
	LOGIN_STATE_ONLINE = 3,
	LOGIN_STATE_OFFLINE = 4,
	LOGIN_STATE_LOGIN_FAILED = 5,
	LOGIN_STATE_LOGOUT_FAILED = 6,
}PLUTO_SATE;

typedef struct
{
    socket_open_t 	open;
    socket_close_t	close;
    socket_output_t	output;
}SocketAdapter_t;

typedef struct pluto
{
	uint32 	version;
	char	*bin_name;
	char	*chip_name;
	char	*ota_support;
	char	*compile_date;
}FirmwareInfo_t;

typedef struct
{
	uint32	product_ID;//��Ʒ������룬HA���Ʒ����Ϊ1
	uint32  vendor_ID;
	char	*product_date;
	char	*password;
	uint8 	*sn;
	uint8	*addr;
	char	*url;
	char	*server_ipv4;
	uint16	server_udp_port;
	uint16	server_tcp_port;
	uint16	local_udp_port;
	uint16	version;//��Ʒ��ʼ�汾
}PlutoServer_t;


void 	pluto_init(FirmwareInfo_t *info);
void	pluto_deinit(void);
void	pluto_registe_led(led_blink_cb_t cb);
void 	pluto_led_blink(int num, int htime, int ltime);
sint8 	pluto_write_server(PlutoServer_t *pserver);

#endif /* PLUTO_PLUTO_ENTRY_H_ */
