/*
 * application_id.h
 *
 *  Created on: 2019.1.1
 *      Author: Administrator
 */

#ifndef __APPLICATION_ID_H__
#define __APPLICATION_ID_H__


/************��Ҫ�豸����*****************************/
#define Application_ID_Gateway						0x000001
#define Application_ID_Device						0x000002
#define Application_ID_Route						0x000010

#define Application_ID_Key							0x001000

#define Application_ID_Switch						0x001010

#define Application_ID_Light						0x001020
#define Application_ID_DimmerLight					0x001021
#define Application_ID_ColorLight					0x001023

#define Application_ID_86Socket						0x001030
#define Application_ID_LineSocket					0x001031

#define Application_ID_WidowCovering				0x001041
#define Application_ID_Blind_WidowCovering			0x001042
#define Application_ID_PercentWidowCovering			0x001045

#define Application_ID_Locker						0x001080

#define Application_ID_IRBox						0x001100
////////////////////////////////////////////////////////
#define Application_ID_Fan							0x002100
#define Application_ID_WatterRefrigeration			0x002101

#define Application_ID_TempController				0x002200
#define Application_ID_AirConditioning				0x002201
#define Application_ID_FanMachineController			0x002202
#define Application_ID_DihumidificationController	0x002203
#define Application_ID_MainPower_Outlet				0x003000
#define Application_ID_MainPower_RCCB				0x003001
#define Application_ID_MainPower_AirCircuitBreaker	0x003002

///////////////////////////////////////////////////////////
#define Application_ID_FireAlarm					0x004000
#define Application_ID_SmokeAlarm					0x004001
#define Application_ID_PM25Alarm					0x004002

#define Application_ID_FloodAlarm					0x004010
#define Application_ID_WaterAlarm					0x004011

#define Application_ID_PoisonGasAlarm				0x004020
#define Application_ID_CarbonMonoxideAlarm			0x004021

#define Application_ID_CombustibelAlarm				0x004030

#define Application_ID_DoorMagneticAlarm			0x004100
#define Application_ID_BodyInfraredAlarm			0x004110
#define Application_ID_EmergencyButtonAlarm			0x004120

#define Application_ID_PowerAlarm					0x004200
#define Application_ID_WifiProbe					0x004210
#define Application_ID_AlarmDevice					0x004FFE
#define Application_ID_IAS_ZONE						0x004FFF
///////////////////////////////////////////////////////////
#define Application_ID_HumidityAndTemSensor			0x005000
#define Application_ID_TemperatureSensor			0x005001
#define Application_ID_HumiditySensor				0x005002

#define Application_ID_CarbonMonoxideSensor			0x005010
#define Application_ID_CarbonDioxideSensor			0x005011
#define Application_ID_OxygenSensor					0x005012
#define Application_ID_NitrogenSensor				0x005013
#define Application_ID_OxygenAndNitrogenSensor		0x005014
#define Application_ID_OzoneSensor					0x005015

#define Application_ID_NaturalGasSensor				0x005020
#define Application_ID_MethaneSensor				0x005021

#define Application_ID_FormaldehydeSensor			0x005030

#define Application_ID_LocationSensor				0x005100
#define Application_ID_DistanceSensor				0x005101
#define Application_ID_HeightSensor					0x005102
#define Application_ID_AltitudeSensor				0x005104
#define Application_ID_PressureSensor				0x005105

#define Application_ID_AirPressureSensor			0x005110
#define Application_ID_WatterPressureSensor			0x005111
#define Application_ID_SimpleSensor					0x005112

#define Application_ID_GravitySensor				0x005120

#define Application_ID_IlluminanceSensor			0x005130

#define Application_ID_SpeedSensor					0x005140
#define Application_ID_AccelerationSensor			0x005141

#define Application_ID_WindSpeedSensor				0x005150

#define Application_ID_FlowSensor					0x005160

#define Application_ID_ElectricityMeter				0x005170
#define Application_ID_WattHourSensor				0x005171
#define Application_ID_VoltageSensor				0x005172
#define Application_ID_AmmeterSensor				0x005173



///////////////////////////////////////////////////////////
#define Application_ID_RFASKRemoteCtrl			0x007000
#define Application_ID_UART						0x007010

#define Application_ID_Unkown					0x00FFFF

#endif
