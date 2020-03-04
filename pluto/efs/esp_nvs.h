
/*
 * pluto_nvs.h
 *
 *  Created on: 2019.9.25
 *      Author: lort
 */

#ifndef PLUTO_NVS_H_
#define PLUTO_NVS_H_

#include "eloop.h"

s_int8  efs_nvs_write(char *file, u_int8 *pdata, u_int32 len);
u_int32 efs_nvs_read(char *file, u_int8 *pdata, u_int32 len);
u_int32 efs_nvs_length(char *file);

#endif /* JIFAN_JF_FS_JF_NVS_H_ */


