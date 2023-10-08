#ifndef _SCENE_INIT_H
#define _SCENE_INIT_H

#include "scene.h"

extern Scene* scene_init_create(void);

extern void scene_init_init(Scene *scene);
extern void scene_init_on_enter(Scene *scene);
extern void scene_init_on_exit(Scene *scene);
extern void scene_init_on_transition_finished(Scene *scene);
extern void scene_init_fixed_update(Scene *scene);
extern void scene_init_render(Scene *scene);

#endif /* _SCENE_INIT_H */