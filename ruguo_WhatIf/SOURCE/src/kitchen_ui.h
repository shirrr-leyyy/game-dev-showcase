//---------------------------------------------------------
// file:	kitchen_ui.h
// author:	Shirley
// email:	shirleywenhui.l@digipen.edu 
//
// brief:	Function declarations for Dishwasher Level 
//			3 & 4 Game UI
//
//---------------------------------------------------------

#pragma once
#include "cprocessing.h"
#include <stdbool.h>

void Kitchen_Ui_Init(void);
void Kitchen_Ui(void);
void Game_Over_ui(int level);
bool Game_Menu_ui(int level);
void Kitchen_Ui_Exit(void);