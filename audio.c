#include <stdbool.h>
#include <stdio.h>
#include <libapi.h>

#include "res.h"
#include "audio.h"
#include "game.h"
#include "third_party/nugget/modplayer/modplayer.h"

int32_t audio_music_volume = 5;
int32_t audio_sfx_volume = 5;

static uint8_t *musics_data[MUSICS_NUM];

static uint32_t current_music_id = -1;

static bool is_music_paused;
static bool is_music_stopped;

#define BPM_TIME_CONV 140
#define BPM_TIME_INC  15000

static uint32_t bpm_time;

// invoked every ~0.015 secs
int32_t audio_play_next_sample()
{
    if (is_music_paused || is_music_stopped)
    {
        return 1;
    }

    bpm_time += BPM_TIME_INC;
    while (bpm_time >= MOD_BPM * BPM_TIME_CONV)
    {
        bpm_time -= MOD_BPM * BPM_TIME_CONV;
        MOD_Poll();
        //printf("audio order=%d pattern=%d row=%d\n", MOD_CurrentOrder, MOD_CurrentPattern, MOD_CurrentRow);

        // TODO: workaround to play music once (hardcoded for now)
        if (current_music_id == MUSIC_ID_LEVEL_CLEARED && MOD_CurrentRow == 63)
        {
            audio_stop_music();
        }

    }
    
    return 1;
}

void audio_init()
{
    MOD_Silence();

    is_music_paused = true;
    is_music_stopped = true;

    // setup the root counter 2 interruption handler to play 
    // mod music at correct timing regardless of NTSC/PAL video mode
    EnterCriticalSection();
    uint32_t ev = OpenEvent(RCntCNT2, EvSpINT, EvMdINTR, audio_play_next_sample);
    EnableEvent(ev);
    SetRCnt(RCntCNT2, 0xffff, RCntMdINTR);
    ExitCriticalSection();
    StartRCnt(RCntCNT2);

    audio_set_music_volume(2);
    audio_set_sfx_volume(9); 
}

void audio_add_music(size_t music_id, uint8_t *music_data)
{
    musics_data[music_id] = music_data;
}

void audio_stop_music()
{
    if (!is_music_stopped)
    {
        MOD_Silence();
        is_music_stopped = true;
        current_music_id = -1;
    }
}

void audio_pause_music()
{
    is_music_paused = true;
}

void audio_resume_music()
{
    is_music_paused = false;
}

void audio_play_music(uint8_t music_id)
{
    if (current_music_id == music_id)
    {
        return;
    }

    current_music_id = music_id;

    is_music_paused = false;
    is_music_stopped = false;

    MOD_Load((struct MODFileFormat*) musics_data[music_id]);
}

void audio_play_sound(uint8_t sound_id)
{
    #define SPU_CHANNEL_START_FIX 5
    MOD_PlaySoundEffect(sound_id + SPU_CHANNEL_START_FIX, sound_id, 128, 7 * audio_sfx_volume);
}

void audio_set_music_volume(int32_t music_volume)
{
    audio_music_volume = music_volume;
    MOD_SetMusicVolume(7281 * audio_music_volume);
}

void audio_set_sfx_volume(int32_t sfx_volume)
{
    audio_sfx_volume = sfx_volume;
}
