//---------------------------------------------------------
// file:	mainmenu.c
// author:	Shirley, Yi Ting
// email:	shirleywenhui.l@digipen.edu, yiting.tin@digipen.edu 
//
// brief:	Main Menu frame with PLAY, EXIT, SETTINGS and 
//			CREDITS button
//
//---------------------------------------------------------
#include "cprocessing.h"
#include "mainmenu.h"
#include "playerselection.h"
#include "utils.h"
#include "gamecredits.h"
#include "settings.h"
#include "audio.h"

CP_Font myFont;
CP_Image background;

typedef struct Rect {
	float x, y;
	CP_Color color;
	float width, height;
	float line_x1, line_y1;
	float line_x2, line_y2;
	float degree;
	float radius;
	float scale;
}Rect;

Rect play;
Rect credit;
Rect settings;
Rect quit;
CP_Color white;
CP_Color bgColor;
CP_Color light_purple;

static void AnimateButton(Rect* b, int hovered)
{
	float dt = CP_System_GetDt();
	float speed = 10.0f;
	float target = hovered ? 1.08f : 1.0f;

	b->scale += (target - b->scale) * speed * dt;
}

void Main_Menu_Init(void)
{
	white = CP_Color_Create(255, 255, 255, 255);
	light_purple = CP_Color_Create(201, 160, 220, 255);
	bgColor = CP_Color_Create(4, 7, 32, 255);
	myFont = CP_Font_Load("Assets/Pixellari.ttf");

	background = CP_Image_Load("Assets/bg.png");

	play.x = 800.f;
	play.y = 400.f;
	play.width = 300.f;
	play.height = 90.f;
	play.line_x1 = 725.f;
	play.line_x2 = 875.f;
	play.line_y1 = 430.f;
	play.line_y2 = 430.f;
	play.degree = 0;
	play.radius = 16.f;
	play.scale = 1.0f;

	settings.x = 800.f;
	settings.y = 520.f;
	settings.width = 300.f;
	settings.height = 90.f;
	settings.line_x1 = 700.f;
	settings.line_x2 = 900.f;
	settings.line_y1 = 550.f;
	settings.line_y2 = 550.f;
	settings.degree = 0;
	settings.radius = 16.f;
	settings.scale = 1.0f;

	credit.x = 800.f;
	credit.y = 640.f;
	credit.width = 300.f;
	credit.height = 90.f;
	credit.line_x1 = 670.f;
	credit.line_x2 = 935.f;
	credit.line_y1 = 670.f;
	credit.line_y2 = 670.f;
	credit.degree = 0;
	credit.radius = 16.f;
	credit.scale = 1.0f;

	quit.x = 800.f;
	quit.y = 760.f;
	quit.width = 300.f;
	quit.height = 90.f;
	quit.line_x1 = 730.f;
	quit.line_x2 = 870.f;
	quit.line_y1 = 790.f;
	quit.line_y2 = 790.f;
	quit.degree = 0;
	quit.radius = 16.f;
	quit.scale = 1.0f;
}

void Main_Menu_Update(void)
{
	CP_Graphics_ClearBackground(bgColor);
	CP_Image_Draw(background, 800.f, 450.f, 1600.f, 900.f, 255);

	Audio_Update();
	
	// Get Mouse Position
	float mouseX = CP_Input_GetMouseX();
	float mouseY = CP_Input_GetMouseY();

	// Hover states
	int playHover = IsAreaHovered(play.x, play.y, play.width, play.height, mouseX, mouseY);
	int settingsHover = IsAreaHovered(settings.x, settings.y, settings.width, settings.height, mouseX, mouseY);
	int creditsHover = IsAreaHovered(credit.x, credit.y, credit.width, credit.height, mouseX, mouseY);
	int quitHover = IsAreaHovered(quit.x, quit.y, quit.width, quit.height, mouseX, mouseY);

	// Animate scaling
	AnimateButton(&play, playHover);
	AnimateButton(&settings, settingsHover);
	AnimateButton(&credit, creditsHover);
	AnimateButton(&quit, quitHover);

	CP_Settings_StrokeWeight(0.0f);
	
	CP_Font_Set(myFont);
	CP_Settings_TextSize(55.0f * play.scale);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

	// Interactive Color
	CP_Settings_Fill(playHover ? light_purple : white);
	CP_Graphics_DrawRectAdvanced(play.x, play.y, play.width * play.scale, play.height * play.scale, play.degree, play.radius);

	CP_Settings_Fill(playHover ? white : light_purple);
	CP_Font_DrawText("PLAY", play.x, play.y + 5);

	CP_Settings_Fill(settingsHover ? light_purple : white);
	CP_Graphics_DrawRectAdvanced(settings.x, settings.y, settings.width * settings.scale, settings.height * settings.scale, settings.degree, settings.radius);

	// Interactive color
	CP_Settings_TextSize(55.0f * settings.scale);
	CP_Settings_Fill(settingsHover ? white : light_purple);
	CP_Font_DrawText("SETTINGS", settings.x, settings.y + 5);

	// Interactive color
	CP_Settings_Fill(creditsHover ? light_purple : white);
	CP_Graphics_DrawRectAdvanced(credit.x, credit.y, credit.width * credit.scale, credit.height * credit.scale, credit.degree, credit.radius);

	CP_Settings_TextSize(55.0f * credit.scale);
	CP_Settings_Fill(creditsHover ? white : light_purple);
	CP_Font_DrawText("CREDITS", credit.x, credit.y + 5);

	// Interactive color
	CP_Settings_Fill(quitHover ? light_purple : white);
	CP_Graphics_DrawRectAdvanced(quit.x, quit.y, quit.width * quit.scale, quit.height * quit.scale, quit.degree, quit.radius);

	CP_Settings_TextSize(55.0f * quit.scale);
	CP_Settings_Fill(quitHover ? white : light_purple);
	CP_Font_DrawText("EXIT", quit.x, quit.y + 5);

	CP_Settings_Fill(white);
	CP_Settings_TextSize(180.0f);
	CP_Font_DrawText("What If...", 800.f, 240.f);

	// Start Game
	if (IsAreaClicked(play.x, play.y, play.width * play.scale, play.height * play.scale, mouseX, mouseY))
	{
		CP_Engine_SetNextGameState(Player_Selection_Init, Player_Selection_Update, Player_Selection_Exit);
	}

	// Settings page
	if (IsAreaClicked(settings.x, settings.y, settings.width * settings.scale, settings.height * settings.scale, mouseX, mouseY))
	{
		fromMain = 1;
		CP_Engine_SetNextGameState(Settings_Init, Settings_Update, Settings_Exit);
	}
		
	// Credits page
	if (creditsHover && CP_Input_MouseClicked())
	{
		Audio_PlayClick();
		CP_Engine_SetNextGameState(Game_Credits_Init, Game_Credits_Update_v2, Game_Credits_Exit);
	}

	// Terminate Window
	if (IsAreaClicked(quit.x, quit.y, quit.width * quit.scale, quit.height * quit.scale, mouseX, mouseY))
	{
		Audio_Exit();
		CP_Engine_Terminate();
	}

	if (CP_Input_KeyDown(KEY_X)) {
		Audio_Exit();
		CP_Engine_Terminate();
	}
}

void Main_Menu_Exit(void)
{
	CP_Font_Free(myFont);
	CP_Graphics_ClearBackground(bgColor);
	CP_Image_Free(background);
}