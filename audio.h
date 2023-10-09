#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <stdint.h>

#define SOUND_START_CHANNEL 10

#define SOUND_ID_DROP        7
#define SOUND_ID_ERROR       8
#define SOUND_ID_MOVE        9
#define SOUND_ID_SELECT     10
#define SOUND_ID_START      11

#define MUSICS_NUM                  8

#define MUSIC_ID_TITLE              1
#define MUSIC_ID_PLAYING            2
#define MUSIC_ID_LEVEL_CLEARED      3
#define MUSIC_ID_ALL_LEVELS_CLEARED 4

// vol 0~9
extern int32_t audio_music_volume;
extern int32_t audio_sfx_volume;

extern void audio_init(void);
extern void audio_add_music(size_t music_id, uint8_t *music_data);
extern void audio_play_music(uint8_t music_id);
extern void audio_stop_music(void);
extern void audio_pause_music(void);
extern void audio_resume_music(void);

extern void audio_play_sound(uint8_t sound_id);

extern void audio_set_music_volume(int32_t music_volume);
extern void audio_set_sfx_volume(int32_t sfx_volume);

#endif /* _AUDIO_H_ */