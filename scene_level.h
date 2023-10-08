#ifndef _SCENE_LEVEL_H
#define _SCENE_LEVEL_H

#include "scene.h"

extern Scene* scene_level_create(void);

extern void scene_level_init(Scene *scene);
extern void scene_level_on_enter(Scene *scene);
extern void scene_level_on_exit(Scene *scene);
extern void scene_level_on_transition_finished(Scene *scene);
extern void scene_level_fixed_update(Scene *scene);
extern void scene_level_render(Scene *scene);

#endif /* _SCENE_LEVEL_H */