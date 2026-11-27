//---------------------------------------------------------
// file:	dishwasher1.c
// author:	Dylan
// email:	dylanjoonteck.lim@digipen.edu
//
// brief:	Dishwasher level 1 game passing int 0(easy) to  
//			call function using same ui and game logic 
//
//---------------------------------------------------------

#include "dishwasher1_2internal.h"
#include "dishwasher1_2public.h"   

void Dishwasher_1_Init(void) { Spotless_CommonInit(0); }
void Dishwasher_1_Update(void) { Spotless_CommonUpdate(0); }
void Dishwasher_1_Exit(void) { Spotless_CommonExit(); }
