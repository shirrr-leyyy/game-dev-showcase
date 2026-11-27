//---------------------------------------------------------
// file:	dishwasherlevels.c
// author:	Shirley
// email:	shirleywenhui.l@digipen.edu,
//
// brief:	Dishwasher career level seclection menu
//---------------------------------------------------------
#include "cprocessing.h"
#include "dishwasherlevels.h"
#include "dishwasher1_2public.h"   
#include "playerselection.h"
#include "score.h"
#include "math.h"
#include "utils.h"
#include <stdio.h>
#include "kitchen.h"
#include "kitchen2.h"
#include "payslip.h"

CP_Font myFont;
CP_Image arrow;
CP_Image coin;

typedef struct Level {
	float x, y;
	float width, height;
	float degree;
	float radius;

}Level;

typedef struct Button {
	float x, y;
	float width, height;
	float degree;

}Button;

Level level1;
Level level2;
Level level3;
Level level4;

Button back;

CP_Color white;
CP_Color bgColor;
CP_Color gold;
CP_Color light_purple;
CP_Color dark_purple;

int coin_txt = 0;

int payslip = 0;

void Dishwasher_Levels_Init(void)
{
	white = CP_Color_Create(255, 255, 255, 255);
	bgColor = CP_Color_Create(4, 7, 32, 255);
	light_purple = CP_Color_Create(201, 160, 220, 255);
	dark_purple = CP_Color_Create(161, 88, 220, 255);
	gold = CP_Color_Create(245, 227, 58, 255);
	myFont = CP_Font_Load("Assets/Pixellari.ttf");

	coin = CP_Image_Load("Assets/coin.png");

	level1.x = 272.5f;
	level1.y = 450.f;
	level1.width = 225.f;
	level1.height = 225.f;
	level1.degree = 0;
	level1.radius = 24.f;

	level2.x = 597.5f;
	level2.y = 450.f;
	level2.width = 225.f;
	level2.height = 225.f;
	level2.degree = 0;
	level2.radius = 24.f;

	level3.x = 1002.5f;
	level3.y = 450.f;
	level3.width = 225.f;
	level3.height = 225.f;
	level3.degree = 0;
	level3.radius = 24.f;

	level4.x = 1327.5f;
	level4.y = 450.f;
	level4.width = 225.f;
	level4.height = 225.f;
	level4.degree = 0;
	level4.radius = 24.f;

	back.x = 75.f;
	back.y = 75.f;
	back.width = 75.f;
	back.height = 75.f;
	back.degree = 0;
}


