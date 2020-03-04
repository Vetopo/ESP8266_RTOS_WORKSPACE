
#ifndef __ATC_CONFIG_H__
#define __ATC_CONFIG_H__

#include    "pluto_adapter.h"
#define     PLUTO_DONGLE_AP_SSID        "pluto_dongle"
#define     PLUTO_DONGLE_STA_SSID       "pluto_device"
#define     PLUTO_DEVICE_AP_SSID        "pluto_device"
#define     PLUTO_DEVICE_STA_SSID       "pluto_dongle"
#define     PLUTO_APSTA_PASSWORD        "1234567890"
#define     PLUTO_ATC_SOCKET_PORT       (10086)

typedef enum{
    ATC_MSG_TYPE_UART = 0,
    ATC_MSG_TYPE_WIFI = 1,

    ATC_TYPE_AS_DEVICE = 1,
    ATC_TYPE_AS_DONGLE = 0,
}_ATC_Type;

#endif

