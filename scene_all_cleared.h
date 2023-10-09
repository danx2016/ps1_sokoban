#ifndef _SCENE_ALL_CLEARED_H
#define _SCENE_ALL_CLEARED_H

#include "scene.h"

extern Scene* scene_all_cleared_create(void);

extern void scene_all_cleared_init(void);
extern void scene_all_cleared_on_enter(void);
extern void scene_all_cleared_on_exit(void);
extern void scene_all_cleared_on_transition_finished(void);
extern void scene_all_cleared_fixed_update(void);
extern void scene_all_cleared_render(void);

#endif /* _SCENE_ALL_CLEARED_H */