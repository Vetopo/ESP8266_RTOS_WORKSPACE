/*
 * pluto_config.h
 *
 *  Created on: 2020.2.10
 *      Author: luyunyi
 */
#ifndef __PLUTO_CONFIG_H__
#define __PLUTO_CONFIG_H__

#define     PLUTO_PLATFORM_ESP8266          1


#define     ENABLE_ATC_FUNCTION             0
#define     ENABLE_ZIGBEE_GATEWAY           0

typedef enum
{
	dev_type_invalide	= 0x00,
	dev_type_gateway 	= 0x01,
	dev_type_route		= 0x02,
	dev_type_device		= 0x03,
	dev_type_lowenergy	= 0x04,
}device_type_t;

#endif


