/*
 * efs_nvs.c
 *
 *  Created on: 2019.9.25
 *      Author: lort
 */

#include "eloop.h"
#include "efs_config.h"
#include "esp_system.h"
#include "esp_partition.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_nvs.h"

static s_int8 _nvs_init(void)
{
	esp_err_t err;
	static u_int8 init_flag = 0x00;
	eloop_log(DBG_EFS_NVS,"_nvs_init: \r\n");
	if(init_flag==0x00)
	{
		init_flag = 0x01;
	    err = nvs_flash_init();
	    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
	        // NVS partition was truncated and needs to be erased
	        const esp_partition_t* nvs_partition = esp_partition_find_first( ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, "nvs");
	        assert(nvs_partition && "partition table must have an NVS partition");
	        esp_partition_erase_range(nvs_partition, 0, nvs_partition->size);
	        eloop_log(DBG_EFS_NVS,"_nvs_init: nvPartition label:%s, size:%d start addr:0x%08x \r\n",nvs_partition->label,nvs_partition->size,nvs_partition->address);
	        // Retry nvs_flash_init
	        err = nvs_flash_init();
	        if(err==ESP_OK)
	        {
	        	eloop_log(DBG_EFS_NVS,"_nvs_init: init nvs and format disk Ok \r\n");
	        	return ES_SUCCEED;
	        }
	    }
	    else if(err==ESP_OK)
	    {
	    	eloop_log(DBG_EFS_NVS,"_nvs_init: init nvs Ok \r\n");
	    	const esp_partition_t* nvs_partition1 = esp_partition_find_first( ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, "nvs");
	    	eloop_log(DBG_EFS_NVS,"_nvs_init: nvPartition label:%s, size:%d start addr:0x%08x \r\n",nvs_partition1->label,nvs_partition1->size,nvs_partition1->address);
	    }
	    return ES_FAILED;
	}
	else
		return ES_SUCCEED;
}

s_int8 efs_nvs_write(const char *file, u_int8 *pdata, u_int32 len)
{
	nvs_handle 	my_handle;
	esp_err_t 	err;
	_nvs_init();
	eloop_log(DBG_EFS_NVS,"efs_nvs_write: at %s\r\n",file);
	eloop_logb(DBG_EFS_NVS,NULL,pdata,len);
    err = nvs_open(file, NVS_READWRITE, &my_handle);
    if (err == ESP_OK)
    {
    	err = nvs_set_blob(my_handle,file,pdata,len);
    	err = nvs_commit(my_handle);
    	nvs_close(my_handle);
    	if(err==ESP_OK)
    	{
    		return ES_SUCCEED;
    	}
    	else
    	{
    		eloop_log(DBG_EERROR,"efs_nvs_write error at %s \r\n", file);
    	}
    }
    return ES_FAILED;
}
u_int32 efs_nvs_read(const char *file, u_int8 *pdata, u_int32 len)
{
	nvs_handle 	my_handle;
	size_t 		slen = 0;
	esp_err_t 	err;
	_nvs_init();
	eloop_log(DBG_EFS_NVS,"efs_nvs_read: at %s\r\n",file);
    err = nvs_open(file, NVS_READONLY, &my_handle);
    if (err == ESP_OK)
    {
    	err = nvs_get_blob(my_handle, file, NULL,&slen);//get block lenth
    	if(err==ESP_OK)
    	{
    		slen = (slen>len)?len:slen;
    		err = nvs_get_blob(my_handle, file, (char*)pdata,&slen);
    		if(err==ESP_OK)
    		{
    			eloop_logb(DBG_EFS_NVS,"efs_nvs_read:",pdata,slen);
    		}
    		else
    		{
    			slen = 0;
    		}
    	}
    	nvs_close(my_handle);
        eloop_log(0,"Error (%d) opening NVS handle! \r\n", err);
    }
    return slen;
}
u_int32 efs_nvs_length(const char *file)
{
	nvs_handle 	my_handle;
	size_t 		slen = 0;
	esp_err_t 	err;
	_nvs_init();
	eloop_log(DBG_EFS_NVS,"efs_nvs_length: at %s\r\n",file);
    err = nvs_open(file, NVS_READONLY, &my_handle);
    if (err == ESP_OK)
    {
    	slen = 0;
    	err = nvs_get_blob(my_handle, file, NULL,&slen);//get block lenth
    	if(err==ESP_OK)
    	{
    		eloop_log(1,"efs_nvs_read length:%d \r\n",slen);
    	}
    	else
    		slen = 0;
    	nvs_close(my_handle);
        eloop_log(0,"Error (%d) opening NVS handle! \r\n", err);
    }
    return slen;
}

