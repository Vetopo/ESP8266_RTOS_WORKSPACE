
#include "eloop.h"
#include "pluto.h"

AfListener_t *pAF_Adapter = NULL;
static led_blink_cb_t blink_led=NULL;

extern char*   	pdo_update_read_hardware_info(void);
extern sint8	pdo_set_device_describe(AfDescribe_t *desc);
extern char*    pdo_read_device_describe(void);
extern sint8 	pdo_req_send_annouce(uint8 *pdata, uint32 len);
extern sint8 	pdo_req_send_beacon(uint8 keyID, uint8 *addr, uint8 *pdata, uint32 len);
extern uint8 	login_get_state(void);
extern void 	login_init(uint8* user, uint8 *psw);
extern sint8 	login_start(void);
extern sint8 	login_stop(void);
extern sint8 	login_logout(void);
extern uint8*	local_get_address(void);
extern uint8*	local_get_password(void);
extern uint8*	local_get_device_key(uint8 key_id);

void af_register_listener(AfListener_t *adp)
{
   pAF_Adapter = adp;
}
sint8 af_set_device_describe(AfDescribe_t *desc)
{
	return pdo_set_device_describe(desc);
}
char *af_get_device_describe(void)
{
	return pdo_read_device_describe();
}
sint8 af_req_send_annouce(uint8 *pdata, uint32 len)
{
	return pdo_req_send_annouce(pdata,len);
}
sint8 af_req_send_beacon(uint8 keyID, uint8 *addr, uint8 *pdata, uint32 len)
{
	return pdo_req_send_beacon(keyID,addr,pdata,len);
}
char *af_read_firmware_info(void)
{
   return pdo_update_read_hardware_info();
}

void af_login_set_user(char *user, char *psw)
{
	login_init((uint8 *)user,(uint8*)psw);
}
void af_login_get_user(char user[8],char psw[16])
{
	eloop_memcpy(user,local_get_address(),8);
	eloop_memcpy(psw,local_get_password(),16);
}
uint8 af_login_start(void)
{
	return login_start();
}
uint8 af_login_stop(void)
{
	return login_stop();
}
uint8 af_logout(void)
{
	return login_logout();
}
uint8 af_login_get_state(void)
{
	return login_get_state();
}
void af_registe_led(led_blink_cb_t cb)
{
	blink_led= cb;
}
void af_led_blink(int num, int htime, int ltime)
{
	if(login_get_state()==LOGIN_STATE_ONLINE)
	{
		if(blink_led!=NULL)
		{
			blink_led(num,htime,ltime);
		}
	}
}
uint8 *af_get_local_addr(void)
{
	return local_get_address();
}
uint8 *af_get_local_key(uint8 keyID)
{
	return local_get_device_key(keyID);
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
