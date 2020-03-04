/*
 * Hal_LED.c
 *
 *  Created on: 2014-8-29
 *      Author: Administrator
 */

#include "eloop.h"
#include "hal_led.h"
#include "driver/gpio.h"

#define DBG_HAL_LED		0

struct Hal_led_t Led;

#define GPIO_LED_NUM    15//23
#define GPIO_LED_MASK	(1<<GPIO_LED_NUM)

static void hal_led_task(uint8 sig);

void hal_led_init(void)
{
	eloop_log(DBG_HAL_LED,"hal_led_init: \r\n");
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_LED_MASK;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
	hal_led_set(LED_STATUS_OFF);
}
void hal_led_set(uint8 val)
{
	eloop_log(DBG_HAL_LED,"hal_led_set: state:%d  \r\n",val);
	Led.lastStat = val;
	gpio_set_level(GPIO_LED_NUM,val);
}
void hal_led_blink(int blinkNum, int blinkTimeH, int blinkTimeL)
{
	eloop_log(DBG_HAL_LED,"hal_led_blink:num:%d h:%d l:%d \r\n",blinkNum,blinkTimeH,blinkTimeL);
	Led.status = LED_STATUS_OFF;
	Led.blinkNum=blinkNum;
	if(blinkTimeH==0)
		blinkTimeH=DEFAULT_BLINK_TIME_H;
	if(blinkTimeL==0)
		blinkTimeL=DEFAULT_BLINK_TIME_L;
	Led.blinkTimeH=(blinkTimeH);
	Led.blinkTimeL=(blinkTimeL);
	if(Led.blinkNum)
	{
		if(Led.status==LED_STATUS_OFF)
			eloop_start_timer(hal_led_task,1,Led.blinkTimeL,ES_FALSE);
		else
			eloop_start_timer(hal_led_task,1,Led.blinkTimeH,ES_FALSE);
	}
	else
	{
		eloop_start_timer(hal_led_task,1,10,ES_FALSE);
	}
}
static void hal_led_task(uint8 sig)
{
	if(Led.blinkNum>0)
	{
		if(Led.status==LED_STATUS_OFF)
		{
			//eloop_log(DBG_HAL_LED,"hal_led_task: high num:%d \r\n",Led.blinkNum);
			gpio_set_level(GPIO_LED_NUM,LED_STATUS_ON);
			eloop_start_timer(hal_led_task,1,Led.blinkTimeH,ES_FALSE);
		}
		else
		{
			//eloop_log(DBG_HAL_LED,"hal_led_task: low num:%d \r\n",Led.blinkNum);
			gpio_set_level(GPIO_LED_NUM,LED_STATUS_OFF);
			Led.blinkNum--;
			eloop_start_timer(hal_led_task,1,Led.blinkTimeL,ES_FALSE);
		}
		Led.status = !Led.status;
	}
	else
		gpio_set_level(GPIO_LED_NUM,Led.lastStat);
}