/*
 * app_task.c
 *
 *  Created on: Apr 15, 2019
 *      Author: lort
 */
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

#include "eloop.h"
#include "pluto_stack_config.h"
#include "pluto.h"
#include "pluto_entry.h"
#include "pluto_adapter.h"
#include "ATC_Task.h"
void app_task_init(void)
{
	eloop_init();
	ATC_TaskInit(ATC_TYPE_AS_DONGLE);
}
void app_set_wifi_mode(wifi_mode_t mode)
{
	esp_wifi_set_mode(mode);
}

