#ifndef _SCENE_ALL_CLEARED_H
#define _SCENE_ALL_CLEARED_H

#include "scene.h"

extern Scene* scene_all_cleared_create(void);

extern void scene_all_cleared_init(Scene *scene);
extern void scene_all_cleared_on_enter(Scene *scene);
extern void scene_all_cleared_on_exit(Scene *scene);
extern void scene_all_cleared_on_transition_finished(Scene *scene);
extern void scene_all_cleared_fixed_update(Scene *scene);
extern void scene_all_cleared_render(Scene *scene);

#endif /* _SCENE_ALL_CLEARED_H */