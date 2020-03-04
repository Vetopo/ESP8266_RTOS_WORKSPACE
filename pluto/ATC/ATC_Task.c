/*
 * ATC_Task.c
 *
 *  Created on: 2017��1��12��
 *      Author: lort
 */

#include "eloop.h"
#include "esp_wifi.h"
#include "ATC_Config.h"
#include "pluto_stack_config.h"
#include "pluto.h"
#include "pluto_adapter.h"
#include "efs.h"
#include "cJSON.h"
#include "ATC_Uart.h"
#include "ATC_Task.h"
#include "ATC_Lqi.h"
#include "ATC_UDP.h"
#include "ATC_Utils.h"
#include "af.h"
#include "pluto_adapter.h"
#include "pluto_update.h"

static void 	_response_device_info(void);
static void 	_response_service_info(void);
static void 	_write_service_info(uint8 *pdata, int len);
static void 	_respone_read_hardware_info(void);
static void 	_update_firmware(uint8 *pdata,int len);
static void 	_test_gpio(uint8 *pdata, int len);
static void 	_ATC_Task(uint32 event, void *arg, uint32 len);
extern void 	smart_set_event_cb(void);

void ATC_DelayInit(uint8 sig);
uint8 ATC_TaskInit(uint8 dev_type)
{
	static uint8 init_flag = 0x00;
	if(init_flag==0x00)
	{
		ATC_SetDeviceType(dev_type);
		eloop_create_task(_ATC_Task,2);
		ATC_Lqi_Init();
		if(dev_type==ATC_TYPE_AS_DEVICE)
		{
			ATC_Lqi_StartConnectWifi();
		}
		else{
			ATC_UartInit();
			eloop_start_timer_task(_ATC_Task,ATC_TASK_UART_POLL_EVENT,20,ES_TRUE);
			eloop_start_timer(ATC_DelayInit,0x01,1000,ES_FALSE);
		}
		eloop_start_timer(ATC_DelayInit,0x02,2000,ES_FALSE);
		init_flag = 0x01;
	}
	return ES_SUCCEED;
}

void ATC_DelayInit(uint8 sig)
{
	if(sig==0x01)
	{
		ATC_Lqi_StartConnectWifi();
	}	
	if(sig==0x02)
	{
		atc_socket_init(pluto_adapter_htons(PLUTO_ATC_SOCKET_PORT));
	}
	if(sig==0x03)
	{
		atc_udp_send_command(ATC_RPC_CMD_SREQ|ATC_RPC_SYS_SYS,ATC_SYS_READ_INFO,NULL,0);
	}
}
void  ATC_ReceiveMessageCB(uint8 type,uint8 cmd0, uint8 cmd1, uint8 *pdata, int len)
{
	eloop_log(DBG_FACTORY,"\r\n\r\nATC_ReceiveMessageCB :cmd0:%02x cmd1:%02x ",cmd0,cmd1);
	eloop_logb(DBG_FACTORY,"data:",pdata,len);
	if(type==ATC_MSG_TYPE_UART)
	{
		atc_udp_send_command(cmd0,cmd1,pdata,len);
	}
	else
	{
		if(ATC_GetDeviceType()==ATC_TYPE_AS_DONGLE)
		{
			if((cmd0&0xF0)==ATC_RPC_CMD_SRSP)
			{
				if(((cmd0&0x0F)==ATC_RPC_SYS_SYS)&&(cmd1==ATC_SYS_TEST_LQI))//pending rx lqi to recieve data and transport
				{
					ATC_LqiRecieveLqiProcess(cmd0,cmd1,pdata,len);
				}
				else
				{
					ATC_Uart_Write(cmd0,cmd1,pdata,len);
				}
			}
		}
		else
		{
			if((cmd0&0xF0)==ATC_RPC_CMD_SREQ)//only process request  command
			{
				if((cmd0&0x0F)==ATC_RPC_SYS_SYS)//only process system request layer
				{
					switch(cmd1)
					{
					case ATC_SYS_TEST:
						break;
					case ATC_SYS_RESET:
						pluto_adapter_restart(1000);
						break;
					case ATC_SYS_READ_INFO://request read device info
						_response_device_info();                                    
						break;
					case ATC_SYS_READ_SN://request read device serial number info of remote service
						_response_service_info();
						break;
					case ATC_SYS_WRITE_SN://request write device serial numer inof of remote service
						_write_service_info(pdata,len);
						break;
					case ATC_SYS_TEST_LQI:
						ATC_Lqi_ResponseSendSelfRssi();
						break;
					case ATC_SYS_TEST_GPIO:
						_test_gpio(pdata,len);
						break;
					case   ATC_SYS_READ_HARDWARE_INFO:
						_respone_read_hardware_info();
						break;
  					case ATC_SYS_UPDATE_FIRMWARE://9
						_update_firmware( pdata, len);
					  	break;
					}
				}
			}
		}
	}
}
static void _ATC_Task(uint32 event, void *arg, uint32 len)
{
	if(event&ATC_TASK_UART_POLL_EVENT)
	{
		ATC_UartPoll();
	}
}
static void  _response_device_info(void)
{
	char 	*text;
	cJSON 	*root;
	uint8 	chipID[8];
	char 	buf[64];
	text = (char*)efs_read_text(DEVICE_DESCRIBE_FILE_NAME);
	if(text!=NULL)
	{
		root = cJSON_Parse(text);
		eloop_free(text);
		if(root!=NULL)
		{
			eloop_memset(chipID,0,sizeof(chipID));
			eloop_memcpy(chipID,pluto_adapter_get_chip_id(),8);
			eloop_bytes2hexstr(buf,0x00,chipID,8);
			cJSON_AddStringToObject(root,"chip_id",buf);
			text = cJSON_Print(root);
			cJSON_Delete(root);
			atc_udp_send_command(ATC_RPC_CMD_SRSP|ATC_RPC_SYS_SYS,ATC_SYS_READ_INFO,(uint8*)text,eloop_strlen(text));
			eloop_free(text);
		}
	}
	else
	{
		eloop_log(DBG_FACTORY,"_response_device_info : read device info error !\r\n");
	}
	
}
static void  _response_service_info(void)
{
	char *info;
	info = (char*)efs_nvs_read_sn();
	if(info!=NULL)
	{
		atc_udp_send_command(ATC_RPC_CMD_SRSP|ATC_RPC_SYS_SYS,ATC_SYS_READ_SN,(uint8*)info,eloop_strlen(info));
		eloop_free(info);
	}
	else
	{
		eloop_log(DBG_FACTORY,"_response_service_info : read service info error !\r\n");
	}
}
static void  _write_service_info(uint8 *pdata, int len)
{
	char *str;
	str = eloop_data2str(pdata,len);
	if(str!=NULL){
		eloop_log(DBG_FACTORY,"_write_service_info:%s\r\n",str);
		efs_nvs_write_sn(str);
		eloop_free(str);
		_response_service_info();
	}
}
static void _respone_read_hardware_info(void)
{
	char *info;
	info = (char*)af_read_hardware_info();
	if(info!=NULL)
	{
		atc_udp_send_command(ATC_RPC_CMD_SRSP|ATC_RPC_SYS_SYS,ATC_SYS_READ_HARDWARE_INFO,(uint8*)info,eloop_strlen(info));
		eloop_free(info);
	}
	else
	{
		eloop_log(DBG_FACTORY,"_response_service_info : read service info error !\r\n");
	}
}

