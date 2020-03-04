#include "eloop.h"
#include "pluto.h"
#include "aps.h"
#include "app_onoff.h"
#include "hal_lamp.h"

void  app_set_on_off(uint8 port, uint8 state)
{
	switch(port)
	{
	case 0x01:
		hal_lamp_set(LAMP0,state);
		break;
	case 0x02:
		hal_lamp_set(LAMP1,state);
		break;
	case 0x03:
		hal_lamp_set(LAMP2,state);
		break;
	case 0x04:
		hal_lamp_set(LAMP3,state);
		break;
	default:
		return ;
	}
}
uint8  app_read_on_off(uint8 port)
{
	uint8 state = 0x00;
	switch(port)
	{
	case 0x01:
		state = hal_lamp_get(LAMP0);
		break;
	case 0x02:
		state = hal_lamp_get(LAMP1);
		break;
	case 0x03:
		state = hal_lamp_get(LAMP2);
		break;
	case 0x04:
		state = hal_lamp_get(LAMP3);
		break;
	}
	return state;
}
