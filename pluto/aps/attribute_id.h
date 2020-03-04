/*
 * attribute_id.h
 *
 *  Created on: 2019.1.1
 *      Author: Administrator
 */

#ifndef AID_H_
#define AID_H_

#include "eloop.h"

/*******PDO Attribute ID value define as follow: ***************/
#define Attribute_ID_ServerInfo				    0x000000 //
#define Attribute_ID_DeviceInfo					0x000001 //
#define	Attribute_ID_PortList					0x000002
#define Attribute_ID_PortDescribe				0x000003
#define Attribute_ID_Scene						0x000004
#define	Attribute_ID_File						0x000005
#define	Attribute_ID_WhiteTable					0x000006
#define	Attribute_ID_AlarmRecord				0x000007
#define	Attribute_ID_HistoryRecord				0x000008
#define	Attribute_ID_Beacon						0x00000F
#define Attribute_ID_DeviceIndication			0x000010
#define Attribute_ID_Time						0x000011
#define Attribute_ID_Upgrade					0x000012
#define Attribute_ID_MCMD						0x000013
#define Attribute_ID_MFC_Info					0x0000FF


#define Attribute_ID_SubDevice					0x000100
#define Attribute_ID_LQI						0x000102

#define Attribute_ID_Ethernet_Info				0x000104
#define Attribute_ID_WIFI_Info				    0x000105
#define Attribute_ID_GSM_Info                   0x000106
#define Attribute_ID_Zigbee_Info                0x000107
#define Attribute_ID_Bluetooth_Info             0x000108
#define Attribute_ID_Lora_Info                  0x000109
#define Attribute_ID_ZWave_Info                 0x00010A
#define Attribute_ID_Thread_Info                0x00010B

/*********PDO All Attribute Command option as follow *****************/
#define COMMAND_DEFUALT_OPTION						0x00
/*******************************************/

/*************ͨ������*********************************/
#define Attribute_ID_Option							0x00
////////////////////////////////////////////////////////////
#define Attribute_ID_KeyValue					0x009000

#define Attribute_ID_Switch						0x009020

#define Attribute_ID_Level						0x009030
#define Attribute_ID_Red						0x009031
#define Attribute_ID_Green						0x009032
#define Attribute_ID_Blue						0x009033
#define Attribute_ID_Yellow						0x009034
#define Attribute_ID_White						0x00903F
#define Attribute_ID_Color						0x009040

#define Attribute_ID_WindowSwitch				0x009050
#define Attribute_ID_WindowPercent				0x009051
#define Attribute_ID_BlindsWindow				0x009052

#define Attribute_ID_IRCode						0x009055
#define Attribute_ID_HXDIRCode					0x009056
#define Attribute_ID_RFCode						0x00905A

#define Attribute_ID_FanSwitch					0x009060
#define Attribute_ID_FanSpeedPercent			0x009061
#define Attribute_ID_FanHorizontalForward	 	0x009062
#define Attribute_ID_FanVerticalForward	 		0x009063
#define Attribute_ID_FanWaterSwitch				0x009068

#define Attribute_ID_Locker						0x009080
#define Attribute_ID_Locker_RFID				0x009081
#define Attribute_ID_Locker_PSW					0x009082
#define Attribute_ID_Locker_FINGER				0x009083


#define Attribute_ID_FireAlarm					0x00C000
#define Attribute_ID_SmokeAlarm					0x00C001
#define Attribute_ID_PM25Alarm					0x00C002

#define Attribute_ID_FloodAlarm					0x00C010
#define Attribute_ID_WaterAlarm					0x00C011

#define Attribute_ID_PoisonGasAlarm				0x00C020
#define Attribute_ID_CarbonMonoxideAlarm		0x00C021

#define Attribute_ID_CombustibleAlarm			0x00C030

#define Attribute_ID_MenciAlarm					0x00C100
#define Attribute_ID_HumenIRAlarm				0x00C110
#define Attribute_ID_EmergencyButton			0x00C120
#define Attribute_ID_LowPowerAlarm				0x00C130

#define Attribute_ID_DisassembleAlarm			0x00C400
#define Attribute_ID_WifiProbe					0x00C410

#define Attribute_ID_Alarm_Blink				0x00CFFD
#define Attribute_ID_Alarm_Voice				0x00CFFE
#define Attribute_ID_IAS_ZON_Alarm				0x00CFFF


#define Attribute_ID_TemperatureMeasurement				0x00D000
#define Attribute_ID_HumidityMeasurement				0x00D001
#define Attribute_ID_Illuminance_Measurement			0x00D003

#define Attribute_ID_PressureMeasurement				0x00D105
#define Attribute_ID_WeightMeasurement					0x00D120

#define Attribute_ID_TensionMeasurement					0x00D130

#define Attribute_ID_FlowMeasurement					0x00D160
#define Attribute_ID_EnergyMeasurement					0x00D170

#define Attribute_ID_PowerMeasurement					0x00D174
#define Attribute_ID_VoltageMeasurement					0x00D178
#define Attribute_ID_CurrentMeasurement					0x00D17C
#define Attribute_ID_Freq								0x00D180

#define Attribute_ID_Unkown						0x00FFFF

uint8 attr_get_data_type(uint32 aID);

#endif /* AID_H_ */
