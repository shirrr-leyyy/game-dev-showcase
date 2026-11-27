//---------------------------------------------------------
// file:	payslip.c
// author:	Shirley
// email:	shirleywenhui.l@digipen.edu 
//
// brief:	Payslip showing earning of each career
//
//---------------------------------------------------------

#include "cprocessing.h"
#include "score.h"
#include "utils.h"
#include "payslip.h"
#include "engineerlevels.h"
#include "audio.h"
#include <stdio.h>

CP_Font myFont;

CP_Color white;
CP_Color bgColor;
CP_Color light_purple;

CP_Image profile;
CP_Image printing_img;

int total = 0;
int game_1 = 0;
int game_2 = 0;
int game_3 = 0;
int game_4 = 0;
int monthlySalaries[12] = {0};

static float timer = 0.0f;
static int printing_time = 0;
static int finished = 0;
static int audio = 0;

static const char* months[14] = {
	"Month",
	"January",
	"February",
	"March",
	"April",
	"May",
	"June",
	"July",
	"August",
	"September",
	"October",
	"November",
	"December",
	"Total"
};

static char salary[14][10] = {
	"Amount",
	"-",
	"-",
	"-",
	"-",
	"-",
	"-",
	"-",
	"-",
	"-",
	"-",
	"-",
	"-",
	"-"
};

void Payslip_Init(void)
{
	white = CP_Color_Create(255, 255, 255, 255);
	light_purple = CP_Color_Create(201, 160, 220, 255);
	bgColor = CP_Color_Create(4, 7, 32, 255);
	myFont = CP_Font_Load("Assets/Pixellari.ttf");
	printing_img = CP_Image_Load("Assets/printer.png");

}


