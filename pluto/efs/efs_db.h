/*
 * efs_db.h
 *
 *  Created on: 20200201
 *      Author: luyunyi
 */

#ifndef _EFS_DB_H_
#define _EFS_DB_H_

#if ENABLE_EFS

int 	efs_db_save_string(const char *db, char *key, char *value);
int 	efs_db_save_block(const char *db, char *key, void *block, int block_size);
int 	efs_db_save_number(const char *db, char *key, u_int32 value);
char 	*efs_db_read_string(const char *db, char *key);
int 	efs_db_read_block(const char *db, char *key, void *block, int block_size);
u_int32 efs_db_read_number(const char *db,char *key);
int 	efs_db_count_element(const char *db);
char 	*efs_db_get_key(const char *db, int id);
int 	efs_db_delete_value(const char *db, char *key);
int 	efs_db_flush(void);

#endif

#endif /* SRC_EFS_EFS_DB_H_ */
