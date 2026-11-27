//-----------------------------------------------------------------------
// file:	audio.c
// author:	Yi Ting, Dylan
// email:	yiting.tin@digipen.edu, dylanjoonteck.lim@digipen.edu
//
// brief:	Function definitions for the audio system in game
//          Handles loading, playback, volume control, sound effects,
//          background music looping, and audio cleanup
//-----------------------------------------------------------------------

#include "audio.h"
#include "cprocessing.h"

// sounds
static CP_Sound clickSound;
static CP_Sound bugDeathSound;
static CP_Sound bgm;
static CP_Sound scrub;
static CP_Sound rinse;
static CP_Sound pickup;
static CP_Sound platedrop;
static CP_Sound mopping;
static CP_Sound printing;

// volume channels
static float musicVolume = 0.8f;
static float sfxVolume = 1.0f;

// bgm loop control
static float bgmTimer = 120.0f;
static float bgmLength = 112.0f;

void Audio_Init(void)
{
	// load sounds
    clickSound = CP_Sound_Load("Assets/click.mp3");
    bugDeathSound = CP_Sound_Load("Assets/bug_death.mp3");
    bgm = CP_Sound_Load("Assets/gam100.mp3");
    scrub = CP_Sound_Load("Assets/scrub.ogg");
    rinse = CP_Sound_Load("Assets/rinse.ogg");
    pickup = CP_Sound_Load("Assets/pickup.mp3");
    platedrop = CP_Sound_Load("Assets/plate_drop.mp3");
    mopping = CP_Sound_Load("Assets/mopping.mp3");
    printing = CP_Sound_Load("Assets/printing.mp3");

    // apply default volume
    CP_Sound_SetGroupVolume(CP_SOUND_GROUP_MUSIC, musicVolume);
    CP_Sound_SetGroupVolume(CP_SOUND_GROUP_SFX, sfxVolume);

    // start looping bgm immediately
    CP_Sound_PlayAdvanced(bgm, 1.0f, 1.0f, TRUE, CP_SOUND_GROUP_MUSIC);
}

void Audio_PlayClick(void)
{
    CP_Sound_PlayAdvanced(clickSound, 1, 1, FALSE, CP_SOUND_GROUP_SFX);
}

void Audio_PlayBugDeath(void)
{
    CP_Sound_PlayAdvanced(bugDeathSound, 1, 1, FALSE, CP_SOUND_GROUP_SFX);
}

void Audio_SetMusicVolume(float volume)
{
    if (volume < 0) volume = 0;
    if (volume > 1) volume = 1;
    musicVolume = volume;

    CP_Sound_SetGroupVolume(CP_SOUND_GROUP_MUSIC, musicVolume);
}

void Audio_SetSFXVolume(float volume)
{
    if (volume < 0) volume = 0;
    if (volume > 1) volume = 1;
    sfxVolume = volume;

    CP_Sound_SetGroupVolume(CP_SOUND_GROUP_SFX, sfxVolume);
}

float Audio_GetMusicVolume(void)
{
    return musicVolume;
}

float Audio_GetSFXVolume(void)
{
    CP_Sound_SetGroupVolume(CP_SOUND_GROUP_SFX, .2f);
    CP_Sound_PlayAdvanced(bugDeathSound, 1, 1, FALSE, CP_SOUND_GROUP_SFX);
    return sfxVolume;    
}

void Audio_Scrub(void) {
    CP_Sound_PlayAdvanced(scrub, 1, 1, FALSE, CP_SOUND_GROUP_SFX);
}

void Audio_Rinse(void) {
    CP_Sound_PlayAdvanced(rinse, 1, 1, FALSE, CP_SOUND_GROUP_SFX);
}

void Audio_PickUp(void) {
    CP_Sound_PlayAdvanced(pickup, 1, 1, FALSE, CP_SOUND_GROUP_SFX);
}

void Audio_PlateDrop(void) {
    CP_Sound_PlayAdvanced(platedrop, 1, 1, FALSE, CP_SOUND_GROUP_SFX);
}

void Audio_Mopping(void) {
    CP_Sound_PlayAdvanced(mopping, 1, 1, FALSE, CP_SOUND_GROUP_SFX);
}

void Audio_Printing(void) {
    CP_Sound_PlayAdvanced(printing, 1, 1, FALSE, CP_SOUND_GROUP_SFX);
}

void Audio_Update(void)
{
}

void Audio_Exit(void)
{
    CP_Sound_Free(clickSound);
    CP_Sound_Free(bugDeathSound);
    CP_Sound_Free(bgm);
    CP_Sound_Free(scrub);
    CP_Sound_Free(rinse);
    CP_Sound_Free(pickup);
    CP_Sound_Free(platedrop);
    CP_Sound_Free(mopping);
    CP_Sound_Free(printing);
}
