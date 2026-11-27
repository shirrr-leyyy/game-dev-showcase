//-----------------------------------------------------------------
// file:	score.h
// author:	Dylan, Yi Ting
// email:	dylanjoonteck.lim@digipen.edu, yiting.tin@digipen.edu
//
// brief:	Function declarations for the scoring system in game
//-----------------------------------------------------------------

#ifndef SCORE_H
#define SCORE_H

#pragma once

// existing
void Score_Init(void);
void Score_AddPoints(int points);
void Score_DeductPoints(int points);
int  Score_Get(void);
void Score_SetFinal(int value);
int  Score_Final(void);

// levels/high-scores
void Score_InitLevels(int max_levels);      // call once at boot (e.g., 3 levels)
void Score_LevelStart(int level_index);     // call on level enter
void Score_LevelEnd(void);                  // call on level completion/timeout/quit

int  Score_GetLevelScore(int level_index);  // last attempt score for that level
int  Score_GetTotalScore(void);             // sum of last attempt scores

// high score api
int  Score_GetHigh(int level_index);        // best score achieved for that level
int  Score_GetHighTotal(void);              // sum of best scores across all levels
void Score_ResetHighs(void);                // optional: clear highs during runtime

// streak api
int  Score_GetStreak(void);
void Score_IncrementStreak(void);
int Score_GetHighStreak(int level_index);
int Score_GetStreakTier(void);
void Score_ResetStreak(void);
#endif