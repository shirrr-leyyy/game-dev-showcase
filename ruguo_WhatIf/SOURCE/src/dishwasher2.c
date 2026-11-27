//---------------------------------------------------------
// file:	dishwasher2.c
// author:	Dylan
// email:	dylanjoonteck.lim@digipen.edu
//
// brief:	Dishwasher level 2 game passing int 1(hard) to  
//			call function using same ui and game logic 
//
//---------------------------------------------------------

#include "dishwasher1_2internal.h"
#include "dishwasher1_2public.h"   

void Dishwasher_2_Init(void) { Spotless_CommonInit(1); }
void Dishwasher_2_Update(void) { Spotless_CommonUpdate(1); }
void Dishwasher_2_Exit(void) { Spotless_CommonExit(); }
