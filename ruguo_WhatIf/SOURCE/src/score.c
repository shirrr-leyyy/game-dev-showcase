//-----------------------------------------------------------------
// file:	score.c
// author:	Dylan, Yi Ting
// email:	dylanjoonteck.lim@digipen.edu, yiting.tin@digipen.edu
//
// brief:	Function definitions for the scoring system in game
//          Calculates and save total, highest score of each game. 
//          Shared accross all minigames.
//-----------------------------------------------------------------

#include "score.h"
#include <string.h>

static int currentScore = 0;
static int finalScore = 0;

static int currentStreak = 0;
static int highStreak = 0;

#ifndef SCORE_MAX_LEVELS
#define SCORE_MAX_LEVELS 16
#endif

static int levelScores[SCORE_MAX_LEVELS];
static int highScores[SCORE_MAX_LEVELS];
static int highStreaks[SCORE_MAX_LEVELS];

static int levelCount = SCORE_MAX_LEVELS;
static int currentLevel = -1;

// set once per process; prevents accidental high-score resets
static int s_bootstrapped = 0;

// Initializes all scoring variables
void Score_Init(void) {
    currentScore = 0;
    finalScore = 0;

    // last attempts can reset each time
    memset(levelScores, 0, sizeof(levelScores));

    // Only clear highs once per app run
    if (!s_bootstrapped) {
        memset(highScores, 0, sizeof(highScores));
        memset(highStreaks, 0, sizeof(highStreaks));
        s_bootstrapped = 1;
    }
    currentLevel = -1;
}

// Add points to current score
void Score_AddPoints(int points) {
    currentScore += points;
}

// Deduct points from current score
void Score_DeductPoints(int points) {
    currentScore -= points;
    if (currentScore < 0) currentScore = 0;
}

// Get current score
int Score_Get(void) {
    return currentScore;
}

// Set final score
void Score_SetFinal(int value) {
    finalScore = value;
}

// Score scored at level end
int Score_Final(void) {
    return finalScore;
}

// -------- levels/highs --------
void Score_InitLevels(int max_levels) {
    if (max_levels > 0 && max_levels <= SCORE_MAX_LEVELS)
        levelCount = max_levels;

    // Reset last runs for a fresh session of levels
    memset(levelScores, 0, sizeof(levelScores));

    // Do NOT clear highs unless this is the first ever call in the process
    if (!s_bootstrapped) {
        memset(highScores, 0, sizeof(highScores));
        memset(highStreaks, 0, sizeof(highStreaks));
        s_bootstrapped = 1;
    }
}

void Score_LevelStart(int level_index) {
    if (level_index < 1 || level_index > levelCount) {
        currentLevel = -1;
        currentScore = 0;
        return;
    }
    currentLevel = level_index;
    currentScore = 0;  // fresh attempt
    finalScore = 0;

    // streak reset
    currentStreak = 0;
}

void Score_LevelEnd(void) {
    if (currentLevel < 1 || currentLevel > levelCount) return;

    // store last attempt
    levelScores[currentLevel - 1] = currentScore;

    // update high score if this attempt is better
    if (currentScore > highScores[currentLevel - 1])
        highScores[currentLevel - 1] = currentScore;

    // store best streak per level
    if (currentStreak > highStreaks[currentLevel - 1])
        highStreaks[currentLevel - 1] = currentStreak;

    // legacy mirror
    finalScore = currentScore;

    // optional: stay on this level index; next LevelStart will reset currentScore
}

int Score_GetLevelScore(int level_index) {
    if (level_index < 1 || level_index > levelCount) return 0;
    return levelScores[level_index - 1];
}

int Score_GetTotalScore(void) {
    int total = 0;
    for (int i = 0; i < levelCount; ++i) total += levelScores[i];
    return total;
}

// -------- high score API --------
int Score_GetHigh(int level_index) {
    if (level_index < 1 || level_index > levelCount) return 0;
    return highScores[level_index - 1];
}

int Score_GetHighTotal(void) {
    int total = 0;
    for (int i = 0; i < levelCount; ++i) total += highScores[i];
    return total;
}

void Score_ResetHighs(void) {
    memset(highScores, 0, sizeof(highScores));
    memset(highStreaks, 0, sizeof(highStreaks));
}

// -------- streak API --------
int Score_GetStreak(void) {
    return currentStreak;
}

// Increases streak by 1
void Score_IncrementStreak(void) {
    currentStreak++;
}

// Returns the highest streak for a given level
int Score_GetHighStreak(int level_index) {
    if (level_index < 1 || level_index > levelCount) return 0;
    return highStreaks[level_index - 1];
}

// Returns streak tier
int Score_GetStreakTier(void)
{
    if (currentStreak >= 35) return 4;
    if (currentStreak >= 25) return 3;
    if (currentStreak >= 15) return 2;
    if (currentStreak >= 1)  return 1;
    return 0;
}

// Resets streak to zero
void Score_ResetStreak(void) {
    currentStreak = 0;
}
