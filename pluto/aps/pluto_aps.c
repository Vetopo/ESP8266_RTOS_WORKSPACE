
#include "eloop.h"
#include "pluto.h"
static ApsCmdListener_t myListener;
static uint16	seq = 0;

extern sint8 	pdo_register_port(uint8 port_num,uint32 applicationID, uint32 *attr_list, uint8 attr_num, uint8 force);
extern uint8  	pdo_read_port_num(void);
extern uint8*  	pdo_read_port_list(void);
extern uint8 	pdo_get_free_port(void);
extern uint8 	pdo_port_is_exist(uint8 port);
extern char* 	pdo_read_port_describe(uint8 port);
extern sint8  	pdo_update_port_describe(char *describe);
extern sint8 	pdo_delete_port(uint8 port_num);

extern sint8 	pdo_port_set_value(uint8 port,char *obj, int value);
extern sint8 	pdo_port_set_string(uint8 port, char *obj, char *str);
extern uint32 	pdo_port_get_value(uint8 port, char *obj);
extern char* 	pdo_port_get_string(uint8 port, char *obj);
extern uint8 	pdo_count_port_by_value(char *obj,int value);
extern uint8 	pdo_count_port_by_string(char *obj,char *str);
extern uint8 	pdo_get_port_by_value_index(char *obj,int value, uint8 index);
extern uint8 	pdo_get_port_by_string_index(char *obj,char *str, uint8 index);
extern uint8	pdo_get_port_by_value2(char *obj1,int value1, char *obj2, int value2);

extern void 	pdo_set_fs_listener(ApsFsListener_t *adp);
extern sint8 	pdo_req_write_file(Address_t *dst,uint8 seq,char *name, uint8 *pdata, int len);
extern sint8 	pdo_req_rename(Address_t *dst,uint8 seq,char *name,char *new_name);
extern sint8 	pdo_req_read_file(Address_t *dst,uint8 seq,char *name);
extern sint8 	pdo_req_read_name(Address_t *dst,uint8 seq,char *name);
extern sint8 	pdo_req_delete_file(Address_t *dst,uint8 seq,char *name);

extern sint8  	access_req_send(Address_t *dst,uint8 seq,Command_t* cmd, uint8 *pdata, uint32 len,uint8 send_option);
extern sint8  	access_req_send_state(Address_t *dst,uint8 seq,Command_t* cmd,uint8 state, uint8 *pdata, uint32 len,uint8 send_option);
extern sint8  	access_req_report(uint8 port, uint8 seq,Command_t *cmd,uint8 *pdata, uint32 len ,uint8 send_option);
extern sint8  	access_req_report_state(uint8 port,uint8 seq,Command_t* cmd,uint8 state, uint8 *pdata, uint32 len,uint8 send_option);
extern sint8  	access_mcmd_req_send(Address_t *dst,uint8 seq,MixCommand_t* cmd, int cmd_num,uint8 send_option);
extern sint8  	access_mcmd_req_report(uint8 port, uint8 seq,MixCommand_t* cmd, int cmd_num ,uint8 send_option);

extern int  	device_key_add(uint8 *addr,uint8 keyID,uint8 *key);
extern uint8*	device_key_get(uint8 *addr, uint8 keyID);
extern int  	device_key_clean(void);
extern int  	device_key_remove(uint8 *addr);
extern int  	device_key_remove_key(uint8 *addr,uint8 keyID);

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

void aps_set_fs_listener(ApsFsListener_t *listener)
{
	pdo_set_fs_listener(listener);
}
sint8 	aps_req_write_file(Address_t *dst,uint8 seq,char *name, uint8 *pdata, int len)
{
	return 	pdo_req_write_file( dst, seq, name,  pdata, len);
}
sint8 	aps_req_rename_file(Address_t *dst,uint8 seq,char *name,char *new_name)
{
	return 	pdo_req_rename( dst, seq, name, new_name);
}
sint8 	aps_req_read_file(Address_t *dst,uint8 seq,char *name)
{
	return  pdo_req_read_file( dst, seq, name);
}
sint8 	aps_req_read_name(Address_t *dst,uint8 seq,char *name)
{
	return 	pdo_req_read_name( dst, seq, name);
}
sint8 	aps_req_delete_file(Address_t *dst,uint8 seq,char *name)
{
	return 	pdo_req_delete_file( dst, seq, name);
}

uint16 aps_get_seq(void)
{
	return seq++;
}

sint8 aps_add_device_key(uint8 *addr,uint8 keyID,uint8 *key)
{
	return device_key_add(addr,keyID,key);
}
uint8 *aps_get_device_key(uint8 *addr, uint8 keyID)
{
	return device_key_get(addr,keyID);
}
sint8 aps_remove_device_keys(void)
{
	return device_key_clean();
}
sint8 aps_remove_device_key(uint8 *addr)
{
	return device_key_remove(addr);
}
sint8 aps_remove_key(uint8 *addr,uint8 keyID)
{
	return device_key_remove_key(addr,keyID);
}


sint8 aps_register_port(uint8 port_num,uint32 applicationID, uint32 *attr_list, uint8 attr_num, uint8 force)
{
	return pdo_register_port( port_num, applicationID,  attr_list,  attr_num,  force);
}
uint8 aps_read_port_num(void)
{
	return	pdo_read_port_num();
}
uint8* aps_read_port_list(void)
{
	return pdo_read_port_list();
}
uint8 aps_get_free_port(void)
{
	return pdo_get_free_port();
}
uint8 aps_port_is_exist(uint8 port)
{
	return pdo_port_is_exist(port);
}
char* aps_read_port_describe(uint8 port)
{
	return pdo_read_port_describe(port);
}
sint8 aps_update_port_describe(char *describe)
{
	return pdo_update_port_describe(describe);
}
sint8 aps_delete_port(uint8 port_num)
{
	return pdo_delete_port(port_num);
}
sint8 aps_port_set_value(uint8 port,char *obj, int value)
{
	return pdo_port_set_value( port, obj, value);
}
sint8 aps_port_set_string(uint8 port, char *obj, char *str)
{
	return pdo_port_set_string( port,  obj,  str);
}
uint32 	aps_port_get_value(uint8 port, char *obj)
{
	return pdo_port_get_value( port, obj);
}
char* aps_port_get_string(uint8 port, char *obj)
{
	return pdo_port_get_string( port, obj);
}
uint8 aps_count_port_by_value(char *obj,int value)
{
	return pdo_count_port_by_value( obj, value);
}
uint8 aps_count_port_by_string(char *obj,char *str)
{
	return pdo_count_port_by_string( obj,str);
}
uint8 aps_get_port_by_value_index(char *obj,int value, uint8 index)
{
	return pdo_get_port_by_value_index( obj, value, index);
}
uint8 aps_get_port_by_string_index(char *obj,char *str, uint8 index)
{
	return pdo_get_port_by_string_index( obj, str, index);
}
uint8 aps_get_port_by_value2(char *obj1,int value1, char *obj2, int value2)
{
	return pdo_get_port_by_value2(obj1, value1, obj2, value2);
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
