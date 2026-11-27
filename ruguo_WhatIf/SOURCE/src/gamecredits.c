//---------------------------------------------------------
// file:	gamecredits.c
// author:	Shirley
// email:	shirleywenhui.l@digipen.edu,
//
// brief:	DigiPen logo frame before game and credits screen 
//			for programmers, instructors and school president,
//			professors
//---------------------------------------------------------
#include "cprocessing.h"
#include <stdio.h>
#include <stdlib.h>
#include "gamecredits.h"
#include "mainmenu.h"
#include "audio.h"
#include "utils.h"

CP_Font myFont;
CP_Image digi_logo;
CP_Image dw_heart;
CP_Image sw_heart;
CP_Image arrow;

CP_Color white;
CP_Color bgColor;
CP_Color light_purple;

typedef struct BackBtn {
	float x, y;
	float w, h;
} BackBtn;

BackBtn backBtn;

float y_pos = 900.f;
float stop_y = 0.f;

float fadeAlpha = 255.0f;
int fadingOut = 0;

void Game_Credits_Init(void)
{
	white = CP_Color_Create(255, 255, 255, 255);
	bgColor = CP_Color_Create(4, 7, 32, 255);
	light_purple = CP_Color_Create(201, 160, 220, 255);
	myFont = CP_Font_Load("Assets/Pixellari.ttf");

	digi_logo = CP_Image_Load("Assets/DigiPen_WHITE.png");
	dw_heart = CP_Image_Load("Assets/dishwasher_heart.png");
	sw_heart = CP_Image_Load("Assets/softwareengineer_heart.png");
	arrow = CP_Image_Load("Assets/purple_arrow.png");

	// back button
	backBtn.x = 75.f;
	backBtn.y = 75.f;
	backBtn.w = 75.f;
	backBtn.h = 75.f;

	fadeAlpha = 255.0f;
	fadingOut = 0;

	Audio_Init();
}

static void Draw_BackButton(void)
{
	float mx = CP_Input_GetMouseX();
	float my = CP_Input_GetMouseY();

	int hovered = IsAreaHovered(
		backBtn.x, backBtn.y,
		backBtn.w, backBtn.h,
		mx, my
	);

	// hover swap arrow + colors
	if (hovered)
	{
		arrow = CP_Image_Load("Assets/arrow.png");
		CP_Settings_Stroke(white);
		CP_Settings_Fill(light_purple);
	}
	else
	{
		arrow = CP_Image_Load("Assets/purple_arrow.png");
		CP_Settings_Stroke(light_purple);
		CP_Settings_Fill(white);
	}

	CP_Settings_StrokeWeight(5.0f);
	CP_Graphics_DrawRect(backBtn.x, backBtn.y, backBtn.w, backBtn.h);

	CP_Image_Draw(
		arrow,
		backBtn.x + 2.5f,
		backBtn.y,
		100.f,
		100.f,
		255
	);

	// CLICK HANDLING � returns to main menu
	if (IsAreaClicked(backBtn.x, backBtn.y, backBtn.w, backBtn.h, mx, my))
	{
		CP_Engine_SetNextGameState(Main_Menu_Init, Main_Menu_Update, Main_Menu_Exit);
	}
}


void Game_Credits_Update(void)
{
	CP_Graphics_ClearBackground(bgColor);

	float t = CP_System_GetSeconds();

	if (!fadingOut)
	{
		fadeAlpha -= 200 * CP_System_GetDt();
		if (fadeAlpha < 0) fadeAlpha = 0;
	}

	// fade in alpha
	float logoAlpha = t * 150.0f;
	if (logoAlpha > 255.0f)
		logoAlpha = 255.0f;

	// draw logo
	CP_Image_Draw(digi_logo, 800.f, 400.f, 1240.f, 362.f, (int)logoAlpha);

	// fade in text after logo
	if (t > 0.5f)
	{
		float textAlpha = (t - 1.5f) * 180.f;
		if (textAlpha > 255.0f)
			textAlpha = 255.0f;

		CP_Font_Set(myFont);
		CP_Settings_TextSize(24.f);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
		CP_Settings_Fill(CP_Color_Create(255, 255, 255, (int)textAlpha));

		CP_Font_DrawText(
			"All content (C) 2025 Digipen Institute of Technology Singapore. All Rights Reserved",
			800.f, 820.f
		);
	}

	if (t > 3.0f && !fadingOut)
	{
		fadingOut = 1; // trigger fade out
	}

	if (fadingOut)
	{
		fadeAlpha += 200 * CP_System_GetDt();
		if (fadeAlpha >= 255.0f)
		{
			fadeAlpha = 255.0f;
			// switch to main menu after fully faded
			CP_Engine_SetNextGameState(Main_Menu_Init, Main_Menu_Update, Main_Menu_Exit);
		}
	}

	// draw fade overlay
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, (int)fadeAlpha));
	CP_Graphics_DrawRect(800.f, 450.f, 1600.f, 900.f);

	if (CP_Input_KeyTriggered(KEY_ESCAPE) || CP_Input_MouseClicked())
	{
		CP_Engine_SetNextGameState(Main_Menu_Init, Main_Menu_Update, Main_Menu_Exit);
	}
}

