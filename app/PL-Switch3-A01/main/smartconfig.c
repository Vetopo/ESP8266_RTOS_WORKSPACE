#include "eloop.h"
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"
#include "esp_smartconfig.h"
#include "smartconfig_ack.h"
#include "aps.h"
#include "pluto.h"
#include "hal_led.h"
void app_set_wifi_mode(wifi_mode_t mode);
/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
static const int CONNECTED_BIT = BIT0;
static const int ESPTOUCH_DONE_BIT = BIT1;
static const char *TAG = "sc";
static const uint8 sc_data[]={0xFD,0x01,0xFE,0xFC,0x00,0x10,0x00,0x01,0x00,0x00,0x00,0x53,0x00,0x00,0x10,0x02,0x7B,\
0x22,0x64,0x65,0x76,0x69,0x63,0x65,0x49,0x6E,0x66,0x6F,0x22,0x3A,0x7B,0x22,0x64,0x65,0x76,\
0x69,0x63,0x65,0x54,0x79,0x70,0x65,0x22,0x3A,0x22,0x67,0x68,0x5F,0x39,0x65,0x32,0x63,0x66,\
0x66,0x33,0x64,0x66,0x61,0x35,0x31,0x22,0x2C,0x22,0x64,0x65,0x76,0x69,0x63,0x65,0x49,0x64,\
0x22,0x3A,0x22,0x31,0x32,0x32,0x34,0x37,0x35,0x22,0x7D,0x7D};
void smartconfig_example_task(void * parm);
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    system_event_info_t *info = &event->event_info;
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        xTaskCreate(smartconfig_example_task, "smartconfig_example_task", 4096, NULL, 3, NULL);
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(s_wifi_event_group, CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        eloop_log(1, "Disconnect reason : %d", info->disconnected.reason);
        if (info->disconnected.reason == WIFI_REASON_BASIC_RATE_NOT_SUPPORT) {
            /*Switch to 802.11 bgn mode */
            esp_wifi_set_protocol(ESP_IF_WIFI_STA, WIFI_PROTOCAL_11B | WIFI_PROTOCAL_11G | WIFI_PROTOCAL_11N);
        }
        esp_wifi_connect();
        xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}
static void sc_callback(smartconfig_status_t status, void *pdata);
void initialise_wifi(void)
{
    tcpip_adapter_init();
	wifi_config_t	sta_conf;
	s_wifi_event_group = xEventGroupCreate();
    esp_event_loop_init(event_handler, NULL);
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    app_set_wifi_mode(WIFI_MODE_APSTA);//WIFI_MODE_AP    WIFI_MODE_APSTA WIFI_MODE_STA
    ESP_ERROR_CHECK( esp_wifi_start());
	ESP_ERROR_CHECK(esp_wifi_get_config(ESP_IF_WIFI_STA,&sta_conf));
	if(0)//((strlen((char*)sta_conf.sta.ssid)==0)||(strlen((char*)sta_conf.sta.password)==0))
	{
        app_set_wifi_mode(WIFI_MODE_STA);
	    ESP_ERROR_CHECK( esp_smartconfig_set_type(SC_TYPE_AIRKISS) );
	    ESP_ERROR_CHECK( esp_smartconfig_start(sc_callback) );
	    hal_led_blink(0x7FFFFFFF,100,100);
	}
	else
	{
		ESP_LOGI(TAG, "Connect to AP :%s @ %s \r\n",sta_conf.sta.ssid,sta_conf.sta.password);
		ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA,&sta_conf));
		ESP_ERROR_CHECK(esp_wifi_connect());
		hal_led_blink(0x7FFFFFFF,1000,1000);
	}
}

static void sc_callback(smartconfig_status_t status, void *pdata)
{
    uint8 rand_num;
    switch (status) {
        case SC_STATUS_WAIT:
            ESP_LOGI(TAG, "SC_STATUS_WAIT");
            break;
        case SC_STATUS_FIND_CHANNEL:
            ESP_LOGI(TAG, "SC_STATUS_FINDING_CHANNEL");
            break;
        case SC_STATUS_GETTING_SSID_PSWD:
            ESP_LOGI(TAG, "SC_STATUS_GETTING_SSID_PSWD");
            break;
        case SC_STATUS_LINK:
            ESP_LOGI(TAG, "SC_STATUS_LINK");
            wifi_config_t *wifi_config = pdata;
            ESP_LOGI(TAG, "SSID:%s", wifi_config->sta.ssid);
            ESP_LOGI(TAG, "PASSWORD:%s", wifi_config->sta.password);
            ESP_ERROR_CHECK( esp_wifi_disconnect() );
            app_set_wifi_mode(WIFI_MODE_APSTA);
            ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, wifi_config) );
            ESP_ERROR_CHECK( esp_wifi_connect() );
            sc_ack_send_info((uint8*)sc_data,sizeof(sc_data));
            break;
        case SC_STATUS_LINK_OVER:
            ESP_LOGI(TAG, "SC_STATUS_LINK_OVER");
            if (pdata != NULL) {
                sc_callback_data_t *sc_callback_data = (sc_callback_data_t *)pdata;
                switch (sc_callback_data->type) {
                    case SC_ACK_TYPE_ESPTOUCH:
                        ESP_LOGI(TAG, "Phone ip: %d.%d.%d.%d", sc_callback_data->ip[0], sc_callback_data->ip[1], sc_callback_data->ip[2], sc_callback_data->ip[3]);
                        ESP_LOGI(TAG, "TYPE: ESPTOUCH");
                        break;
                    case SC_ACK_TYPE_AIRKISS:
                        ESP_LOGI(TAG, "TYPE: AIRKISS");
                        break;
                    default:
                        ESP_LOGE(TAG, "TYPE: ERROR");
                        break;
                }
            }
            xEventGroupSetBits(s_wifi_event_group, ESPTOUCH_DONE_BIT);
            rand_num = eloop_get_timestamp()&0x00FF;
            af_req_send_annouce(&rand_num,1);
            hal_led_blink(0x7FFFFFFF,1000,1000);
            break;
        default:
            break;
    }
}

void smartconfig_example_task(void * parm)
{
    EventBits_t uxBits;
    //ESP_ERROR_CHECK( esp_smartconfig_set_type(SC_TYPE_ESPTOUCH) );
    //ESP_ERROR_CHECK( esp_smartconfig_start(sc_callback) );
    while (1) {
        uxBits = xEventGroupWaitBits(s_wifi_event_group, CONNECTED_BIT | ESPTOUCH_DONE_BIT, true, false, portMAX_DELAY); 
        if(uxBits & CONNECTED_BIT) {
            ESP_LOGI(TAG, "WiFi Connected to ap");
        }
        if(uxBits & ESPTOUCH_DONE_BIT) {
            ESP_LOGI(TAG, "smartconfig over");
            esp_smartconfig_stop();
            vTaskDelete(NULL);
        }
    }
}
