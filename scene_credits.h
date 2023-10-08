#ifndef _SCENE_CREDITS_H
#define _SCENE_CREDITS_H

#include "scene.h"

extern Scene* scene_credits_create(void);

extern void scene_credits_init(Scene *scene);
extern void scene_credits_on_enter(Scene *scene);
extern void scene_credits_on_exit(Scene *scene);
extern void scene_credits_on_transition_finished(Scene *scene);
extern void scene_credits_fixed_update(Scene *scene);
extern void scene_credits_render(Scene *scene);

#endif /* _SCENE_CREDITS_H */