void Game_Credits_Update_v2(void) {
	CP_Graphics_ClearBackground(bgColor);
	CP_Settings_AntiAlias(FALSE);
	CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_NEAREST);
	CP_Image_Draw(dw_heart, 200.f, 100.f, 400.f, 400.f, 255);
	CP_Image_Draw(sw_heart, 1400.f, 800.f, 400.f, 400.f, 255);

	CP_Font_Set(myFont);
	CP_Settings_TextSize(32.f);
	CP_Settings_Fill(white);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

	const char* text = "What If...\n\n\nFelicia Goh\n\nTin Yi Ting\nShirley Liau\n\nDylan Lim\n\nFelicia Goh\n\nDylan Lim\n\nShirley Liau\n\nJerald Peng\nTin Yi Ting\n\nTin Yi Ting\nShirley Liau\n\nDylan Lim\n\nLow Yue Jun\n\nGerald Wong\nSoroor Malekmohammadi Faradounbeh\nTommy Tan";
	const char* prof_name = "Created at\n\n\n\n\n\n\nClaude Comair\n\nChu Jason Yeu Tat   Michael Gats\nTan Chek Ming   Prasanna Kumar Ghali\nMandy Wong   Johnny Deek";
	const char* title_text = "\n\n\n\nDIRECTOR and PROGRAMMER\n\n\n\nPRODUCER and DESIGN LEAD\n\n\n\n\n\n TECHNICAL LEAD\n\n\n\n ART LEAD\n\n\n\n AUDIO LEAD\n\n\n\n GRAPHICS PROGRAMMER\n\n\n\n LEVEL DESIGNER\n\n\n\n\n\n ENVIRONMENT ARTIST AND UI\n\n\n\n\n\n COMPOSER\n\n\n\nSPECIAL THANKS\n\n\n\nFACULTY and ADVISORS\n\n\n";
	const char* school_credit = "\n\n\n\n\n\n\n\n\n\n\n\nPRESIDENT\n\n\n\nEXECUTIVES";

	y_pos -= 3;
	stop_y = y_pos + 1700.f;


	if (CP_Input_KeyTriggered(KEY_ESCAPE)) {
		y_pos = 900.0f;
		CP_Engine_SetNextGameState(Main_Menu_Init, Main_Menu_Update, Main_Menu_Exit);
	}

	if (stop_y <= 125.f){

		y_pos = 900.0f;
		Sleep(3000);
		CP_Engine_SetNextGameState(Main_Menu_Init, Main_Menu_Update, Main_Menu_Exit);
	}
	CP_Settings_TextSize(50.f);
	CP_Font_DrawTextBox(text, 0.f, y_pos, 1600.f);

	CP_Settings_Fill(light_purple);
	CP_Settings_TextSize(25.f);
	CP_Font_DrawTextBox(title_text, 0.f, y_pos + 10, 1600.f);

	CP_Settings_Fill(white);
	CP_Settings_TextSize(50.f);
	CP_Font_DrawTextBox(prof_name, 0.f, stop_y, 1600.f);

	CP_Image_Draw(digi_logo, 800.f, stop_y + 150.f, 685.f, 200.f, 255);

	CP_Settings_Fill(light_purple);
	CP_Settings_TextSize(25.f);
	CP_Font_DrawTextBox(school_credit, 0.f, stop_y + 10, 1600.f);

	Draw_BackButton();

	if (stop_y <= 175.f) {
		CP_Settings_TextSize(24.f);
		CP_Font_DrawText("WWW.DIGIPEN.EDU", 800.f, 800.f);
		CP_Font_DrawText("All content (C) 2025 Digipen Institute of Technology Singapore. All Rights Reserved", 800.f, 820.f);
	}
}

void Game_Credits_Exit(void)
{
	CP_Font_Free(myFont);
	CP_Graphics_ClearBackground(bgColor);
	CP_Image_Free(digi_logo);
	CP_Image_Free(dw_heart);
	CP_Image_Free(sw_heart);
}