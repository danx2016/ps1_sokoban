#ifndef _SCENE_CREDITS_H
#define _SCENE_CREDITS_H

#include "scene.h"

extern Scene* scene_credits_create(void);

extern void scene_credits_init(void);
extern void scene_credits_on_enter(void);
extern void scene_credits_on_exit(void);
extern void scene_credits_on_transition_finished(void);
extern void scene_credits_fixed_update(void);
extern void scene_credits_render(void);

#endif /* _SCENE_CREDITS_H */