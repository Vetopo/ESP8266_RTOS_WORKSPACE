
#include "eloop.h"
#include "af.h"

AfListener_t *pAF_Adapter = NULL;

extern char*   	pdo_update_read_hardware_info(void);
extern sint8	pdo_set_device_describe(AfDescribe_t *desc);
extern void 	pdo_fs_set_listener(AfFsListener_t *adp);
extern sint8 	pdo_req_send_annouce(uint8 *pdata, uint32 len);
extern sint8 	pdo_req_send_beacon(uint8 keyID, uint8 *addr, uint8 *pdata, uint32 len);
extern sint8 	pdo_register_port(uint8 port_num,uint32 applicationID, uint32 *attr_list, uint8 attr_num, uint8 force);

void af_register_listener(AfListener_t *adp)
{
   pAF_Adapter = adp;
}
void af_set_fs_listener(AfFsListener_t *listener)
{
	pdo_fs_set_listener(listener);
}
sint8 af_set_describe(AfDescribe_t *desc)
{
	return pdo_set_device_describe(desc);
}
sint8 af_register_port(uint8 port_num,uint32 applicationID, uint32 *attr_list, uint8 attr_num, uint8 force)
{
	return pdo_register_port( port_num, applicationID, attr_list, attr_num, force);
}
sint8 af_req_send_annouce(uint8 *pdata, uint32 len)
{
	return pdo_req_send_annouce(pdata,len);
}
sint8 af_req_send_beacon(uint8 keyID, uint8 *addr, uint8 *pdata, uint32 len)
{
	return pdo_req_send_beacon(keyID,addr,pdata,len);
}
char *af_read_hardware_info(void)
{
   return pdo_update_read_hardware_info();
}

void af_reacieve_beacon_cb(uint8 *src, uint8 lqi,uint8 *pdata, uint32 len)
{
	if((pAF_Adapter!=NULL)&&(pAF_Adapter->rec_beacon_cb!=NULL))
		pAF_Adapter->rec_beacon_cb(src,lqi,pdata,len);	
}
void af_permite_join_cb(uint8 state, uint16 duration)
{
	if((pAF_Adapter!=NULL)&&(pAF_Adapter->permit_join_cb!=NULL))
		pAF_Adapter->permit_join_cb(state,duration);
}
void af_recieve_device_announce_cb(uint8*src,uint8 *pdata, uint8 len)
{
	if((pAF_Adapter!=NULL)&&(pAF_Adapter->rec_annouce_cb!=NULL))
	{
		pAF_Adapter->rec_annouce_cb(src,pdata,len);
	}
}
void af_delete_port_cb(uint8 port)
{
	if((pAF_Adapter!=NULL)&&(pAF_Adapter->delete_port_cb!=NULL))
	{
		pAF_Adapter->delete_port_cb(port);
	}
}
uint8 af_read_port_lqi_cb(uint8 port)
{
	if((pAF_Adapter!=NULL)&&(pAF_Adapter->read_lqi_cb!=NULL))
	{
		return pAF_Adapter->read_lqi_cb(port);
	}
	return 0x00;
}
void af_update_hardware_cb(Address_t *src,char *ip, uint16 port, char *url)
{
	if((pAF_Adapter!=NULL)&&(pAF_Adapter->req_update_cb!=NULL))
	{
		pAF_Adapter->req_update_cb(src,ip,port,url);
	}
}
void af_login_state_cb(uint8 state)
{
	if((pAF_Adapter!=NULL)&&(pAF_Adapter->login_state_cb!=NULL))
	{
		pAF_Adapter->login_state_cb(state);
	}	
}
void af_read_device_time_cb(Address_t *src, uint8 state, uint8 zone, uint32 unix_time)
{

}
void af_write_device_time_cb(Address_t *src, uint8 state)
{

}
