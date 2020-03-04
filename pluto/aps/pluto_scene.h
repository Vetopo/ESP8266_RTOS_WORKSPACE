#ifndef __PLUTO_SCENE_H__
#define __PLUTO_SCENE_H__

#include "eloop.h"
#include "cJSON.h"

sint8   aps_run_scene(char *task,char *name);
uint16  aps_count_scene(void);
char*   aps_get_scene_by_name(char *name);
char*   aps_get_scene_name_by_id(uint16 id);
sint8   aps_delete_scene(char *name);
sint8   aps_join_scene(char *tsk0, char *tsk1);

cJSON*  aps_make_scene_head(int version, char *boot_state);
cJSON*  aps_make_scene_loop(cJSON *body,char *reason);
cJSON*  aps_make_scene_if(cJSON *body, char *what, char *reason);
cJSON*  aps_make_scene_else(cJSON *body);
cJSON*  aps_make_scene_read_for(cJSON *body, char *reason);
void    aps_make_scene_action(cJSON *body, char *fuc, char *str);
cJSON*  aps_make_scene_exception(cJSON *body);
sint8   aps_save_scene(char *name,cJSON *psc);
sint8   aps_release_scene(cJSON *psc);

#endif


