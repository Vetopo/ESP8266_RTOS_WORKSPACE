#include "efs_config.h"
#include "eloop.h"
#include "esp_nvs.h"

#define FLASH_SN_USE_FLAG		(0x5A5AA5A5)
#define FLASH_SN_PSW_LENGTH		32

const char *bk_sn0="SN0";
const char *bk_sn1="SN1";
const char *bk_sn2="SN2";

typedef struct
{
	uint32 	useflag;
	uint16	crc;
	uint16	len;
	uint8	sn[0];
}sn_table_t;


sint8 efs_nvs_write_sn(char *str)
{
	sn_table_t 	*psn;
	uint32		dlen;
	uint32 		slen;
	dlen = eloop_strlen(str);
	eloop_log(DBG_EFS,"write_sn:%s, length:%d \r\n",str,dlen);
	slen = sizeof(sn_table_t)+dlen;
	psn = (sn_table_t*)eloop_malloc(slen);
	if(psn!=NULL)
	{
		eloop_memset(psn,0,slen);
		eloop_log(DBG_EFS,"write_flash:\r\n ");
		psn->useflag = FLASH_SN_USE_FLAG;
		psn->len = dlen;
		psn->crc = eloop_get_crc16(0,(uint8*)str,dlen);
		eloop_memcpy(psn->sn,str,dlen);
		efs_nvs_write((char*)bk_sn0,(uint8*)psn,slen);
		efs_nvs_write((char*)bk_sn1,(uint8*)psn,slen);
		efs_nvs_write((char*)bk_sn2,(uint8*)psn,slen);
		eloop_free(psn);
		return ES_SUCCEED;
	}
	return ES_FAILED;
}
char* efs_nvs_read_sn(void)
{
	sn_table_t 	*psn = NULL;
	char 		*p;
	uint32 		slen;
	uint16		ret;
	uint16 		crc;
	uint16		i;
	for(i=0;i<3;i++)
	{
		switch(i)
		{
		case 0:
			p = (char*)bk_sn0;
			break;
		case 1:
			p = (char*)bk_sn1;
			break;
		default:
			p = (char*)bk_sn2;
			break;
		}
		slen = efs_nvs_length(p);
		psn = (sn_table_t *)eloop_malloc(slen);
		if(psn!=NULL)
		{
			ret = efs_nvs_read(p,(uint8*)psn,slen);
			if(ret>0)
			{
				if(psn->useflag==FLASH_SN_USE_FLAG)
				{
					crc = eloop_get_crc16(0,psn->sn,psn->len);
					if(crc==psn->crc)
					{
						eloop_log(DBG_EFS,"pluto_read_sn: malloc:%d, realize:%d\r\n",slen,psn->len);
						p = (char*)psn;
						i = psn->len;
						eloop_memcpy(p,psn->sn,i);
						p[i] = '\0';
						return p;
					}
				}
			}
			eloop_free(psn);
		}
	}
	return NULL;
}

