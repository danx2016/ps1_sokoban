#ifndef _SCENE_LEVELS_H
#define _SCENE_LEVELS_H

#include "scene.h"

extern Scene* scene_levels_create(void);

extern void scene_levels_init(void);
extern void scene_levels_on_enter(void);
extern void scene_levels_on_exit(void);
extern void scene_levels_on_transition_finished(void);
extern void scene_levels_fixed_update(void);
extern void scene_levels_render(void);

#endif /* _SCENE_LEVELS_H */