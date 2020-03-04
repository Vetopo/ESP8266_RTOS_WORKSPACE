#include "pluto_config.h"

#if(PLUTO_PLATFORM_ESP8266==1)

#include "eloop.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include <sys/socket.h>
#include <netdb.h>
#include "lwip/dns.h"
#include "pluto_adapter.h"
#include "efs.h"
#include "efs_db.h"

static uint8	buf32[8];
static void _port_reset_timer(uint8 sig);

#define MAC_SIZE    18  
#define IP_SIZE     16  

static set_server_ipv4_cb_t set_server_ipv4_cb = NULL;
// function declare  
static int _get_ip_by_domain(const char *domain, char *ip); // 根据域名获取ip  

uint8 *pluto_adapter_get_chip_id(void)
{ 
    eloop_memset(buf32,0,8);
    esp_wifi_get_mac(ESP_IF_WIFI_AP,&buf32[2]); 
	return buf32;
}
uint8 *pluto_adapter_get_default_address(void)
{
    eloop_memset(buf32,0,8);
    esp_wifi_get_mac(ESP_IF_WIFI_AP,&buf32[2]);
	return buf32;
}
uint8 pluto_adapter_get_lqi(void)
{
    return 0x66;
}
void pluto_adapter_get_ipv4_by_url(char *url,set_server_ipv4_cb_t cb)
{
    char ipv4[32];
    eloop_log(3,"pluto_adapter_get_ipv4_by_url:%s\r\n",url);
    set_server_ipv4_cb = cb;
    if(_get_ip_by_domain(url,ipv4)>=0)
    {
        eloop_log(3,"pluto_adapter_get_ipv4_by_url:%s-->%s\r\n",url,ipv4);
    }
}
uint32 pluto_adapter_get_local_ip(void)
{
    tcpip_adapter_ip_info_t ip_info;
    if(tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ip_info)==ESP_OK)
    {
        return ip_info.ip.addr;
    }
    return 0;
}
uint32 pluto_adapter_get_free_heap_size(void)
{
    return eloop_get_free_heap();
}
void pluto_adapter_restart(uint32 tim)
{
    if(tim>0)
    {
        eloop_start_timer(_port_reset_timer,1,tim,ES_FALSE);
    }
    else
    {
        esp_restart();
    }
}
void pluto_adapter_restart_abort(void)
{
    eloop_stop_timer(_port_reset_timer,1);
}
uint32 pluto_adapter_bytes_to_u32(uint8 *pdata)
{
    uint32 temp;
    temp = eloop_bytes_to_u32(pdata,CONVERT_TPYE_BIG_ENDIAN);
    return temp;
}
void pluto_adapter_u32_to_bytes(uint8 *pdata, uint32 val)
{
    eloop_u32_to_bytes(pdata,val,CONVERT_TPYE_BIG_ENDIAN);
}
uint32 pluto_adapter_bytes_to_u16(uint8 *pdata)
{
    uint32 temp;
    temp = eloop_bytes_to_u16(pdata,CONVERT_TPYE_BIG_ENDIAN);
    return temp;
}
void pluto_adapter_u16_to_bytes(uint8 *pdata, uint32 val)
{
    eloop_u16_to_bytes(pdata,val,CONVERT_TPYE_BIG_ENDIAN);
}
uint32 pluto_adapter_get_max_record_size(void)
{
    return (10*1024);
}
void pluto_adapter_reset_wifi_config(void)
{
    wifi_config_t sta_conf;
	eloop_log(1,"pluto_adapter_reset_wifi_config\r\n !");
	eloop_memset(&sta_conf,0,sizeof(wifi_config_t));
	esp_wifi_set_config(ESP_IF_WIFI_STA,&sta_conf);
}
void pluto_adapter_factory_reset(void)
{
    eloop_log(1,"pluto_adapter_factory_reset\r\n !");
    pluto_adapter_reset_wifi_config();
    efs_delete_all_file();
}
uint16 pluto_adapter_htons(uint16 val)
{
    return htons(val);
}
uint16 pluto_adapter_ntohs(uint16 val)
{
    return ntohs(val);
}
uint32 pluto_adapter_inet_addr(char *str)
{
    eloop_log(0,"pluto_adapter_inet_addr: addr:%s \r\n",str);
    return inet_addr(str);
}
char *pluto_adapter_ntoa(uint32 soc_addr)
{
    struct in_addr s_addr;
    eloop_log(0,"pluto_adapter_ntoa: addr:%x \r\n",soc_addr);
    s_addr.s_addr = soc_addr;
    return inet_ntoa(s_addr);
}
static void _port_reset_timer(uint8 sig)
{
    esp_restart();
}

// 根据域名获取ip  
static ip_addr_t 	dnsAddr;
static  void  _dns_found_cb(const char *name, const ip_addr_t *ipaddr, void *arg);
static int _get_ip_by_domain(const char *domain, char *ip)  
{  
    return dns_gethostbyname(domain,&dnsAddr,_dns_found_cb,NULL);
}  
static  void  _dns_found_cb(const char *name, const ip_addr_t *ipaddr, void *arg)
{
    char ipv4[32];
	ip4_addr_t *ip4;
	ip4 =(void*) &ipaddr->addr;
    if(ipaddr==NULL)
    {
    	eloop_log(3,"dns_foundCB: error! \r\n");
    }
    else
    {
		sprintf(ipv4,"%d.%d.%d.%d",
				*((uint8 *)&ip4->addr), *((uint8 *)&ip4->addr + 1),
				*((uint8 *)&ip4->addr + 2), *((uint8 *)&ip4->addr + 3));
        eloop_log(3,"dns_found:host:%s-->%s\r\n",name,ipv4);
        if(set_server_ipv4_cb!=NULL)
        {
            set_server_ipv4_cb(inet_addr(ipv4));
        }
    }
}

#endif

