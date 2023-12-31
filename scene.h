#ifndef _SCENE_H
#define _SCENE_H

#include <stddef.h>
#include <stdint.h>

#define SCENE_MAX_NUM 8

#define SCENE_ID_INIT        0
#define SCENE_ID_TITLE       1
#define SCENE_ID_LEVEL       2 // this is the playing scene
#define SCENE_ID_LEVELS      3
#define SCENE_ID_CREDITS     4
#define SCENE_ID_ALL_CLEARED 5

typedef struct Scene Scene;

struct Scene
{
    void (*init)(void);
    void (*on_enter)(void);
    void (*on_exit)(void);
    void (*on_transition_finished)(void);
    void (*fixed_update)(void);
    void (*render_background)(void);
    void (*render)(void);
};

extern uint32_t scene_frame_count;

extern Scene *scene_create(void);

extern void scene_add(size_t scene_id, Scene *scene);
extern void scene_init_all(void);
extern void scene_switch_to(size_t scene_id);

extern void scene_fixed_update(void);
extern void scene_render(void);

#endif /* _SCENE_H */