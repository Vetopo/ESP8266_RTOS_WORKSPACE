#ifndef __EFS_SN_H__
#define __EFS_SN_H__

#include "eloop.h"

sint8   efs_nvs_write_sn(const char *str);
char*   efs_nvs_read_sn(void);

#endif
