#ifndef __PLUTO_AF_H__
#define __PLUTO_AF_H__

#include "eloop.h"
#include "pluto_aps.h"


/*************receive command callback functions************************************/
typedef void 	(*af_rec_beacon_cb_t)(uint8 *src, uint8 lqi,uint8 *pdata, uint32 len);
typedef void 	(*af_rec_annouce_cb_t)(uint8*src,uint8 *pdata, uint8 len);
typedef void    (*af_rec_login_state_cb_t)(uint8 state);

typedef void 	(*af_req_permit_join_cb_t)(uint8 state, uint16 duration);
typedef void 	(*af_req_del_port_cb_t)(uint8 port);
typedef uint8 	(*af_req_read_lqi_cb_t)(uint8 port);
typedef void 	(*af_rsp_read_time_cb_t)(uint8 state,uint8 zone, uint32 unix_time);
typedef void	(*af_req_update_cb_t)(Address_t *src,char *ip, uint16 port, char *url);

typedef void    (*led_blink_cb_t)(int num,int htime, int ltime);


typedef struct
{
	int 	version;
	char 	*MFG;
	char	*bin_name;
	char 	*device_name;
	int	 	device_type;
	int		guestLock;
	int  	shareLock;
	int		localLock;
	int		remoteLock;
	int		zone;
	int		aIDNum;
	uint32	*aID;
}AfDescribe_t;

typedef struct
{
    af_rec_beacon_cb_t		    rec_beacon_cb;
    af_rec_annouce_cb_t		    rec_annouce_cb;
    af_rec_login_state_cb_t	    login_state_cb;

    af_req_permit_join_cb_t		permit_join_cb;
    af_req_del_port_cb_t		delete_port_cb;
    af_req_read_lqi_cb_t		read_lqi_cb;
    af_req_update_cb_t          req_update_cb;
}AfListener_t;

void    af_register_listener(AfListener_t *listener);
sint8   af_set_device_describe(AfDescribe_t *desc);
char*	af_get_device_describe(void);
sint8 	af_req_send_annouce(uint8 *pdata, uint32 len);
sint8   af_req_send_beacon(uint8 keyID, uint8 *addr, uint8 *pdata, uint32 len);
char*   af_read_firmware_info(void);

void 	af_login_set_user(char *user, char *psw);
void 	af_login_get_user(char user[8],char psw[16]);
uint8 	af_login_start(void);
uint8 	af_login_stop(void);
uint8 	af_login_get_state(void);
uint8 	af_logout(void);

uint8*	af_get_local_addr(void);
uint8*	af_get_local_key(uint8 keyID);

void 	af_registe_led(led_blink_cb_t cb);
void 	af_led_blink(int num, int htime, int ltime);

#endif

