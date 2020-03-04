/*
 * main.c
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
#include "esp_partition.h"
#include "esp_ota_ops.h"
#include "eloop.h"

extern void initialise_wifi(void);
extern void app_task_init(void);
void app_main()
{
    const esp_partition_t *running = esp_ota_get_running_partition();
    eloop_log(3, "Running partition type %d subtype %d (offset 0x%08x)\r\n\r\n",
             running->type, running->subtype, running->address);
    initialise_wifi();
    app_task_init();
}
