//---------------------------------------------------------
// file:	spotless_bubbles.h
// author:	Dylan
// email:	dylanjoonteck.lim@digipen.edu
//
// brief:	Function declarations Dishwasher level 1-2 game 
//			Bubbles animation
//
//---------------------------------------------------------

#ifndef SPOTLESS_BUBBLES_H
#define SPOTLESS_BUBBLES_H

#include "cprocessing.h"

// Call once when the level is created (in Spotless_CommonInit)
void Spotless_Bubbles_Init(void);

// Optional: reset if you ever need to clear all bubbles mid-level
void Spotless_Bubbles_Reset(void);

// Call once per frame while the game is running & not paused
// This both emits new bubbles from the cursor and updates/draws them.
void Spotless_Bubbles_Update(void);

// Call once when leaving the level (in Spotless_CommonExit)
void Spotless_Bubbles_Exit(void);

#endif // SPOTLESS_BUBBLES_H
