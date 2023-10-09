#ifndef _SCENE_TITLE_H
#define _SCENE_TITLE_H

#include "scene.h"

extern Scene* scene_title_create(void);

extern void scene_title_init(void);
extern void scene_title_on_enter(void);
extern void scene_title_on_exit(void);
extern void scene_title_on_transition_finished(void);
extern void scene_title_fixed_update(void);
extern void scene_title_render(void);

#endif /* _SCENE_TITLE_H */