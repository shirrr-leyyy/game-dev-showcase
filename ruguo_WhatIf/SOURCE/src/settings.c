// -----------------------------------------------------------------------------
// file:    settings.c
// author:  Yi Ting
// email:   yiting.tin@digipen.edu
//
// brief:   function definitions for the settings control of music volume
// -----------------------------------------------------------------------------
#include "bughunt1.h"
#include "bughunt2.h"
#include "audio.h"
#include "settings.h"
#include "mainmenu.h"
#include "utils.h"
#include "cprocessing.h"
#include <math.h>

// slider struct
typedef struct {
    float x, y;
    float width;
    float value; // 0.0 - 1.0
    int dragging;
    float renderX; // used for knob-only dragging
} Slider;

typedef struct BackBtn {
    float x, y;
    float w, h;
} BackBtn;

static Slider musicSlider;
static Slider sfxSlider;
static Slider* g_active = NULL;

static CP_Color cyan;
static CP_Color light_purple;
static CP_Color white;

static CP_Font font;

static BackBtn backBtn;
static CP_Image arrow;
static CP_Image arrow_hover;

static CP_Image knobImg;
static CP_Image cyanknobImg;

static CP_Image settingsBG;

int fromMain = 0;

extern int g_isLevel2; // 0 = level1, 1 = level2

static void Draw_BackButton(void)
{
    float mx = CP_Input_GetMouseX();
    float my = CP_Input_GetMouseY();

    int hovered = IsAreaHovered(    
        backBtn.x, backBtn.y,
        backBtn.w, backBtn.h,
        mx, my
    );

    // hover swap arrow
    CP_Image currentArrow = hovered ? arrow_hover : arrow;

	// hover swap stroke
    CP_Settings_StrokeWeight(5.0f);
    if (hovered)
        CP_Settings_Stroke(white);
    else
        CP_Settings_Stroke(fromMain ? light_purple : cyan);

    // hover swap fill
    if (hovered)
        CP_Settings_Fill(fromMain ? light_purple : cyan);
    else
        CP_Settings_Fill(white);

    CP_Graphics_DrawRect(backBtn.x, backBtn.y, backBtn.w, backBtn.h);

    CP_Image_Draw(
        currentArrow,
        backBtn.x,
        backBtn.y,
        100.0f,
        100.0f,
        255
    );

    // click handling
    if (IsAreaClicked(backBtn.x, backBtn.y, backBtn.w, backBtn.h, mx, my))
    {
        if (fromMain)
        {
            // return to main menu
            CP_Engine_SetNextGameState(Main_Menu_Init, Main_Menu_Update, Main_Menu_Exit);
        }
        else
        {
            // return to correct game level
            if (!g_isLevel2)
                CP_Engine_SetNextGameStateForced(Bughunt1_Init, Bughunt1_Update, Bughunt1_Exit);
            else
                CP_Engine_SetNextGameStateForced(Bughunt2_Init, Bughunt2_Update, Bughunt2_Exit);
        }
    }
}

void Settings_Init(void)
{
    CP_Settings_AntiAlias(FALSE);
    CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_NEAREST);

    font = CP_Font_Load("Assets/Pixellari.ttf");

    if (fromMain)
    {
        // main menu theme
        settingsBG = CP_Image_Load("Assets/settingsbg.png");
        knobImg = CP_Image_Load("Assets/knob.png");
        arrow = CP_Image_Load("Assets/purple_arrow.png");
        arrow_hover = CP_Image_Load("Assets/arrow.png");

        white = CP_Color_Create(255, 255, 255, 255);
        light_purple = CP_Color_Create(201, 160, 220, 255);
    }
    else
    {
        // bug hunt theme
        settingsBG = CP_Image_Load("Assets/bughuntbg.png");
        knobImg = CP_Image_Load("Assets/bughunt_knob.png");
        arrow = CP_Image_Load("Assets/cyanarrow.png");
        arrow_hover = CP_Image_Load("Assets/arrow.png");

        white = CP_Color_Create(255, 255, 255, 255);
        cyan = CP_Color_Create(107, 210, 224, 255);
    }

    float cx = CP_System_GetWindowWidth() * 0.5f;

    // music
    musicSlider.x = cx;
    musicSlider.y = 380;
    musicSlider.width = 400;
    musicSlider.value = Audio_GetMusicVolume();
    musicSlider.dragging = 0;

    // sfx
    sfxSlider.x = cx;
    sfxSlider.y = 520;
    sfxSlider.width = 400;
    sfxSlider.value = Audio_GetSFXVolume();
    sfxSlider.dragging = 0;

    // back button
    backBtn.x = 75.f;
    backBtn.y = 75.f;
    backBtn.w = 75.f;
    backBtn.h = 75.f;
}

