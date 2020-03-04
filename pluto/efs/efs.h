/*
 * efs.h
 *
 *  Created on: 20200201
 *      Author: luyunyi
 */

#ifndef _EFS_H_
#define _EFS_H_

#include "efs_config.h"
#include "efs_sn.h"

#if ENABLE_EFS

char    *efs_read_text(char *fname);
int     efs_write_text(char *fname, char *str);
int 	efs_read_file(char *fname, int offset, u_int8 *buf, int len);
int  	efs_write_file(char *fname, int offset, u_int8 *pdata, int len);
int  	efs_get_file_length(char *fname);
char 	*efs_get_file_name(char *filter, int id);
int 	efs_delete_file(char *fname);
int 	efs_delete_all_file(void);
int 	efs_disk_format(void);
int		efs_rename(char *old_name, char *new_name);

#endif

#endif /* SRC_EFS_EFS_H_ */
