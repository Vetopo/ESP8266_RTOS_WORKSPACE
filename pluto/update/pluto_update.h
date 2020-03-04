
#ifndef __PLUTO_UPDATE_H__
#define __PLUTO_UPDATE_H__

#include "eloop.h"
#include "pluto.h"

void pluto_update_cb(Address_t *src,char *ip, uint16 port, char *url);
void pluto_update_save_wifi_conf(char *ssid, char *psw);
uint8 pluto_update_check(void);

void esp_update_start(char *ip, int port, char *url);
void esp_udpate_finished_cb(uint8 state);

#endif