void Dishwasher_Levels_Update(void)
{
	CP_Graphics_ClearBackground(bgColor);
	if (payslip) {
		Payslip_Init();
		int continue_game = Payslip_Update(1);
		if (continue_game == 1) {
			payslip = 0;
		}
	}

	if (!payslip){
		float mouseX = CP_Input_GetMouseX();
		float mouseY = CP_Input_GetMouseY();

		// Draw Rectangle
		CP_Settings_Stroke(white);
		CP_Settings_StrokeWeight(10.0f);
		CP_Settings_Fill(light_purple);
		CP_Graphics_DrawRectAdvanced(level1.x, level1.y, level1.width, level1.height, level1.degree, level1.radius);
		CP_Graphics_DrawRectAdvanced(level2.x, level2.y, level2.width, level2.height, level2.degree, level2.radius);
		
		CP_Settings_Fill(dark_purple);
		CP_Graphics_DrawRectAdvanced(level3.x, level3.y, level3.width, level3.height, level3.degree, level3.radius);
		CP_Graphics_DrawRectAdvanced(level4.x, level4.y, level4.width, level4.height, level4.degree, level4.radius);

		//Back
		if (IsAreaHovered(back.x, back.y, back.width, back.height, mouseX, mouseY)) {
			arrow = CP_Image_Load("Assets/purple_arrow.png");
			CP_Settings_Stroke(light_purple);
			CP_Settings_Fill(white);
		}
		else {
			arrow = CP_Image_Load("Assets/arrow.png");
			CP_Settings_Stroke(white);
			CP_Settings_Fill(light_purple);
		}

		// back btn
		CP_Settings_StrokeWeight(5.0f);
		CP_Graphics_DrawRect(back.x, back.y, back.width, back.height); // back btn
		CP_Image_Draw(arrow, back.x + 2.5f, back.y, 100.f, 100.f, 255);

		// Draw coin image
		CP_Image_Draw(coin, 175.f, 70.f, 150.f, 150.f, 255);

		// IF hovered
		if (IsAreaHovered(175.f, 70.f, 50.f, 50.f, mouseX, mouseY)) {
			CP_Settings_Fill(white);
		}
		else {
			CP_Settings_Fill(gold);
		}

		if (IsAreaClicked(175.f, 70.f, 50.f, 50.f, mouseX, mouseY)) {
			if (CP_Input_MouseClicked()) {
				payslip = 1;
			}
		}

		//Money Earned
		CP_Font_Set(myFont);
		CP_Settings_TextSize(50.0f);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_MIDDLE);
		coin_txt = (Score_GetHigh(1) >= 100) ? 7200 : ((Score_GetHigh(1) >= 70) ? 6600 : (Score_GetHigh(1) > 0) ? 6000 : 0);
		coin_txt += (Score_GetHigh(2) >= 60) ? 7200 : ((Score_GetHigh(2) >= 40) ? 6600 : (Score_GetHigh(2) > 0) ? 6000 : 0);
		coin_txt += (Score_GetHigh(3) >= 70) ? 7200 : ((Score_GetHigh(3) >= 60) ? 6600 : (Score_GetHigh(3) > 0) ? 6000 : 0);
		coin_txt += (Score_GetHigh(4) >= 110) ? 7200 : ((Score_GetHigh(4) >= 90) ? 6600 : (Score_GetHigh(4) > 0) ? 6000 : 0);
		char coinText[50] = { 0 };
		sprintf_s(coinText, 50, "$%d", coin_txt);
		CP_Font_DrawText(coinText, 205.f, 75.f);


		// Draw Text
		CP_Settings_TextSize(120.0f);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
		CP_Settings_Fill(white);
		CP_Font_DrawText("1", level1.x - 1.5f, level1.y + 10);
		CP_Font_DrawText("2", level2.x, level2.y + 10);
		CP_Font_DrawText("3", level3.x, level3.y + 10);
		CP_Font_DrawText("4", level4.x, level4.y + 10);

		// Draw Title
		CP_Font_DrawText("Dishwasher", 800.f, 200.f);


		//label
		CP_Settings_TextSize(60.0f);
		CP_Font_DrawText("Scrub Master", 440.f, 620.f);
		CP_Font_DrawText("Kitchen Chaos", 1160.f, 620.f);

		// Draw Hover Line
		CP_Settings_Fill(white);
		CP_Settings_StrokeWeight(10.0f);

		// Terminate Window
		if (CP_Input_KeyDown(KEY_X)) {
			CP_Engine_Terminate();
		}

		if (IsAreaHovered(level1.x, level1.y, level1.width, level1.height, mouseX, mouseY)) {
			CP_Graphics_DrawLine(level1.x - 37.5f, level1.y + 50, level1.x + 37.5f, level1.y + 50);
		}
		else if (IsAreaHovered(level2.x, level2.y, level2.width, level2.height, mouseX, mouseY)) {
			CP_Graphics_DrawLine(level2.x - 37.5f, level2.y + 50, level2.x + 37.5f, level2.y + 50);
		}
		else if (IsAreaHovered(level3.x, level3.y, level3.width, level3.height, mouseX, mouseY)) {
			CP_Graphics_DrawLine(level3.x - 37.5f, level3.y + 50, level3.x + 37.5f, level3.y + 50);
		}
		else if (IsAreaHovered(level4.x, level4.y, level4.width, level4.height, mouseX, mouseY)) {
			CP_Graphics_DrawLine(level4.x - 37.5f, level4.y + 50, level4.x + 37.5f, level4.y + 50);
		}



		// Go back to previous menu
		if (IsAreaClicked(back.x, back.y, back.width, back.height, mouseX, mouseY)) {
			if (CP_Input_MouseClicked())
			{
				CP_Engine_SetNextGameState(Player_Selection_Init, Player_Selection_Update, Player_Selection_Exit);
			}
		}

		//Level 1
		if (IsAreaClicked(level1.x, level1.y, level1.width, level1.height, mouseX, mouseY)) {
			if (CP_Input_MouseClicked())
			{
				CP_Engine_SetNextGameState(Dishwasher_1_Init, Dishwasher_1_Update, Dishwasher_1_Exit);
			}
		}

		//Level 2
		if (IsAreaClicked(level2.x, level2.y, level2.width, level2.height, mouseX, mouseY)) {
			if (CP_Input_MouseClicked())
			{
				CP_Engine_SetNextGameState(Dishwasher_2_Init, Dishwasher_2_Update, Dishwasher_2_Exit);
			}
		}

		//Level 3level3.x, level3.y, level3.width, level3.height,
		if (IsAreaClicked(level3.x, level3.y, level3.width, level3.height, mouseX, mouseY)) {
			if (CP_Input_MouseClicked())
			{
				CP_Engine_SetNextGameState(Kitchen_Init, Kitchen_Update, Kitchen_Exit);
			}
		}

		//Level 4
		if (IsAreaClicked(level4.x, level4.y, level4.width, level4.height, mouseX, mouseY)) {
			if (CP_Input_MouseClicked())
			{
				CP_Engine_SetNextGameState(Kitchen2_Init, Kitchen2_Update, Kitchen2_Exit);
			}
		}
	}
}

void Dishwasher_Levels_Exit(void)
{
	CP_Font_Free(myFont);
	CP_Graphics_ClearBackground(bgColor);
	CP_Image_Free(arrow);
	CP_Image_Free(coin);
	Payslip_Exit();

}