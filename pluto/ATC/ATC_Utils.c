#include "ATC_Utils.h"
static uint8 Device_Type = ATC_TYPE_AS_DONGLE;

void ATC_SetDeviceType(uint8 type)
{
    Device_Type = type;
}
uint8 ATC_GetDeviceType(void)
{
    return Device_Type;
}