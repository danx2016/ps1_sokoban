#ifndef _SCENE_INIT_H
#define _SCENE_INIT_H

#include "scene.h"

extern Scene* scene_init_create(void);

extern void scene_init_init(void);
extern void scene_init_on_enter(void);
extern void scene_init_on_exit(void);
extern void scene_init_on_transition_finished(void);
extern void scene_init_fixed_update(void);
extern void scene_init_render(void);

#endif /* _SCENE_INIT_H */