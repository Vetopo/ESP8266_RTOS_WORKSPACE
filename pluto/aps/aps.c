
#include "eloop.h"
#include "pluto.h"
#include "access_output.h"
static ApsCmdListener_t myListener;
static uint16	seq = 0;
extern sint8  access_req_send(Address_t *dst,uint8 seq,Command_t* cmd, uint8 *pdata, uint32 len,uint8 send_option);
extern sint8  access_req_send_state(Address_t *dst,uint8 seq,Command_t* cmd,uint8 state, uint8 *pdata, uint32 len,uint8 send_option);
extern sint8  access_req_report(uint8 port, uint8 seq,Command_t *cmd,uint8 *pdata, uint32 len ,uint8 send_option);
extern sint8  access_req_report_state(uint8 port,uint8 seq,Command_t* cmd,uint8 state, uint8 *pdata, uint32 len,uint8 send_option);
extern sint8  access_mcmd_req_send(Address_t *dst,uint8 seq,MixCommand_t* cmd, int cmd_num,uint8 send_option);
extern sint8  access_mcmd_req_report(uint8 port, uint8 seq,MixCommand_t* cmd, int cmd_num ,uint8 send_option);

void aps_set_cmd_listener(ApsCmdListener_t *plistener)
{
	myListener.recieve_mcmd_cb = plistener->recieve_mcmd_cb;
	myListener.recieve_cmd_cb = plistener->recieve_cmd_cb;
	myListener.send_cmd_cb = plistener->send_cmd_cb;
	myListener.send_mcmd_cb = plistener->send_mcmd_cb;
}
sint8 aps_req_send(Address_t *dst,uint8 seq,Command_t* cmd, uint8 *pdata, uint32 len,uint8 send_option)
{
	return access_req_send(dst,seq,cmd,pdata,len,send_option);
}
sint8 aps_req_send_state(Address_t *dst,uint8 seq,Command_t* cmd,uint8 state, uint8 *pdata, uint32 len,uint8 send_option)
{
	return access_req_send_state(dst,seq,cmd,state,pdata,len,send_option);
}
sint8 aps_req_report(uint8 port, uint8 seq,Command_t *cmd,uint8 *pdata, uint32 len ,uint8 send_option)
{
	return access_req_report(port,seq,cmd,pdata,len,send_option);
}
sint8 aps_req_report_state(uint8 port,uint8 seq,Command_t* cmd,uint8 state, uint8 *pdata, uint32 len,uint8 send_option)
{
	return access_req_report_state(port,seq,cmd,state,pdata,len,send_option);
}
sint8  aps_mcmd_req_send(Address_t *dst,uint8 seq,MixCommand_t* cmd, int cmd_num,uint8 send_option)
{
	return access_mcmd_req_send(dst, seq, cmd,  cmd_num, send_option);
}
sint8  aps_mcmd_req_report(uint8 port, uint8 seq,MixCommand_t* cmd, int cmd_num ,uint8 send_option)
{
	return access_mcmd_req_report( port,  seq, cmd,  cmd_num , send_option);
}
uint16 aps_get_seq(void)
{
	return seq++;
}
void aps_command_input(Address_t *src,uint8 seq, Command_t *cmd, uint8 *pdata,uint32 len)
{
	if(myListener.recieve_cmd_cb!=NULL)
	{
		myListener.recieve_cmd_cb(src,seq,cmd,pdata,len);
	}
}
void aps_mixcommand_input(Address_t *src,uint8 seq, MixCommand_t *cmd,uint32 cmd_num)
{
	if(myListener.recieve_mcmd_cb!=NULL)
	{
		myListener.recieve_mcmd_cb(src,seq,cmd,cmd_num);
	}
}
void aps_send_state_input(Address_t *dst,uint8 seq, Command_t *cmd, uint8 state)
{
	if(myListener.send_cmd_cb!=NULL)
	{
		myListener.send_cmd_cb(dst,seq,cmd,state);
	}
}
void aps_mixcommand_send_state_input(Address_t *dst,uint8 seq, MixCommand_t *cmd, uint8 state)
{
	if(myListener.send_mcmd_cb!=NULL)
	{
		myListener.send_mcmd_cb(dst,seq,cmd,state);
	}
}
