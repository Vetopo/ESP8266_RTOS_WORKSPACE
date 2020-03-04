#ifndef HAL_KEY_H__
#define HAL_KEY_H__

#include "eloop.h"

#define KEY_FILTER_TIME   100

#define KEY0	0x01
#define KEY1	0x02
#define KEY2	0x04
#define KEY3	0x08

#define KEY_SHIFT_INVALID       0x00
#define KEY_SHIFT_DOWN          0x01
#define KEY_SHIFT_KEEP          0x02
#define KEY_SHIFT_UP            0x03
#define KEY_SHIFT_LONG_KEEP     0x04
#define KEY_SHIFT_LONG_HIT1     0x05
#define KEY_SHIFT_LONG_HIT2     0x06
#define KEY_SHIFT_LONG_HIT3     0x07
#define KEY_SHIFT_LONG_HIT4     0x08 
#define KEY_SHIFT_LONG_HIT8     0x09 
#define KEY_SHIFT_LONG_BUSSY    0x0A

#define KEY_LONG_RUNTIME        4000
#define KEY_HIT_TIMEOUT         16000

#define GPIO_KEY_NUM0   0//4//0
#define GPIO_KEY_MASK0	(1<<GPIO_KEY_NUM0)

#define GPIO_KEY_NUM1   2
#define GPIO_KEY_MASK1	(((uint64_t)1)<<GPIO_KEY_NUM1)

#define GPIO_KEY_NUM2   4
#define GPIO_KEY_MASK2	(((uint64_t)1)<<GPIO_KEY_NUM2)

#define GPIO_KEY_NUM3   5
#define GPIO_KEY_MASK3	(((uint64_t)1)<<GPIO_KEY_NUM3)

void    hal_key_init(void);
uint8   hal_key_direct_read(void); 
int     hal_key_get_time(void);
uint8   hal_key_read(uint8 *val, int tick);

#endif