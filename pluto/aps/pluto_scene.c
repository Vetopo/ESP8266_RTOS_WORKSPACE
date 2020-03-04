#include "pluto.h"
#include "cJSON.h"
#include "efs.h"

extern sint8    Scene_CreateTask(char *task,char *name,sint8 isboot);
extern int      Scene_CountTask(void);
extern cJSON*   Scene_GetTask(char *name);
extern char*    Scene_GetTaskNameByID(int id);
extern sint8    Scene_DelTask(char *name);
extern sint8    Scene_Join(char *tsk0, char *tsk1);

extern cJSON *Scene_CreatBody(void);
extern cJSON *Scene_AddWhile(cJSON *body, char *reason);
extern cJSON *Scene_Addif(cJSON *body, char *what, char *reason);
extern cJSON *Scene_AddReadFor(cJSON *body, char *reason);
extern cJSON *Scene_AddException(cJSON *body);
extern cJSON *Scene_AddElse(cJSON *body);
extern void Scene_AddAction(cJSON *body, char *fuc, char *str);

sint8 aps_run_scene(char *task,char *name)
{
    return Scene_CreateTask( task, name,ES_FALSE);
}

uint16  aps_count_scene(void)
{
    return Scene_CountTask();
}

char* aps_get_scene_by_name(char *name)
{
    cJSON *psc;
    char *str = NULL;
    if((psc = Scene_GetTask(name))!=NULL)
    {
        str = cJSON_Print(psc);
    }
    return str;
}

char *aps_get_scene_name_by_id(uint16 id)
{
    return Scene_GetTaskNameByID(id);
}

sint8 aps_delete_scene(char *name)
{
    return Scene_DelTask(name);
}

sint8 aps_join_scene(char *tsk0, char *tsk1)
{
    return Scene_Join( tsk0, tsk1);
}
cJSON *aps_make_scene_head(int version, char *boot_state)
{
    cJSON *root;
    char *ver = "1";
    if((root = Scene_CreatBody())!=NULL)
    {
        Scene_AddAction(root,"version",ver);
        Scene_AddAction(root,"state",boot_state);
    }
    return root;
}
cJSON *aps_make_scene_loop(cJSON *body,char *reason)
{
    return Scene_AddWhile(body, reason);
}
cJSON *aps_make_scene_if(cJSON *body, char *what, char *reason)
{
    return Scene_Addif( body, what, reason);
}
cJSON *aps_make_scene_else(cJSON *body)
{
    return Scene_AddElse(body);
}
cJSON *aps_make_scene_read_for(cJSON *body, char *reason)
{
    return Scene_AddReadFor(body, reason);
}
void aps_make_scene_action(cJSON *body, char *fuc, char *str)
{
    return Scene_AddAction(body, fuc, str);
}
cJSON *aps_make_scene_exception(cJSON *body)
{
    return Scene_AddException(body);
}
sint8 aps_save_scene(char *name,cJSON *psc)
{
    char *str;
    sint8 ret = ES_FAILED;
    if((str=cJSON_Print(psc))!=NULL)
    {
        ret = efs_write_text((const char*)name,str);
        ret = aps_run_scene(name,str);
        eloop_free(str);
    }
    return ret;
}
sint8 aps_release_scene(cJSON *psc)
{
    cJSON_Delete(psc);
    return ES_SUCCEED;
}


