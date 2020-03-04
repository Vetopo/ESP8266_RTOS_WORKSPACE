#include "eloop.h"
#include "driver/gpio.h"
#include "hal_key.h"

#define DBG_HAL_KEY		0

typedef struct
{
  uint8 key;
  uint8 state;
  uint8 shift;
  uint32 runtime;
}Button_t;

static Button_t Button;

static uint8 	_key_filter(uint8 key, int tick);
static void 	_key_reset_state(void);
static uint8 	_key_special_process(uint8 key,uint8 shift, uint8 ticks);
static void 	_key_scan(uint8 *key, uint8 *shift, int tick);

void  hal_key_init()
{
    gpio_config_t io_conf;
    eloop_memset(&io_conf,0,sizeof(io_conf));
    //interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_KEY_MASK0|GPIO_KEY_MASK1|GPIO_KEY_MASK2|GPIO_KEY_MASK3;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);
	_key_reset_state();
}
uint8 hal_key_direct_read(void)
{
	uint8 temp = 0x00;
	if(!(gpio_get_level(GPIO_KEY_NUM0)))
		temp|=KEY0;
	if(!(gpio_get_level(GPIO_KEY_NUM1)))
		temp|=KEY1;
	if(!(gpio_get_level(GPIO_KEY_NUM2)))
		temp|=KEY2;
	if(!(gpio_get_level(GPIO_KEY_NUM3)))
		temp|=KEY3;
	return temp;
}
int  hal_key_get_time(void)
{
  return Button.runtime;
}
uint8  hal_key_read(uint8 *val, int tick)
{
	uint8 shift;
	uint8 key;
	_key_scan(&key,&shift,tick);
	shift = _key_special_process(key,shift,tick);
	*val = key;
	return shift;
}
static void _key_scan(uint8 *key, uint8 *shift, int tick)
{
	uint8 temp;
	temp = hal_key_direct_read();
	temp = _key_filter(temp,tick);
	if(temp)
	{
		Button.key=temp;
		if(Button.state==0x00)
		{
			Button.state=0x01;
			Button.shift=KEY_SHIFT_DOWN;
			Button.runtime=0;
		}
		else
		{
			Button.runtime+=tick;
			Button.shift=KEY_SHIFT_KEEP;
		}
	}
	else
	{
		if(Button.state)
		{
			Button.state=0x00;
			Button.shift=KEY_SHIFT_UP;
		}
	}
	*key = Button.key;
	*shift = Button.shift;
  Button.shift = 0;
}

static uint8 _key_filter(uint8 key, int tick)
{
  static uint8 runtime = 0;
  static uint8 last_key = 0;
  if(last_key!=key)
  {
    last_key = key;
    runtime = 0;
  }
  if(runtime>=KEY_FILTER_TIME)
  {
    return key;
  }
  else
  {
    runtime+=tick;
    return 0;
  }
}
static uint8 _shift = 0;
static uint8 _hitcnt = 0;
static uint8 _lastkey = 0;
static uint16 _runtime = 0;
static void _key_reset_state(void)
{
  eloop_log(DBG_HAL_KEY,"_key_reset_state:%d\r\n",_hitcnt);
  _shift = 0;
  _hitcnt = 0;
  _lastkey = 0;
  _runtime = 0;
}
static uint8 _key_special_process(uint8 key,uint8 shift, uint8 ticks)
{
  if(_runtime>0)
  {
    if(_runtime>ticks)
    {
      _runtime -= ticks;
    }
    else
    {
      _runtime = 0;
      _key_reset_state();
    }
  }
  if(shift==KEY_SHIFT_KEEP)
  {
    if((hal_key_get_time()>=KEY_LONG_RUNTIME)&&(_shift!=KEY_SHIFT_LONG_KEEP))
    {
       _shift = KEY_SHIFT_LONG_KEEP;
       shift = KEY_SHIFT_LONG_KEEP;
       _lastkey = key;
       _runtime = KEY_HIT_TIMEOUT;
    }
  }
  else if(shift==KEY_SHIFT_DOWN)
  {
    if(_shift==KEY_SHIFT_LONG_KEEP)
    {
      _hitcnt++;
      eloop_log(DBG_HAL_KEY,"key press cnt:%d\r\n",_hitcnt);
    }
  }
  else if(shift==KEY_SHIFT_UP)
  {
    eloop_log(DBG_HAL_KEY,"key press cnt:%d\r\n",_hitcnt);
    switch(_hitcnt)
    {
    case 0x00://do nothing
      break;
    case 0x01://
      shift = KEY_SHIFT_LONG_HIT1;
      break;
    case 0x02://
      shift = KEY_SHIFT_LONG_HIT2;
      break;
    case 0x03://
      shift = KEY_SHIFT_LONG_HIT3;
      break;
    case 0x04://
	    shift = KEY_SHIFT_LONG_HIT4;
      break;
    case 0x08://
      shift = KEY_SHIFT_LONG_HIT8;
      break;
    }
    if(_lastkey != key)
    {
      _key_reset_state();
    }
  }
  if((shift==KEY_SHIFT_INVALID)&&(_shift==KEY_SHIFT_LONG_KEEP))
  {
    shift = KEY_SHIFT_LONG_BUSSY;
  }
  return shift;
}
