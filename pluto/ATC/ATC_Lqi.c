#include "eloop.h"
#include "ATC_Config.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "ATC_Task.h"
#include "ATC_Uart.h"
#include "ATC_UDP.h"
#include "ATC_Utils.h"
#include "internal/esp_wifi_internal.h"


uint8 ATC_Lqi_Init(void)
{
	wifi_config_t			wifi_conf;
	esp_wifi_set_mode(WIFI_MODE_APSTA);
	eloop_memset(&wifi_conf,0,sizeof(wifi_conf));
	wifi_conf.ap.max_connection = 10;
	wifi_conf.ap.channel = 1;
	wifi_conf.ap.authmode = WIFI_AUTH_OPEN;
	if(ATC_GetDeviceType() == ATC_TYPE_AS_DEVICE)
	{
		eloop_strcpy(wifi_conf.ap.ssid,PLUTO_DEVICE_AP_SSID);
	}
	else
	{
		eloop_strcpy(wifi_conf.ap.ssid,PLUTO_DONGLE_AP_SSID);
	}
    eloop_strcpy(wifi_conf.ap.password,PLUTO_APSTA_PASSWORD);
    esp_wifi_set_config(ESP_IF_WIFI_AP,&wifi_conf);
    esp_wifi_start();
	esp_wifi_disconnect();
    return ES_SUCCEED;
}
void ATC_Lqi_StartConnectWifi(void)
{
	wifi_config_t			wifi_conf;
	eloop_log(DBG_FACTORY,"ATC_Lqi_StartConnectWifi \r\n");
	eloop_memset(&wifi_conf,0,sizeof(wifi_conf));
    if(ATC_GetDeviceType() == ATC_TYPE_AS_DEVICE)
    {
        eloop_strcpy(wifi_conf.sta.ssid,PLUTO_DEVICE_STA_SSID);
    }
    else
    {
        eloop_strcpy(wifi_conf.sta.ssid,PLUTO_DONGLE_STA_SSID);
    }
    eloop_strcpy(wifi_conf.sta.password,PLUTO_APSTA_PASSWORD);
	esp_wifi_set_config(ESP_IF_WIFI_STA,&wifi_conf);
	esp_wifi_connect();
}
void ATC_LqiRecieveLqiProcess(uint8 cmd0,uint8 cmd1,uint8 *pdata,int len)
{
	int rssi;
	uint8 lqi;
	if(cmd1==ATC_SYS_TEST_LQI)
	{
		rssi = esp_wifi_get_ap_rssi();
		lqi = (((90+rssi)*255)/91); 
		pdata[7] = lqi;
		eloop_log(3,"get rssi:%d,rxlqi:%d txlqi:%d\r\n",rssi,pdata[6],pdata[7]);
    	ATC_Uart_Write(ATC_RPC_CMD_SRSP|ATC_RPC_SYS_SYS,ATC_SYS_TEST_LQI,pdata,len);
	}
}
void ATC_Lqi_ReqSendSelfRssi(void)
{
	int rssi;
	uint8 lqi;
	uint8 buf[6+2];
	if(ATC_GetDeviceType()==ATC_TYPE_AS_DEVICE)
	{
		rssi = esp_wifi_get_ap_rssi();
		lqi = (((90+rssi)*255)/91); 
		eloop_log(3,"get rssi:%d,lqi:%d\r\n",rssi,lqi);
		esp_wifi_get_mac(ESP_IF_WIFI_AP,&buf[0]); 
		buf[6] = lqi;
		buf[7] = 0x00;
		atc_udp_send_command(ATC_RPC_CMD_AREQ|ATC_RPC_SYS_SYS,ATC_SYS_TEST_LQI,buf,sizeof(buf));
	}
}
void ATC_Lqi_ResponseSendSelfRssi(void)
{
	int rssi;
	uint8 lqi;
	uint8 buf[6+2];
	if(ATC_GetDeviceType()==ATC_TYPE_AS_DEVICE)
	{
		rssi = esp_wifi_get_ap_rssi();
		lqi = (((90+rssi)*255)/91); 
		eloop_log(3,"get rssi:%d,lqi:%d\r\n",rssi,lqi);
		esp_wifi_get_mac(ESP_IF_WIFI_AP,&buf[0]); 
		buf[6] = lqi;
		buf[7] = 0x00;
		atc_udp_send_command(ATC_RPC_CMD_SRSP|ATC_RPC_SYS_SYS,ATC_SYS_TEST_LQI,buf,sizeof(buf));
	}
}
