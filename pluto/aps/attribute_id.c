/*
 * aID.c
 *
 *  Created on: 2016��5��21��
 *      Author: lort
 */
#include "eloop.h"
#include "pluto_stack_config.h"
#include "pluto.h"
#include "attribute_id.h"

uint8 attr_get_data_type(uint32 aID)
{
	switch(aID)
	{
		case Attribute_ID_LQI:
		case Attribute_ID_Ethernet_Info:
		case Attribute_ID_KeyValue:
		case Attribute_ID_Switch:
		case Attribute_ID_Level:
		case Attribute_ID_Red:
		case Attribute_ID_Green:
		case Attribute_ID_Blue:
		case Attribute_ID_Yellow:
		case Attribute_ID_White:
		case Attribute_ID_Color:
		case Attribute_ID_WindowSwitch:
		case Attribute_ID_WindowPercent:
		case Attribute_ID_BlindsWindow:
		case Attribute_ID_IRCode:
		case Attribute_ID_HXDIRCode:
		case Attribute_ID_RFCode:
		case Attribute_ID_FanSwitch:
		case Attribute_ID_FanSpeedPercent:
		case Attribute_ID_FanHorizontalForward:
		case Attribute_ID_FanVerticalForward:
		case Attribute_ID_FanWaterSwitch:
		case Attribute_ID_Locker:
		case Attribute_ID_Locker_RFID:
		case Attribute_ID_Locker_PSW:
		case Attribute_ID_Locker_FINGER:
		case Attribute_ID_FireAlarm:
		case Attribute_ID_SmokeAlarm:
		case Attribute_ID_PM25Alarm:
		case Attribute_ID_FloodAlarm:
		case Attribute_ID_WaterAlarm:
		case Attribute_ID_PoisonGasAlarm:
		case Attribute_ID_CarbonMonoxideAlarm:
		case Attribute_ID_CombustibleAlarm:
		case Attribute_ID_MenciAlarm:
		case Attribute_ID_HumenIRAlarm:
		case Attribute_ID_EmergencyButton:
		case Attribute_ID_LowPowerAlarm:
		case Attribute_ID_DisassembleAlarm:
		case Attribute_ID_Alarm_Blink:
		case Attribute_ID_Alarm_Voice:
		case Attribute_ID_IAS_ZON_Alarm:
            return DTYPE_BYTE;
		case Attribute_ID_TemperatureMeasurement:
		case Attribute_ID_HumidityMeasurement:
		case Attribute_ID_Illuminance_Measurement:
		case Attribute_ID_PressureMeasurement:
		case Attribute_ID_WeightMeasurement:
		case Attribute_ID_TensionMeasurement:
		case Attribute_ID_FlowMeasurement:
		case Attribute_ID_EnergyMeasurement:
		case Attribute_ID_PowerMeasurement:
		case Attribute_ID_VoltageMeasurement:
		case Attribute_ID_CurrentMeasurement:
		case Attribute_ID_Freq:
			return DTYPE_FLOAT;
		default:
			return DTYPE_UNKOWN;
	}
}


