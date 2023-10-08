#ifndef _SCENE_LEVELS_H
#define _SCENE_LEVELS_H

#include "scene.h"

extern Scene* scene_levels_create(void);

extern void scene_levels_init(Scene *scene);
extern void scene_levels_on_enter(Scene *scene);
extern void scene_levels_on_exit(Scene *scene);
extern void scene_levels_on_transition_finished(Scene *scene);
extern void scene_levels_fixed_update(Scene *scene);
extern void scene_levels_render(Scene *scene);

#endif /* _SCENE_LEVELS_H */