int Payslip_Update(int type)
{
	CP_Graphics_ClearBackground(bgColor);
	float w_width = (float) CP_System_GetWindowWidth() / 2;
	float w_height = (float) CP_System_GetWindowHeight() / 2;
	float dt = CP_System_GetDt();
	
	CP_Settings_AntiAlias(FALSE);
	CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_NEAREST);
	CP_Settings_RectMode(CP_POSITION_CENTER);
	CP_Settings_ImageMode(CP_POSITION_CENTER);

	switch (printing_time)
	{
	case 0: printing_img = CP_Image_Load("Assets/printer.png"); break;
	case 1: printing_img = CP_Image_Load("Assets/printer1.png"); break;
	case 2: printing_img = CP_Image_Load("Assets/printer2.png");  break;
	case 3: printing_img = CP_Image_Load("Assets/printer3.png"); break;
	case 4: printing_img = CP_Image_Load("Assets/printer4.png"); break;
	case 5: printing_img = CP_Image_Load("Assets/printer5.png"); break;
	case 6: printing_img = CP_Image_Load("Assets/printer6.png"); break;
	default:
		printing_img = CP_Image_Load("Assets/printer6.png"); break;
	}

	if (!finished)
	{
		timer += dt;

		// Every 0.5s change to the next image
		if (timer >= 0.2f)
		{
			printing_time++;
			timer = 0.0f;
		}
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
		CP_Font_DrawText("Generating Payslip...", w_width, 200.f);
		CP_Image_Draw(printing_img, w_width, w_height, 800.f, 800.f, 255);
		if (printing_time == 8) {
			finished = 1;
		}
		if (!audio) {
			Audio_Printing();
			audio = 1;
		}
	}



	if (finished) {

		//Draw payslip
		CP_Settings_Fill(white);
		CP_Settings_StrokeWeight(5.f);
		CP_Graphics_DrawRect(w_width, w_height, 740.f, 900.f);

		CP_Settings_TextSize(80.0f);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_MIDDLE);
		CP_Settings_Fill(CP_Color_Create(0,0,0,255));
		CP_Font_DrawText("PAYSLIP", 503.f, 103.f);
		CP_Settings_Fill(light_purple);
		CP_Font_DrawText("PAYSLIP", 500.f, 100.f);

		CP_Settings_TextSize(30.0f);
		CP_Font_DrawText("WHAT IF", 990.f, 100.f);
		
		CP_Settings_TextSize(18.0f);
		CP_Font_DrawText("Pay Period: Year 2025", 505.f, 144.f);


		if (type == 1) {
			profile = CP_Image_Load("Assets/dishwasher.png");
			CP_Font_DrawText("Occupation: Dishwasher", 505.f, 159.f);
			game_1 = (Score_GetHigh(1) >= 100) ? 2400 : ((Score_GetHigh(1) >= 70) ? 2200 : (Score_GetHigh(1) > 0) ? 2000 : 0);
			game_2 = (Score_GetHigh(2) >= 60) ? 2400 : ((Score_GetHigh(2) >= 40) ? 2200 : (Score_GetHigh(2) > 0) ? 2000 : 0);
			game_3 = (Score_GetHigh(3) >= 70) ? 2400 : ((Score_GetHigh(3) >= 60) ? 2200 : (Score_GetHigh(3) > 0) ? 2000 : 0);
			game_4 = (Score_GetHigh(4) >= 110) ? 2400 : ((Score_GetHigh(4) >= 90) ? 2200 : (Score_GetHigh(4) > 0) ? 2000 : 0);
		}
		else {
			profile = CP_Image_Load("Assets/softwareengineer.png");
			CP_Font_DrawText("Occupation: Software Engineer", 505.f, 159.f);
			game_1 = (Score_GetHigh(5) >= 120) ? 6000 : ((Score_GetHigh(5) >= 90) ? 5500 : (Score_GetHigh(5) > 0) ? 5000 : 0);
			game_2 = (Score_GetHigh(6) >= 150) ? 6000 : ((Score_GetHigh(6) >= 120) ? 5500 : (Score_GetHigh(6) > 0) ? 5000 : 0);
			game_3 = (Score_GetHigh(7) >= 100) ? 6000 : ((Score_GetHigh(7) >= 60) ? 5500 : (Score_GetHigh(7) > 0) ? 5000 : 0);
			game_4 = (Score_GetHigh(8) >= 50) ? 6000 : ((Score_GetHigh(8) >= 30) ? 5500 : (Score_GetHigh(8) > 0) ? 5000 : 0);

		}

		total = game_1 * 3 + game_2 * 3 + game_3 * 3 + game_4 * 3;

		if (total) {
			sprintf_s(salary[13], 10, "$%d", total);
		}

		CP_Image_Draw(profile, 1050, 150, 200.f, 200.f, 255);

		for (int i = 0; i < 12; ++i) {
			int patternIndex = i / 3; // 0 for months 0-2, 1 for 3-5, 2 for 6-8, 3 for 9-11

			if (patternIndex == 0 && game_1) {
				sprintf_s(salary[i + 1], 10, "$%d", game_1);
			}
			else if (patternIndex == 1 && game_2) {
				sprintf_s(salary[i + 1], 10, "$%d", game_2);
			}
			else if (patternIndex == 2 && game_3) {
				sprintf_s(salary[i + 1], 10, "$%d", game_3);
			}
			else if (game_4){ // patternIndex == 3
				sprintf_s(salary[i + 1], 10, "$%d", game_4);
			}
		}

		CP_Settings_StrokeWeight(0.f);
		for (int i = 0; i < 14; ++i) {
			CP_Settings_Fill(CP_Color_Create(240, 240, 240, 255));
			if ((i % 2) == 1) {
				CP_Graphics_DrawRect(w_width, 300.f + i * 30.f, 700.f, 30.f);
			}
			CP_Settings_Fill(light_purple);
			CP_Settings_TextSize(24.0f);
			CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_MIDDLE);
			CP_Font_DrawText(months[i], 530, 303.f + i * 30.f);

			CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_RIGHT, CP_TEXT_ALIGN_V_MIDDLE);
			CP_Font_DrawText(salary[i], 1100, 303.f + i * 30.f);
		}

		CP_Settings_TextSize(18.0f);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE); // Set alignment for the salary column
		CP_Font_DrawText("This is system generated payslip", w_width, 850.f);
	}
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_MIDDLE); // Set alignment for the salary column
	CP_Settings_Fill(CP_Color_Create(196,196,196,245));
	CP_Settings_TextSize(30);
	CP_Font_DrawText("Press ESC to return", 40, 40);

	if (CP_Input_KeyTriggered(KEY_ESCAPE))
	{
		finished = 0;
		printing_time = 0;
		audio = 0;
		return 1;
	}
	return 0;
}

void Payslip_Exit(void){
	CP_Font_Free(myFont);
	CP_Image_Free(profile);
	CP_Image_Free(printing_img);
	CP_Graphics_ClearBackground(bgColor);
}