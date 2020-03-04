#ifndef __PLUTO_ADAPTER_H__
#define __PLUTO_ADAPTER_H__

#include "eloop.h"

typedef void	(*set_server_ipv4_cb_t)(uint32 ip);

uint8   *pluto_adapter_get_chip_id(void);
uint8   *pluto_adapter_get_default_address(void);
uint8   pluto_adapter_get_lqi(void);
void    pluto_adapter_get_ipv4_by_url(char *url,set_server_ipv4_cb_t cb);
uint32  pluto_adapter_get_local_ip(void);
uint32  pluto_adapter_get_free_heap_size(void);
void    pluto_adapter_restart(uint32 tim);
void    pluto_adapter_restart_abort(void);
uint32  pluto_adapter_bytes_to_u32(uint8 *pdata);
void    pluto_adapter_u32_to_bytes(uint8 *pdata, uint32 val);
uint32  pluto_adapter_bytes_to_u16(uint8 *pdata);   
void    pluto_adapter_u16_to_bytes(uint8 *pdata, uint32 val);
uint32  pluto_adapter_get_max_record_size(void);
void    pluto_adapter_reset_wifi_config(void);
void    pluto_adapter_factory_reset(void);

uint16  pluto_adapter_htons(uint16 val);
uint16  pluto_adapter_ntohs(uint16 val);
uint32  pluto_adapter_inet_addr(char *str);
char*   pluto_adapter_ntoa(uint32 soc_addr);

#endif
