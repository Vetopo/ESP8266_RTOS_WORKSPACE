#include "eloop.h"
#include "pluto_stack_config.h"
#include "pluto.h"
#include "aps.h"
#include "af.h"
#include "attribute_id.h"
#include "application_id.h"
#include "pluto_update.h"
#include "app_data.h"

const uint32 device_attribute_id[] = {
	Attribute_ID_DeviceInfo,//
	Attribute_ID_PortList,
	Attribute_ID_PortDescribe,
	Attribute_ID_Scene,
	Attribute_ID_WhiteTable,
	Attribute_ID_AlarmRecord,
	Attribute_ID_HistoryRecord,
	Attribute_ID_Beacon,
	Attribute_ID_DeviceIndication,
	Attribute_ID_Upgrade,
	Attribute_ID_LQI,
};

#define DEVICE_ATTRIBUTE_ID_NUM	(sizeof(device_attribute_id)/sizeof(device_attribute_id[0]))

const AfDescribe_t af_describe =
{
		.version = 1,
		.MFG = "2017-11-8 15:00:00",
		.device_name = "switch",
		.device_type = dev_type_device,
		.guestLock= ES_FALSE,
		.shareLock= ES_FALSE,
		.localLock= ES_FALSE,
		.remoteLock= ES_FALSE,
		.zone = 8,
		.aIDNum = DEVICE_ATTRIBUTE_ID_NUM,
		.aID = (uint32*)device_attribute_id
};

const FirmwareInfo_t firmware_info = 
{
	.version = 2,
	.bin_name="esp8266_switch.bin",
	.chip_name="esp8266",
	.ota_support="yes",
	.compile_date=__DATE__","__TIME__
};
extern void app_login_state_cb (uint8 state);
const AfListener_t  af_dapter =
{
    .rec_beacon_cb = NULL,
	.rec_annouce_cb = NULL,
	.login_state_cb = app_login_state_cb,
    .permit_join_cb = NULL,
    .delete_port_cb = NULL,
    .read_lqi_cb = NULL,
    .req_update_cb = pluto_update_cb
};