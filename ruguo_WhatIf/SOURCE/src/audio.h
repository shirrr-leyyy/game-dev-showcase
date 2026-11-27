//-----------------------------------------------------------------------
// file:	audio.c
// author:	Yi Ting, Dylan
// email:	yiting.tin@digipen.edu, dylanjoonteck.lim@digipen.edu
//
// brief:	Function declarations for the audio system in game
//          Handles loading, playback, volume control, sound effects,
//          background music looping, and audio cleanup
//-----------------------------------------------------------------------

#pragma once

void Audio_Init(void);
void Audio_PlayClick(void);
void Audio_PlayBugDeath(void);
void Audio_SetMusicVolume(float volume);
void Audio_SetSFXVolume(float volume);
float Audio_GetMusicVolume(void);
float Audio_GetSFXVolume(void);
void Audio_Scrub(void);
void Audio_Rinse(void);
void Audio_PickUp(void);
void Audio_PlateDrop(void);
void Audio_Mopping(void);
void Audio_Printing(void);
void Audio_Update(void);
void Audio_Exit(void);
