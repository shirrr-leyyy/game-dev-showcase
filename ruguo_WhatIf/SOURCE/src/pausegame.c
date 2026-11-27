//---------------------------------------------------------
// file:	pausegame.c
// author:	Shirley, Yi Ting
// email:	shirleywenhui.l@digipen.edu, yiting.tin@digipen.edu 
//
// brief:	Pause Game frame shared across minigames
//
//---------------------------------------------------------

#include "cprocessing.h"
#include "kitchen.h"
#include "utils.h"
#include <stdio.h>

CP_Font myFont;

typedef struct Rect {
	float x, y;
	CP_Color color;
	float width, height;
	float line_x1, line_y1;
	float line_x2, line_y2;
	float degree;
	float radius;

}Rect;

Rect play;
Rect quit;
Rect restart;

CP_Color white;
CP_Color bgColor;
CP_Color light_purple;

void Pause_Game_Init(void)
{
	white = CP_Color_Create(255, 255, 255, 255);
	bgColor = CP_Color_Create(4, 7, 32, 255);
	light_purple = CP_Color_Create(201, 160, 220, 255);
	myFont = CP_Font_Load("Assets/Pixellari.ttf");

	play.x = 800.f;
	play.y = 375.f;
	play.color = white;
	play.width = 350.f;
	play.height = 90.f;
	play.line_x1 = 725.f;
	play.line_x2 = 875.f;
	play.line_y1 = 405.f;
	play.line_y2 = 405.f;
	play.degree = 0;
	play.radius = 16.f;

	restart.x = 800.f;
	restart.y = 525.f;
	restart.color = white;
	restart.width = 350.f;
	restart.height = 90.f;
	restart.line_x1 = 660.f;
	restart.line_x2 = 940.f;
	restart.line_y1 = 555.f;
	restart.line_y2 = 555.f;
	restart.degree = 0;
	restart.radius = 16.f;

	quit.x = 800.f;
	quit.y = 675.f;
	quit.color = white;
	quit.width = 350.f;
	quit.height = 90.f;
	quit.line_x1 = 730.f;
	quit.line_x2 = 870.f;
	quit.line_y1 = 705.f;
	quit.line_y2 = 705.f;
	quit.degree = 0;
	quit.radius = 16.f;
}

int DrawMenuButton(const char* label, float x, float y, float w, float h, float* scale)
{
	float mx = CP_Input_GetMouseX();
	float my = CP_Input_GetMouseY();

	int hovered =
		(mx >= x - w / 2 && mx <= x + w / 2 &&
			my >= y - h / 2 && my <= y + h / 2);

	// scale animation
	if (hovered)
		*scale = 1.15f;
	else
		*scale += (1.0f - *scale) * 10.0f * CP_System_GetDt();

	float drawW = w * (*scale);
	float drawH = h * (*scale);
	float radius = 25.0f;

	// draw buttons background
	CP_Settings_NoStroke();
	CP_Settings_Fill(white);
	CP_Graphics_DrawRectAdvanced(x, y, drawW, drawH, 0, radius);

	// draw outline
	CP_Settings_StrokeWeight(8);
	CP_Settings_Stroke(light_purple);
	CP_Settings_NoFill();
	CP_Graphics_DrawRectAdvanced(x, y, drawW, drawH, 0, radius);

	// text
	CP_Settings_Fill(light_purple);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Settings_TextSize(48 * (*scale));
	CP_Font_DrawText(label, x, y);

	return hovered;
}

int Pause_Game_Update()
{

	CP_Font_Set(myFont);
	CP_Settings_TextSize(180.0f);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

	CP_Settings_Fill(light_purple);
	CP_Font_DrawText("PAUSE", 810.f, 210.f);
	CP_Settings_Fill(white);
	CP_Font_DrawText("PAUSE", 800.f, 200.f);

	// Draw Rectangle
	CP_Settings_Fill(white);
	CP_Settings_StrokeWeight(0.0f);

	static float scalePlay = 1.0f;
	static float scaleRestart = 1.0f;
	static float scaleQuit = 1.0f;

	int hoverPlay = DrawMenuButton("RESUME", play.x, play.y, play.width, play.height, &scalePlay);
	int hoverRestart = DrawMenuButton("RESTART", restart.x, restart.y, restart.width, restart.height, &scaleRestart);
	int hoverQuit = DrawMenuButton("LEVEL SELECT", quit.x, quit.y, quit.width, quit.height, &scaleQuit);

	if (hoverPlay && CP_Input_MouseClicked())
		return -1;

	if (hoverRestart && CP_Input_MouseClicked())
		return 0;

	if (hoverQuit && CP_Input_MouseClicked())
		return 1;
	return -2;
}

void Pause_Game_Exit(void)
{
	CP_Font_Free(myFont);
	CP_Graphics_ClearBackground(bgColor);
}