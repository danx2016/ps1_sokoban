#ifndef _SCENE_LEVEL_H
#define _SCENE_LEVEL_H

#include "scene.h"

extern Scene* scene_level_create(void);

extern void scene_level_init(void);
extern void scene_level_on_enter(void);
extern void scene_level_on_exit(void);
extern void scene_level_on_transition_finished(void);
extern void scene_level_fixed_update(void);
extern void scene_level_render(void);

#endif /* _SCENE_LEVEL_H */