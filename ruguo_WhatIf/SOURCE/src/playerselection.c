//---------------------------------------------------------
// file:	playerselection.c
// author:	Shirley
// email:	shirleywenhui.l@digipen.edu 
//
// brief:	Game selection: Dishwasher or Software Engineer
//
//---------------------------------------------------------

#include "cprocessing.h"
#include "playerselection.h"
#include "dishwasherlevels.h"
#include "engineerlevels.h"
#include "math.h"
#include "mainmenu.h"
#include "utils.h"
#include <stdio.h>

CP_Font myFont;
CP_Color bgColor;
CP_Image dishwasher_profile;
CP_Image engineer_profile;
CP_Image background;
CP_Image arrow;


typedef struct Player {
	float x, y;
	CP_Color color;
	float width, height;
	float textx, texty;
	float degree;
	float radius;

}Player;

typedef struct Button {
	float x, y;
	float width, height;
	float degree;

}Button;

Player dishwasher;
Player software_engineer;
CP_Color white;
CP_Color light_purple;

Button back;


void Player_Selection_Init(void)
{
	white = CP_Color_Create(255, 255, 255, 255);
	bgColor = CP_Color_Create(4, 7, 32, 255);
	myFont = CP_Font_Load("Assets/Pixellari.ttf");

	background = CP_Image_Load("Assets/bg.png");
	dishwasher_profile = CP_Image_Load("Assets/dishwasher.png");
	engineer_profile = CP_Image_Load("Assets/softwareengineer.png");

	dishwasher.x = 500.f;
	dishwasher.y = 500.f;
	dishwasher.color = white;
	dishwasher.width = 350.f;
	dishwasher.height = 350.f;
	dishwasher.textx = 500.f;
	dishwasher.texty = 275.f;
	dishwasher.degree = 0;
	dishwasher.radius = 30.f;

	software_engineer.x = 1100.f;
	software_engineer.y = 500.f;
	software_engineer.color = white;
	software_engineer.width = 350.f;
	software_engineer.height = 350.f;
	software_engineer.textx = 1100.f;
	software_engineer.texty = 275.f;
	software_engineer.degree = 0;
	software_engineer.radius = 30.f;

	back.x = 75.f;
	back.y = 75.f;
	back.width = 75.f;
	back.height = 75.f;
	back.degree = 0;
}

void Player_Selection_Update(void)
{
	CP_Graphics_ClearBackground(bgColor);
	CP_Image_Draw(background, 800.f, 450.f, 1600.f, 900.f, 255);

	float mouseX = CP_Input_GetMouseX();
	float mouseY = CP_Input_GetMouseY();
	
	// Draw Rectangle
	CP_Settings_Fill(white);
	CP_Settings_StrokeWeight(0.0f);

	//Interactive Color
	if (IsAreaHovered(dishwasher.x, dishwasher.y, dishwasher.width, dishwasher.height, mouseX, mouseY)) {
		CP_Settings_Fill(light_purple);
	}
	else {
		CP_Settings_Fill(white);
	}
	CP_Graphics_DrawRectAdvanced(dishwasher.x, dishwasher.y, dishwasher.width, dishwasher.height, dishwasher.degree, dishwasher.radius);
	
	//Interactive Color
	if (IsAreaHovered(software_engineer.x, software_engineer.y, software_engineer.width, software_engineer.height, mouseX, mouseY)) {
		CP_Settings_Fill(light_purple);
	}
	else {
		CP_Settings_Fill(white);
	}
	CP_Graphics_DrawRectAdvanced(software_engineer.x, software_engineer.y, software_engineer.width, software_engineer.height, software_engineer.degree, software_engineer.radius);

	// Draw player text
	CP_Settings_Fill(white);
	CP_Font_Set(myFont);
	CP_Settings_TextSize(60.0f);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Font_DrawText("Dishwasher", dishwasher.textx, dishwasher.texty);
	CP_Font_DrawText("Software Engineer", software_engineer.textx, software_engineer.texty);

	// Load Profile
	CP_Settings_AntiAlias(FALSE);
	CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_NEAREST);
	CP_Image_Draw(dishwasher_profile, dishwasher.x, dishwasher.y + 10, 900.f, 900.f, 255);
	CP_Image_Draw(engineer_profile, software_engineer.x + 10, software_engineer.y + 5, 900.f, 900.f, 255);

	// Start Dishwasher Game
	if (IsAreaClicked(dishwasher.x, dishwasher.y, dishwasher.width, dishwasher.height, mouseX, mouseY)) {
		if (CP_Input_MouseClicked())
		{
			CP_Engine_SetNextGameState(Dishwasher_Levels_Init, Dishwasher_Levels_Update, Dishwasher_Levels_Exit);
		}
	}

	// Start Software Engineer Game
	if (IsAreaClicked(software_engineer.x, software_engineer.y, software_engineer.width, software_engineer.height, mouseX, mouseY)) {
		if (CP_Input_MouseClicked())
		{
			CP_Engine_SetNextGameState(Engineer_Levels_Init, Engineer_Levels_Update, Engineer_Levels_Exit);
		}
	}

	// Terminate Window
	if (CP_Input_KeyDown(KEY_Q)) {
		CP_Engine_Terminate();
	}

	//Back
	if (IsAreaHovered(back.x, back.y, back.width, back.height, mouseX, mouseY)) {
		arrow = CP_Image_Load("Assets/arrow.png");
		CP_Settings_Stroke(white);
		CP_Settings_Fill(light_purple);
	}
	else {
		arrow = CP_Image_Load("Assets/purple_arrow.png");
		CP_Settings_Stroke(light_purple);
		CP_Settings_Fill(white);

	}

	// back btn
	CP_Settings_StrokeWeight(5.0f);
	CP_Graphics_DrawRect(back.x, back.y, back.width, back.height); // back btn
	CP_Image_Draw(arrow, back.x + 2.5f, back.y, 100.f, 100.f, 255);

	// Go back to previous menu
	if (IsAreaClicked(back.x, back.y, back.width, back.height, mouseX, mouseY)) {
		if (CP_Input_MouseClicked())
		{
			CP_Engine_SetNextGameState(Main_Menu_Init, Main_Menu_Update, Main_Menu_Exit);
		}
	}
}

void Player_Selection_Exit(void)
{
	CP_Font_Free(myFont);
	CP_Graphics_ClearBackground(bgColor);
	CP_Image_Free(dishwasher_profile);
	CP_Image_Free(engineer_profile);
	CP_Image_Free(background);
	CP_Image_Free(arrow);
}