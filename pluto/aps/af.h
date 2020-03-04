#ifndef __NLME_H__
#define __NLME_H__

#include "eloop.h"
#include "aps.h"


/*************receive command callback functions************************************/
typedef void 	(*af_rec_beacon_cb_t)(uint8 *src, uint8 lqi,uint8 *pdata, uint32 len);
typedef void 	(*af_rec_annouce_cb_t)(uint8*src,uint8 *pdata, uint8 len);
typedef void    (*af_rec_login_state_cb_t)(uint8 state);

typedef void 	(*af_req_permit_join_cb_t)(uint8 state, uint16 duration);
typedef void 	(*af_req_del_port_cb_t)(uint8 port);
typedef uint8 	(*af_req_read_lqi_cb_t)(uint8 port);
typedef void 	(*af_rsp_read_time_cb_t)(uint8 state,uint8 zone, uint32 unix_time);
typedef void	(*af_req_update_cb_t)(Address_t *src,char *ip, uint16 port, char *url);

typedef void    (*read_file_cb_t)(uint8 *src,uint8 state, char *file_name, uint8 *pdata, uint32 len);
typedef void    (*write_file_cb_t)(uint8 *src, uint8 state, char *file_name);
typedef void    (*rename_file_cb_t)(uint8 *src, uint8 state, char *file_name);
typedef void    (*delete_file_cb_t)(uint8 *src, uint8 state, char *file_name);


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
	read_file_cb_t 		read_file_cb;
	write_file_cb_t		write_file_cb;
	rename_file_cb_t	rename_file_cb;
	delete_file_cb_t	delete_file_cb;
}AfFsListener_t;

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
void 	af_set_fs_listener(AfFsListener_t *listener);

sint8   af_set_describe(AfDescribe_t *desc);
sint8 	af_register_port(uint8 port_num,uint32 applicationID, uint32 *attr_list, uint8 attr_num, uint8 force);
sint8 	af_req_send_annouce(uint8 *pdata, uint32 len);
sint8   af_req_send_beacon(uint8 keyID, uint8 *addr, uint8 *pdata, uint32 len);
char*   af_read_hardware_info(void);

void af_update_hardware_cb(Address_t *src,char *ip, uint16 port, char *url);

#endif

