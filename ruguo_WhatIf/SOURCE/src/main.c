//---------------------------------------------------------
// file:	main.c
// author:	team ruguo
// email:	dylanjoonteck.lim@digipen.edu, f.goh@digipen.edu, 
// p.rongjunjerald@digipen.edu, shirleywenhui.l@digipen.edu,
// yiting.tin@digipen.edu 
//
// brief:	Main entry point for the sample project
//			of the CProcessing library
//
// Copyright © 2020 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "cprocessing.h"
#include "gamecredits.h"


// main() the starting point for the program
// CP_Engine_SetNextGameState() tells CProcessing which functions to use for init, update and exit
// CP_Engine_Run() is the core function that starts the simulation
int main(void)
{
	CP_Engine_SetNextGameState(Game_Credits_Init, Game_Credits_Update, Game_Credits_Exit);
	CP_System_SetWindowSize(1600, 900);
	CP_Engine_Run(0);
	return 0;
}
