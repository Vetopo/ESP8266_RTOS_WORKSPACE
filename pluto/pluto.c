/*
 * pluto_service.c
 *
 *  Created on: 2019.9.1
 *      Author: lort
 */

#include "eloop.h"
#include "pluto_stack_config.h"
#include "socket.h"
#include "pluto.h"
#include "pluto_adapter.h"
#include "pluto_entry.h"
#include "NIB.h"
#include "hal_led.h"
#include "ATC_Task.h"
#include "pluto_update.h"

extern sint8 	pdo_write_server_info(PlutoServer_t *pserver);
extern void    	pdo_set_pluto_info(FirmwareInfo_t *info);
static void 	_start_timer(uint8 sig);

static led_blink_cb_t blink_led=NULL;
const static SocketAdapter_t Adapter={
		.open = socket_open,
		.close = socket_close,
		.output = socket_output
};

void pluto_init(FirmwareInfo_t *info)
{
	pluto_pre_init((SocketAdapter_t*)&Adapter);
	eloop_start_timer(_start_timer,1,500,ES_TRUE);
	pdo_set_pluto_info(info);
}
void pluto_deinit(void)
{
	pluto_pre_deinit();
	pluto_registe_led(NULL);
}
#include "pluto_update.h"
static void _start_timer(uint8 sig)
{
	uint32 local_ip;
	local_ip = pluto_adapter_get_local_ip();
	if (local_ip != 0)
	{
		if(pluto_update_check()==ES_FAILED)
		{
			pluto_start_server(pluto_adapter_ntoa(local_ip));
		}
		eloop_stop_timer(_start_timer,1);
	}
}

void pluto_registe_led(led_blink_cb_t cb)
{
	blink_led= cb;
}
void pluto_led_blink(int num, int htime, int ltime)
{
	if(login_get_state()==LOGIN_STATE_ONLINE)
	{
		if(blink_led!=NULL)
		{
			blink_led(num,htime,ltime);
		}
	}
}
sint8 pluto_write_server(PlutoServer_t *pserver)
{
	return pdo_write_server_info(pserver);
}