static void _update_firmware(uint8 *pdata,int len)
{
	Address_t src;
	cJSON *root,*item;
	char *ip = NULL,*url = NULL;
	char *ssid=NULL,*psw=NULL;
	uint16 port = 0;
	uint8 ret = ES_FAILED;
	eloop_log(3,"_update_firmware:%s\r\n",(char *)pdata);
	eloop_memset(&src,0,sizeof(Address_t));
	root = cJSON_Parse((char*)pdata);
	if(root!=NULL)
	{
		ret = ES_SUCCEED;
		if((item=cJSON_GetObjectItem(root,"wifi_ssid"))!=NULL)
		{
			ssid = item->valuestring;
			eloop_log(DBG_FACTORY,"[OTA]upgrade wifi ssid:%s \r\n",ssid);
		}
		else
		{
			ret = ES_FAILED;
		}
		if((item=cJSON_GetObjectItem(root,"wifi_psw"))!=NULL)
		{
			psw = item->valuestring;
			eloop_log(DBG_FACTORY,"[OTA]upgrade wifi psw:%s \r\n",psw);
		}
		else
		{
			ret = ES_FAILED;
		}
		if((item=cJSON_GetObjectItem(root,"server_ip"))!=NULL)
		{
			ip = item->valuestring;
			eloop_log(DBG_FACTORY,"[OTA]upgrade server ip:%s \r\n",ip);
		}
		else
		{
			ret = ES_FAILED;
		}
		if((item = cJSON_GetObjectItem(root,"server_port"))!=NULL)
		{
			port = item->valueint;
			eloop_log(DBG_FACTORY,"[OTA]upgrade server port:%d \r\n",port);
		}
		else
		{
			ret = ES_FAILED;
		}
		if((item = cJSON_GetObjectItem(root,"server_url"))!=NULL)
		{
			url = item->valuestring;
			eloop_log(DBG_FACTORY,"[OTA]upgrade server url:%s \r\n",url);
		}
		else
		{
			ret = ES_FAILED;
		}
		if(ret==ES_SUCCEED)
		{
			pluto_update_save_wifi_conf(ssid,psw);
			af_update_hardware_cb(&src,ip,port,url);
		}
		cJSON_Delete(root);
	}
	atc_udp_send_command(ATC_RPC_CMD_SRSP|ATC_RPC_SYS_SYS,ATC_SYS_UPDATE_FIRMWARE,(uint8*)&ret,1);
}
#include "driver/gpio.h"
static void _test_gpio(uint8 *pdata, int len)
{
	uint32 GPIO_MASK = (1<<0)|(1<<2)|(1<<3)|(1<<4)|(1<<5)|\
						(1<<12)|(1<<13)|(1<<14)|(1<<15)|(1<<16);
	gpio_config_t io_conf;
	uint32 gout,gin,i;
	uint8 buf[4];
    eloop_memset(&io_conf,0,sizeof(io_conf));
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_MASK;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
	gout = eloop_bytes_to_u32(pdata,CONVERT_TPYE_LITTILE_ENDIAN);
	for(i=0;i<32;i++)
	{
		if(GPIO_MASK&(1<<i))//process
		{
			if(gout&(1<<i))
			{
				gpio_set_level(i,0x01);
			}
			else
			{
				gpio_set_level(i,0x00);
			}
		}
	}
	gin = 0;
	for(i=0;i<32;i++)
	{
		if(GPIO_MASK&(1<<i))//process
		{
			if(gpio_get_level(i))
			{
				gin |= (1<<i);
			}
		}
	}
	eloop_u32_to_bytes(buf,gin,CONVERT_TPYE_LITTILE_ENDIAN);
	eloop_log(DBG_FACTORY,"_test_gpio test io mask:%08x, get io mask:%08x, len:%d\r\n",gout,gin,len);
	atc_udp_send_command(ATC_RPC_CMD_SRSP|ATC_RPC_SYS_SYS,ATC_SYS_TEST_GPIO,(uint8*)buf,4);
}