static void DrawSlider(Slider* s, const char* label)
{
    float screenCenter = CP_System_GetWindowWidth() * 0.5f;

    float sliderCenterX = screenCenter + 80;
    float left = sliderCenterX - (s->width / 2);
    float knobX = left + s->value * s->width;

    // label position
    float labelX = sliderCenterX - s->width - 80;
    float labelY = s->y;

    // label
    CP_Settings_TextSize(40);
    CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_MIDDLE);
    CP_Font_DrawText(label, labelX, labelY);

    // bar
    float barHeight = 32;
    float barRadius = 12;
    float filledWidth = s->width * s->value;
    float emptyWidth  = s->width - filledWidth;

    // filled bar
    if (filledWidth > 0)
    {
        CP_Settings_Fill(fromMain ?
            CP_Color_Create(200, 160, 220, 255) :  // purple
            CP_Color_Create(107, 210, 224, 255)); // cyan

        CP_Graphics_DrawRectAdvanced(
            left + filledWidth * 0.5f,
            s->y,
            filledWidth,
            barHeight,
            0, barRadius
        );
    }

    // empty part
    if (emptyWidth > 0)
    {
        CP_Settings_Fill(CP_Color_Create(90, 90, 90, 255));
        CP_Graphics_DrawRectAdvanced(
            left + filledWidth + emptyWidth * 0.5f,
            s->y,
            emptyWidth,
            barHeight,
            0, barRadius
        );
    }

    // knob
    float knobSize = 64;
    CP_Settings_Tint(CP_Color_Create(255, 255, 255, 255));
    CP_Image_Draw(knobImg, knobX, s->y, knobSize, knobSize, 255);
    CP_Settings_Tint(CP_Color_Create(255, 255, 255, 255));
}

void Settings_Update(void)
{
    CP_Image_Draw(settingsBG,
        CP_System_GetWindowWidth() * 0.5f,
        CP_System_GetWindowHeight() * 0.5f,
        (float)CP_System_GetWindowWidth(),
        (float)CP_System_GetWindowHeight(),
        255);
    CP_Font_Set(font);

    float screenCenter = CP_System_GetWindowWidth() * 0.5f;

    // the slider block center
    float sliderCenterX = screenCenter + 80;

    // save slider centers for drawing + dragging
    musicSlider.renderX = sliderCenterX;
    sfxSlider.renderX = sliderCenterX;

    float mx = CP_Input_GetMouseX();
    float my = CP_Input_GetMouseY();

    int held = CP_Input_MouseDown(MOUSE_BUTTON_LEFT);
    int release = CP_Input_MouseReleased(MOUSE_BUTTON_LEFT);


    if (!g_active && held)
    {
        // check music slider
        float left = musicSlider.renderX - musicSlider.width * 0.5f;
        float knobX = left + musicSlider.value * musicSlider.width;
        float knobR = 64 * 0.5f;

        if (fabsf(mx - knobX) < knobR && fabsf(my - musicSlider.y) < knobR)
        {
            g_active = &musicSlider;
        }

        // check sfx slider
        left = sfxSlider.renderX - sfxSlider.width * 0.5f;
        knobX = left + sfxSlider.value * sfxSlider.width;

        if (fabsf(mx - knobX) < knobR && fabsf(my - sfxSlider.y) < knobR)
        {
            g_active = &sfxSlider;
        }
    }

    // drag active slider
    if (g_active && held)
    {
        float left = g_active->renderX - g_active->width * 0.5f;

        float t = (mx - left) / g_active->width;
        if (t < 0) t = 0;
        if (t > 1) t = 1;

        g_active->value = t;
    }

	// stop dragging on release
    if (release)
    {
        g_active = NULL;
    }

	// apply audio
    Audio_SetMusicVolume(musicSlider.value);
    Audio_SetSFXVolume(sfxSlider.value);

	// draw sliders
    DrawSlider(&musicSlider, "Music Volume");
    DrawSlider(&sfxSlider, "SFX Volume");

    // draw back button
    Draw_BackButton();

    CP_Settings_NoStroke();
}


void Settings_Exit(void)
{
    CP_Font_Free(font);
    CP_Image_Free(arrow);
    CP_Image_Free(arrow_hover);
    CP_Image_Free(knobImg);
    CP_Image_Free(settingsBG);
}
