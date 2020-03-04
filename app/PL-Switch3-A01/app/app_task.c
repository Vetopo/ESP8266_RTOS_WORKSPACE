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
#include "pluto.h"
#include "hal_led.h"
#include "hal_lamp.h"
#include "hal_key.h"
#include "app_server.h"
#include "app_data.h"
#include "app_onoff.h"
#include "ATC_Task.h"
#define KEY_SCAN_TICK		20

typedef enum
{
	APP_EVENT_KEYSCAN = 1,
	APP_EVENT_REGLED = 2,
}APP_EVENT;


const uint32 port_attribute_id[] = {Attribute_ID_Switch,Attribute_ID_LQI};
#define PORT_ATTRIBUTE_ID_NUM	(sizeof(port_attribute_id)/sizeof(port_attribute_id[0]))

static void _recieve_message_cb(Address_t *src,uint8 seq, Command_t *cmd, uint8 *pdata,uint32 len);
void app_task(uint32 evt,void *arg,uint32 len);

void app_task_init(void)
{
	ApsCmdListener_t	mylisten;
	eloop_init();
	eloop_create_task(app_task,1);
	pluto_init((FirmwareInfo_t*)&firmware_info);
	af_register_listener((AfListener_t*)&af_dapter);
	//app_write_server_info(); 
	hal_lamp_init();
	hal_key_init();
	hal_led_init();
	af_registe_led(hal_led_blink);
	mylisten.recieve_cmd_cb = _recieve_message_cb;
	mylisten.recieve_mcmd_cb = NULL;
	mylisten.send_cmd_cb = NULL;
	mylisten.send_mcmd_cb = NULL;
	aps_set_cmd_listener(&mylisten);
    af_set_device_describe((AfDescribe_t*)&af_describe);
    aps_register_port(1,Application_ID_Switch,(uint32*)port_attribute_id,PORT_ATTRIBUTE_ID_NUM,ES_FALSE);
    aps_register_port(2,Application_ID_Switch,(uint32*)port_attribute_id,PORT_ATTRIBUTE_ID_NUM,ES_FALSE);
    aps_register_port(3,Application_ID_Switch,(uint32*)port_attribute_id,PORT_ATTRIBUTE_ID_NUM,ES_FALSE);
	eloop_start_timer_task(app_task,APP_EVENT_KEYSCAN,KEY_SCAN_TICK,ES_TRUE);
}

void app_task(uint32 event,void *arg,uint32 len)
{
	Command_t 	cmd;
	uint8 		state;
	uint8 		key,shift;
	uint8		port = 0x00;
	if(event&APP_EVENT_KEYSCAN)
	{
		shift = hal_key_read(&key,KEY_SCAN_TICK);
		if(shift)
		{
			if(shift==KEY_SHIFT_DOWN)
			{
				eloop_log(1,"app_sig_keyscan: key:%02x shift:%02x  free:%d lqi:%d \r\n ",key,shift,eloop_get_free_heap(),pluto_adapter_get_lqi());
				cmd.aID 	= Attribute_ID_Switch;
				cmd.cmd_id 	= cmd_return;
				cmd.option = 0x00;
				switch(key)
				{
					case KEY3:
						port = 0x03;
					break;
					case KEY2:
						port = 0x02;
					break;
					case KEY1:
						port = 0x01;
					break;
				}
				if(port!=0x00)
				{
					state = app_read_on_off(port);
					state = !state;
					app_set_on_off(port,state);
					aps_req_report(port,aps_get_seq(),&cmd,&state,1,send_option_default);
				}
			}
			else if(shift==KEY_SHIFT_LONG_KEEP)
			{
				af_registe_led(NULL);
				hal_led_blink(4,500,500);
				eloop_start_timer_task(app_task,APP_EVENT_REGLED,(4*1000),ES_FALSE);
			}
			else if(shift==KEY_SHIFT_LONG_HIT4)
			{
				pluto_adapter_reset_wifi_config();
				pluto_adapter_restart(5000);
				hal_led_blink(8,200,200);
			}
			else if(shift==KEY_SHIFT_LONG_HIT8)
			{
				pluto_adapter_factory_reset();
				pluto_adapter_restart(1000);
				hal_led_blink(8,200,200);
			}
		}
	}
	if(event&APP_EVENT_REGLED)
	{
		af_registe_led(hal_led_blink);
	}
}
static void _recieve_message_cb(Address_t *src,uint8 seq, Command_t *cmd, uint8 *pdata,uint32 len)
{
	eloop_log(1,"_recieve_message_cb port:%d, aID:%08x cmd_id:%02x,\r\n",src->port,cmd->aID,cmd->cmd_id);
	eloop_log(1,"test_mem: free heap: %d\r\n",eloop_get_free_heap());
	if(src->port<4)
	{
		uint8 state;
		uint8 cmd_id = cmd->cmd_id;
		if(cmd->aID==Attribute_ID_Switch)
		{
			cmd->cmd_id |= cmd_return;
			if(cmd_id==cmd_write)
			{
				app_set_on_off(src->port,pdata[0]);
				aps_req_report(src->port,seq,cmd,pdata,1,send_option_default);
			}
			else if(cmd_id==cmd_read)
			{
				state = app_read_on_off(src->port);
				aps_req_send(src,seq,cmd,&state,1,send_option_default);
			}
		}
		else if(cmd->aID==Attribute_ID_LQI)
		{
			cmd->cmd_id |= cmd_return;
			if(cmd_id==cmd_read)
			{
				state = pluto_adapter_get_lqi();
				aps_req_send(src,seq,cmd,&state,1,send_option_default);
			}
		}
	}
}
void app_login_state_cb (uint8 state)
{
	static uint8 on_line = ES_FALSE;
	eloop_log(1,"_login_state_cb: state:%d \r\n !",state);
	if(state == LOGIN_STATE_ONLINE)
	{
		on_line = ES_TRUE;
		pluto_adapter_restart_abort();
	}
	else if(on_line==ES_TRUE)//if reconnect failed or reset chip
	{
		pluto_adapter_restart(5*60*1000);
	}
	switch(state)
	{
	case LOGIN_STATE_ONLINE:
		hal_led_blink(1,10,0);
		break;
	case LOGIN_STATE_OFFLINE:
		hal_led_blink(0x7FFFFFFF,1000,1000);
		break;
	default:
		hal_led_blink(0x7FFFFFFF,1000,1000);
		break;
	}
}
void app_set_wifi_mode(wifi_mode_t mode)
{
	esp_wifi_set_mode(mode);
}

