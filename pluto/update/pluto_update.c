#include "eloop.h"
#include "pluto_update.h"
#include "pluto.h"
#include "pluto_adapter.h"
#include "cJSON.h"
#include "efs.h"
#include "esp_wifi.h"

static char *pWifiConfig = "wifi.cfg";
static char *pUpdateConfig = "update.cfg";

extern void esp_update_start(char *ip, int port, char *url);

static void esp_set_wifi_conf(char *ssid, char *psw);
static void esp_get_wifi_conf(char *ssid,char *psw);
static void restore_wifi_conf(void);

void pluto_update_cb(Address_t *src,char *ip, uint16 port, char *url)
{
    cJSON *root;
    char *str;
    root = cJSON_CreateObject();
    if(root!=NULL)
    {
        cJSON_AddStringToObject(root,"ip",ip);
        cJSON_AddNumberToObject(root,"port",port);
        cJSON_AddStringToObject(root,"url",url);
        cJSON_AddNumberToObject(root,"try",0);
        str = cJSON_Print(root);
        if(str!=NULL)
        {
            efs_write_text(pUpdateConfig,str);
            eloop_free(str);
        }
        cJSON_Delete(root);
        pluto_adapter_restart(3000);
    }
}
uint8 pluto_update_check(void)
{
    cJSON *root,*item;
    char *ip = NULL,*url = NULL;
    char *str;
    int port = 0;
    uint8 ret = ES_FAILED;
    uint8 stry;
    str = efs_read_text(pUpdateConfig);
    if(str!=NULL)
    {
        eloop_log(0,"pluto_update_check:%s\r\n",str);
        root = cJSON_Parse(str);
        eloop_free(str);
        if(root!=NULL)
        {
            ret = ES_SUCCEED;
            if((item = cJSON_GetObjectItem(root,"ip"))!=NULL)
            {
                ip = item->valuestring;           
            }
            else
            {
                ret = ES_FAILED;
            }
            if((item = cJSON_GetObjectItem(root,"port"))!=NULL)
            {
                port = item->valueint;
            }
            else{
                ret = ES_FAILED;
            }
            if((item = cJSON_GetObjectItem(root,"url"))!=NULL)
            {
                url = item->valuestring;
            }else{
                ret = ES_FAILED;
            }
            if((item = cJSON_GetObjectItem(root,"try"))!=NULL)
            {
                stry = item->valueint;
                if(stry<3)
                {
                    stry++;
                    cJSON_DeleteItemFromObject(root,"try");
                    cJSON_AddNumberToObject(root,"try",stry);
                }   
                else
                {
                    ret = ES_FAILED;
                }
            }else{
                ret = ES_FAILED;
            }
            if(ret==ES_SUCCEED)
            {
                if((str=cJSON_Print(root))!=NULL)
                {
                    eloop_log(0,"write update firmware info:%s\r\n",str);
                    efs_write_text(pUpdateConfig,str);
                    eloop_free(str);
                }
                esp_update_start(ip,port,url);
            }
            else
            {
                efs_delete_file(pUpdateConfig);
                restore_wifi_conf();
            }
            cJSON_Delete(root);
        }
    }
    return ret;
}
void pluto_update_save_wifi_conf(char *ssid, char *psw)
{
    cJSON *root;
    char *str;
    char s_ssid[32],s_psw[32];
    esp_get_wifi_conf(s_ssid,s_psw);
    root = cJSON_CreateObject();
    if(root!=NULL)
    {
        cJSON_AddStringToObject(root,"ssid",s_ssid);
        cJSON_AddStringToObject(root,"psw",s_psw);
        str = cJSON_Print(root);
        if(str!=NULL)
        {
            efs_write_text(pWifiConfig,str);
            eloop_free(str);
        }
        cJSON_Delete(root);
    }
    esp_set_wifi_conf(ssid,psw);
}
void esp_udpate_finished_cb(uint8 state)
{
    if(state==ES_SUCCEED)
    {
        eloop_log(1,"pluto udpate firmware succeed!\r\n\r\n");
        efs_delete_file(pUpdateConfig);
        restore_wifi_conf();
    }
}
static void restore_wifi_conf(void)
{
    cJSON *root,*item;
    char *ssid=NULL,*psw=NULL,*str;
    str = efs_read_text(pWifiConfig);
    if(str!=NULL)
    {
        if((root = cJSON_Parse(str))!=NULL)
        {
            if((item = cJSON_GetObjectItem(root,"ssid"))!=NULL)
            {
                ssid = item->valuestring;//wifi ssid;
            }
            if((item = cJSON_GetObjectItem(root,"psw"))!=NULL)
            {
                psw = item->valuestring;//wifi ssid;
            }
            esp_set_wifi_conf(ssid,psw);
            cJSON_Delete(root);
        }
        eloop_free(str);
        efs_delete_file(pWifiConfig);
    }
}
static void esp_set_wifi_conf(char *ssid, char *psw)
{
    wifi_config_t	sta_conf;
    if(ssid==NULL||psw==NULL)
     {   return;}
    eloop_memset(&sta_conf,0,sizeof(sta_conf));
	esp_wifi_get_config(ESP_IF_WIFI_STA,&sta_conf);
    eloop_strcpy(sta_conf.sta.ssid,ssid);
    eloop_strcpy(sta_conf.sta.password,psw);
    esp_wifi_set_config(ESP_IF_WIFI_STA,&sta_conf);
}
static void esp_get_wifi_conf(char *ssid,char *psw)
{
    wifi_config_t	sta_conf;
    eloop_memset(&sta_conf,0,sizeof(sta_conf));
	esp_wifi_get_config(ESP_IF_WIFI_STA,&sta_conf);
    eloop_strcpy(ssid,sta_conf.sta.ssid);
    eloop_strcpy(psw,sta_conf.sta.password);
}



