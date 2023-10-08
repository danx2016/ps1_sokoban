#ifndef _SCENE_TITLE_H
#define _SCENE_TITLE_H

#include "scene.h"

extern Scene* scene_title_create(void);

extern void scene_title_init(Scene *scene);
extern void scene_title_on_enter(Scene *scene);
extern void scene_title_on_exit(Scene *scene);
extern void scene_title_on_transition_finished(Scene *scene);
extern void scene_title_fixed_update(Scene *scene);
extern void scene_title_render(Scene *scene);

#endif /* _SCENE_TITLE_H */