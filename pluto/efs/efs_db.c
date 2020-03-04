/*
 * efs_db.c
 *
 *  Created on: 20200201
 *      Author: luyunyi
 */

#include "efs_config.h"
#include "efs.h"
#include "efs_db.h"
#include "cJSON.h"

#if ENABLE_EFS

typedef struct
{
	char 	fname[EFS_NAME_MAX_LENGTH];
	cJSON	*root;
	u_int8	udpate_flag;
}efs_db_t;

static efs_db_t	efs_db;
static char temp_fname[EFS_NAME_MAX_LENGTH+1];

static int _db_safe_open(char *db);
static int _db_flush_data(void);
static int _db_safe_load(char *db);
static int _db_load_database(char *db);
static void _db_delay_update(void);
static void _db_timer(u_int8 sig);
static char *_db_gen_temp_file_name(char *fname);

int efs_db_save_string(char *db, char *key, char *value)
{
	int ret = ES_FAILED;
	eloop_log(DBG_EFS,"efs_db_save_string\r\n");
	if(_db_safe_open(db)==ES_SUCCEED)
	{
		cJSON_DeleteItemFromObject(efs_db.root,key);
		cJSON_AddStringToObject(efs_db.root,key,value);
		_db_delay_update();
		ret = ES_SUCCEED;
	}
	return ret;
}
int efs_db_save_block(char *db, char *key, void *block, int block_size)
{
	int ret = ES_FAILED;
	char *pbuf;
	int slen = (block_size*2)+2;
	eloop_log(DBG_EFS,"efs_db_save_stream\r\n");
	if((pbuf = (char*)eloop_malloc(slen))!=NULL)
	{
		eloop_bytes2hexstr(pbuf,'\0',(u_int8*)block,block_size);
		ret = efs_db_save_string(db,key,pbuf);
		eloop_free(pbuf);
	}
	return ret;
}
int efs_db_save_number(char *db, char *key, u_int32 value)
{
	int ret = ES_FAILED;
	eloop_log(DBG_EFS,"efs_db_save_number\r\n");
	if(_db_safe_open(db)==ES_SUCCEED)
	{
		cJSON_DeleteItemFromObject(efs_db.root,key);
		cJSON_AddNumberToObject(efs_db.root,key,value);
		_db_delay_update();
		ret = ES_SUCCEED;
	}
	return ret;
}
char *efs_db_read_string(char *db, char *key)
{
	cJSON *item;
	eloop_log(DBG_EFS,"efs_db_read_string\r\n");
	if((db==NULL)||(key==NULL))
	{
		return NULL;
	}
	if(_db_safe_open(db)==ES_SUCCEED)
	{
		if((item=cJSON_GetObjectItem(efs_db.root,key))!=NULL)
		{
			return item->valuestring;
		}
	}
	return NULL;
}
int efs_db_read_block(char *db, char *key, void *block, int block_size)
{
	char *str;
	int slen = 0;
	eloop_log(DBG_EFS,"efs_db_read_stream\r\n");
	if((db==NULL)||(key==NULL))
	{
		return 0;
	}
	if((str = efs_db_read_string(db,key))!=NULL)
	{
		slen = eloop_hex2bytes((u_int8*)block,block_size,str,'\0');
	}
	return slen;
}
u_int32 efs_db_read_number(char *db,char *key)
{
	cJSON *item;
	eloop_log(DBG_EFS,"efs_db_read_number\r\n");
	if((db==NULL)||(key==NULL))
	{
		return 0;
	}
	if(_db_safe_open(db)==ES_SUCCEED)
	{
		if((item=cJSON_GetObjectItem(efs_db.root,key))!=NULL)
		{
			return item->valueint;
		}
	}
	return 0;
}
int efs_db_count_element(char *db)
{
	cJSON *item;
	int count = 0;
	eloop_log(DBG_EFS,"efs_db_count_element\r\n");
	if(_db_safe_open(db)==ES_SUCCEED)
	{
		item = efs_db.root->child;
		while(item!=NULL)
		{
			item = item->next;
			count++;
		}
	}
	return count;
}
char *efs_db_get_key(char *db, int id)
{
	cJSON *item;
	int count = 0;
	eloop_log(DBG_EFS,"efs_db_count_element\r\n");
	if(_db_safe_open(db)==ES_SUCCEED)
	{
		item = efs_db.root->child;
		while(item!=NULL)
		{
			if(id==count)
			{
				return item->string;
			}
			item = item->next;
			count++;
		}
	}
	return NULL;
}
int efs_db_delete_value(char *db, char *key)
{
	int ret = ES_FAILED;
	eloop_log(DBG_EFS,"efs_db_delete_value\r\n");
	if(_db_safe_open(db)==ES_SUCCEED)
	{
		cJSON_DeleteItemFromObject(efs_db.root,key);
		_db_delay_update();
		ret = ES_SUCCEED;
	}
	return ret;
}
int efs_db_flush(void)
{
	eloop_log(DBG_EFS,"efs_db_flush\r\n");
	return _db_flush_data();
}
static char *_db_gen_temp_file_name(char *fname)
{
	temp_fname[0] = '~';
	eloop_strcpy(&temp_fname[1],fname);
	return temp_fname;
}
static int _db_safe_open(char *db)
{
	int ret = ES_SUCCEED;
	eloop_log(DBG_EFS,"_db_safe_open\r\n");
	if(efs_db.root==NULL)
	{
		ret = _db_safe_load(db);
	}
	else if(eloop_strcmp(db,efs_db.fname)!=0)
	{
		_db_flush_data();
		ret = _db_safe_load(db);
	}
	return ret;
}
static int _db_flush_data(void)
{
	char *str;
	int ret = ES_FAILED;
	eloop_log(DBG_EFS,"_db_flush_data:%s\r\n",efs_db.fname);
	if(efs_db.root!=NULL)
	{
		if(efs_db.udpate_flag==ES_TRUE)
		{
			if((str = cJSON_Print(efs_db.root))!=NULL)
			{
				_db_gen_temp_file_name(efs_db.fname);
				efs_write_file(temp_fname,0,(uint8*)str,eloop_strlen(str));
				efs_write_file(efs_db.fname,0,(uint8*)str,eloop_strlen(str));
				efs_delete_file(temp_fname);
				ret = ES_SUCCEED;
			}
			efs_db.udpate_flag = ES_FALSE;
		}
	}
	return ret;
}
static int _db_safe_load(char *db)
{
	int	ret = ES_FAILED;
	if((ret = _db_load_database(db))==ES_FAILED)
	{
		_db_gen_temp_file_name(db);
		if((ret = _db_load_database(temp_fname))==ES_SUCCEED)
		{
			eloop_strcpy(efs_db.fname,db);
			efs_db.udpate_flag = ES_TRUE;
			_db_flush_data();
		}
	}
	if(ret==ES_FAILED)
	{
		efs_db.root = cJSON_CreateObject();
		ret = ES_SUCCEED;
	}
	if(ret == ES_SUCCEED)
	{
		eloop_strcpy(efs_db.fname,db);
		efs_db.udpate_flag = ES_FALSE;
	}
	return ret;
}
static int _db_load_database(char *db)
{
	char 	*str;
	int 	slen,dlen;
	int		ret = ES_FAILED;
	eloop_log(DBG_EFS,"_db_load_database\r\n");
	if(efs_db.root!=NULL)
	{
		cJSON_Delete(efs_db.root);
		efs_db.root = NULL;
	}
	slen = efs_get_file_length(db);
	if(slen>0)
	{
		if((str = eloop_malloc(slen+1))!=NULL)
		{
			if((dlen = efs_read_file(db,0,(uint8*)str,slen))>0)
			{
				str[dlen] = '\0';
				if((efs_db.root = cJSON_Parse(str))!=NULL)
				{
					ret = ES_SUCCEED;
				}
			}
			eloop_free(str);
		}
	}
	return ret;
}
static void _db_delay_update(void)
{
	eloop_log(DBG_EFS,"_db_delay_update at:%d\r\n",eloop_get_timestamp());
	eloop_start_timer(_db_timer,1,EFS_DELAY_FLUSH_DB,ES_FALSE);
	efs_db.udpate_flag = ES_TRUE;
}
static void _db_timer(u_int8 sig)
{
	eloop_log(DBG_EFS,"_db_timer at:%d\r\n",eloop_get_timestamp());
	_db_flush_data();
}
#endif
