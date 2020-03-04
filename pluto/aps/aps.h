#ifndef __APS_H__
#define __APS_H__

#include "eloop.h"

typedef enum
{
	cmd_invalide		=  	0x00,
	cmd_read			=	0x01,
	cmd_write 			= 	0x02,
	cmd_del				= 	0x03,
	cmd_toggle			= 	0x04,
	cmd_alarm			= 	0x05,
	cmd_readname		= 	0x06,
	cmd_rename 			= 	0x07,
	cmd_readstate		=   0x08,
	cmd_writestate		=	0x09,
	cmd_readCfg			= 	0x0A,
	cmd_writeCfg		= 	0x0B,
	cmd_beacon			= 	0x0C,
	cmd_notify			=	0x0D,
	cmd_return			=  	0x80,
}PLUTO_DEFAULT_CMD;

typedef enum
{
	KEYID_DEFAULT 	= 0x00,
	KEYID_SN		= 0x01,
	KEYID_ADMIN		= 0x02,
	KEYID_COMMON	= 0x03,
	KEYID_GUEST		= 0x04,
	KEYID_TARGET	= 0x08,
}PLUTO_KEYID;

typedef enum
{
	DTYPE_UNKOWN = 0,
	DTYPE_BYTE,
	DTYPE_INT ,
	DTYPE_FLOAT,
	DTYPE_STRING,
}PLUTO_DATA_TYPE;

typedef enum
{
	send_option_default				= 0x00,//no option
	send_option_skip_scene 			= 0x01,//send message and skip trigger scene
	send_option_skip_route  		= 0x02,//send message without route device
	send_option_no_record 			= 0x04,//send message no record
	send_option_no_delay			= 0x08,//send message no delay
	send_option_Qos_1				= 0x10,//send message and try 24 hour(Will be supported in the future)
	send_option_Qos_2				= 0x20,//send message and try 7 days(Will be supported in the future)
	send_option_present_scene		= 0x40,//message only for trigger scene
	receive_option_from_svr 		= 0x80,//receive message no from local
	receive_option_default			= 0x00,
}PLUTO_OPERATION_OPTIONS;

typedef struct
{
	int 	version;
	char 	*MFG;
	char 	*device_name;
	int	 	device_type;
	int		guestLock;
	int  	shareLock;
	int		localLock;
	int		remoteLock;
	int		zone;
	int		aIDNum;
	uint32	*aID;
}DeviceDescribe_t;

typedef struct device
{
	uint8	addr[8];
	uint8 	keyID;
	uint8	port;
	uint8	option;
}Address_t;

typedef struct command
{
	uint32 	aID;
	uint8 	cmd_id;
	uint8 	option;
	uint8	reserve[2];
}Command_t;

typedef struct mix_command
{
	uint32 	aID;
	uint8 	cmd_id;
	uint8 	option;
	int		dlen;
	uint8	*pdata;
}MixCommand_t;

typedef void (*aps_recieve_cmd_t)(Address_t *src,uint8 seq, Command_t *cmd, uint8 *pdata,uint32 len);
typedef void (*aps_recieve_mcmd_t)(Address_t *src,uint8 seq, MixCommand_t *cmd,uint32 cmd_num);
typedef void (*aps_send_cmd_cb_t)(Address_t *dst,uint8 seq, Command_t *cmd, uint8 state);
typedef void (*aps_send_mcmd_t)(Address_t *dst,uint8 seq, MixCommand_t *cmd, uint8 state);

typedef struct
{
	aps_recieve_mcmd_t		recieve_mcmd_cb;
	aps_recieve_cmd_t		recieve_cmd_cb;
	aps_send_cmd_cb_t		send_cmd_cb;
	aps_send_mcmd_t			send_mcmd_cb;
}ApsCmdListener_t;

void 	aps_set_cmd_listener(ApsCmdListener_t *listener);

sint8 	aps_req_send(Address_t *dst,uint8 seq,Command_t* cmd, uint8 *pdata, uint32 len,uint8 send_option);
sint8 	aps_req_send_state(Address_t *dst,uint8 seq,Command_t* cmd,uint8 state, uint8 *pdata, uint32 len,uint8 send_option);
sint8	aps_req_report(uint8 port, uint8 seq,Command_t *cmd,uint8 *pdata, uint32 len ,uint8 send_option);
sint8 	aps_req_report_state(uint8 port,uint8 seq,Command_t* cmd,uint8 state, uint8 *pdata, uint32 len,uint8 send_option);
sint8  	aps_mcmd_req_send(Address_t *dst,uint8 seq,MixCommand_t* cmd, int cmd_num,uint8 send_option);
sint8  	aps_mcmd_req_report(uint8 port, uint8 seq,MixCommand_t* cmd, int cmd_num ,uint8 send_option);
uint16 	aps_get_seq(void);

#endif

