// -----------------------------------------------------------------------------
// file:    bugs_ui.c
// author:  Yi Ting
// email:   yiting.tin@digipen.edu
//
// brief:   function definitions for the user interface system in Bug Hunt games
//          handles level UI layout, tab navigation, pause menu,
//          how-to-play tutorial pages, animated bug demo,
//          game-over summary panel, timer rendering, and UI animations
// -----------------------------------------------------------------------------

#include "bugs_ui.h"
#include "mainmenu.h"
#include "utils.h"
#include "bughunt1.h"
#include "bughunt2.h"
#include "coffeedriven1.h"
#include "audio.h"
#include "score.h"
#include "settings.h"
#include "cprocessing.h"
#include "engineerlevels.h"

#include <stdio.h>

static CP_Font font;
static float g_winW, g_winH;

int g_isLevel2   = 0; // 0 = level 1, 1 = level 2
int g_showHTP    = 0; // which htp page is being shown
int g_seenHTP    = 0; // first time only display
int g_currentTab = 1; // 1 level, 2 htp, 3 pause

// pause menu scale animations
float pauseScaleResume   = 1.0f;
float pauseScaleRestart  = 1.0f;
float pauseScaleSettings = 1.0f;
float pauseScaleBack     = 1.0f;

// game over animation
float homeScale  = 1.0f;
float retryScale = 1.0f;
float nextScale  = 1.0f;

// delayed button actions for game over
int pendingAction = 0;
float actionTimer = 0.0f;

// tutorial bug
static CP_Image tutBug;
static CP_Image tutDeath[3];

static float tutX = 0.0f;
static float tutY = 0.0f;
static float tutW = 180.0f;
static float tutH = 180.0f;

static int   tutState = 0; // 0 = alive, 1 = death anim, 2 = dead wait
static int   tutFrame = 0;
static float tutTimer = 0.0f;

//static float tutDeathSpeed = 0.15f; // 0.15s per frame

// page 1 fades
static float pg1_line1 = 0.0f;
static float pg1_line2 = -60.0f;
static float fadeAlpha = 0.0f;

// page 2 fades
static float pg2_line1 = 0.0f;
static float pg2_line2 = -50.0f;
static float pg2_line3 = -100.0f;
static float pg2_line4 = -150.0f;
static float pg2_line5 = -200.0f;
static float pg2_line6 = -250.0f;

// next/done button fades + scales
static float nextFade = 0.0f;
static float doneFade = 0.0f;
static float nextScaleHTP = 1.0f;
static float doneScaleHTP = 1.0f;
static float backScale = 1.0f;

// reset all fade & scale states for HTP
static void ResetHowToPlayFades(void)
{
    pg1_line1 = 0.0f;
    pg1_line2 = -60.0f;
    fadeAlpha = 0.0f;

    pg2_line1 = 0.0f;
    pg2_line2 = -50.0f;
    pg2_line3 = -100.0f;
    pg2_line4 = -150.0f;
    pg2_line5 = -200.0f;
    pg2_line6 = -250.0f;

    nextFade = 0.0f;
    doneFade = 0.0f;

    nextScaleHTP = 1.0f;
    doneScaleHTP = 1.0f;
    backScale = 1.0f;
}

void UI_Init(void)
{
    font = CP_Font_Load("Assets/Pixellari.ttf");

    g_winW = (float)CP_System_GetWindowWidth();
    g_winH = (float)CP_System_GetWindowHeight();

    CP_Settings_AntiAlias(FALSE);
    CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_NEAREST);

    // load tutorial bug images
    tutBug = CP_Image_Load("Assets/bug_4.png");
    tutDeath[0] = CP_Image_Load("Assets/bug_4_dead1.png");
    tutDeath[1] = CP_Image_Load("Assets/bug_4_dead2.png");
    tutDeath[2] = CP_Image_Load("Assets/bug_dead3.png");


    // tutorial animation state
    tutState = 0;
    tutFrame = 0;
    tutTimer = 0.0f;

    // reset pause menu scales
    pauseScaleResume   = 1.0f;
    pauseScaleRestart  = 1.0f;
    pauseScaleSettings = 1.0f;
    pauseScaleBack     = 1.0f;

    // reset transition state
    pendingAction = 0;

    // default tab
    g_currentTab = 1;

    // reset fade animations
    ResetHowToPlayFades();
}

void UI_Level1(void)
{
    g_isLevel2 = 0; // level1
}

void UI_Level2(void)
{
    g_isLevel2 = 1; // level2
}

// toggle pause between level tab (1) and pause tab (3)
void TogglePause(void)
{
    if (g_currentTab == 3)
        g_currentTab = 1; // return to gameplay
    else
		g_currentTab = 3; // show pause menu
}

// draws the how to play panel
void DrawHowToPlay(void)
{
    // line spacing
    float spacing = 45.0f;

    float mx = CP_Input_GetMouseX();
    float my = CP_Input_GetMouseY();

    // background overlay
    CP_Settings_Fill(CP_Color_Create(0, 0, 0, 180));
    CP_Settings_NoStroke();
    CP_Graphics_DrawRect(g_winW / 2, g_winH / 2, g_winW, g_winH);

    // panel
    float panelW = 700;
    float panelH = 560;
    float cx = g_winW / 2;
    float cy = g_winH / 2;

    // panel background
    CP_Settings_Fill(CP_Color_Create(25, 25, 25, 255));
    CP_Graphics_DrawRect(cx, cy, panelW, panelH);

    // top bar
    CP_Settings_Fill(CP_Color_Create(45, 45, 45, 255));
    CP_Graphics_DrawRect(cx, cy - panelH / 2 + 25, panelW, 50);

    // title
    CP_Font_Set(font);
    CP_Settings_TextSize(30);
    CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_MIDDLE);
    CP_Font_DrawText("How To Play", cx - panelW / 2 + 25, cy - panelH / 2 + 27);

    // esc hint
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_RIGHT, CP_TEXT_ALIGN_V_MIDDLE);
    CP_Settings_TextSize(18);
    CP_Settings_Fill(CP_Color_Create(200, 200, 200, 255));
    CP_Font_DrawText("[ESC to close]", cx + panelW / 2 - 25, cy - panelH / 2 + 25);


    if (g_showHTP == 1)
    {
        float tx = cx - panelW / 2 + 35.0f;
        float ty = cy - panelH / 2 + 90.0f;

        float lineY = ty;

        float dt = CP_System_GetDt();

        // fade update
        pg1_line1 += dt * 200.0f;
        if (pg1_line1 > 255)
            pg1_line1 = 255;

        pg1_line2 += dt * 200.0f;
        if (pg1_line2 > 255)
            pg1_line2 = 255;

        CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_TOP);
        CP_Settings_TextSize(25);

        // draw line 1
        CP_Settings_Fill(CP_Color_Create(220, 220, 220, (int)pg1_line1));
        CP_Font_DrawText("> Click bugs to eliminate them.", tx, lineY);
        lineY += spacing;

        // draw line 2
        CP_Settings_Fill(CP_Color_Create(220, 220, 220, (int)pg1_line2));
        CP_Font_DrawText("> Try clicking the bug below to continue.", tx, lineY);
        lineY += spacing;

        tutX = cx;
        tutY = ty + 150.0f;

		// state 0 - bug alive
        if (tutState == 0)
        {
            CP_Image_Draw(tutBug, tutX, tutY, tutW, tutH, 255);

            int clicked =
                (mx >= tutX - tutW / 2 && mx <= tutX + tutW / 2 &&
                    my >= tutY - tutH / 2 && my <= tutY + tutH / 2);

            if (clicked && CP_Input_MouseClicked())
            {
                Audio_PlayBugDeath();
                tutState = 1;
                tutFrame = 0;
                tutTimer = 0.0f;
            }
        }
		// state 1 — death animation
        else if (tutState == 1)
        {
            tutTimer += dt;

            if (tutTimer >= 0.15f)
            {
                tutTimer = 0.0f;
                tutFrame++;

                if (tutFrame >= 3)
                {
                    tutFrame = 2;
                    tutState = 2; // go to state 2
                    tutTimer = 0.0f;
                }
            }

            CP_Image_Draw(tutDeath[tutFrame], tutX, tutY, tutW, tutH, 255);
        }

        // state 2 - respawn bug
        else if (tutState == 2)
        {
            CP_Image_Draw(tutDeath[2], tutX, tutY, tutW, tutH, 255);

            tutTimer += dt;
            if (tutTimer >= 0.25f)
            {
                tutState = 0;
                tutFrame = 0;
                tutTimer = 0.0f;
            }
        }

        if (tutState == 1 || tutState == 2)
            fadeAlpha += dt * 350.0f;

        if (fadeAlpha > 255) 
            fadeAlpha = 255;

        if (fadeAlpha > 0)
        {
            int a1 = (int)fadeAlpha;
            int a2 = (int)(fadeAlpha - 60);
            if (a2 < 0) a2 = 0;

            float textY = tutY + tutH / 2;

            CP_Settings_TextSize(25);

            CP_Settings_Fill(CP_Color_Create(200, 200, 200, a1));
            CP_Font_DrawText("> Bugs move toward your servers constantly.", tx, textY);
            textY += spacing;

            CP_Settings_Fill(CP_Color_Create(200, 200, 200, a2));
            CP_Font_DrawText("> If a bug reaches a server,", tx, textY);
            textY += spacing;

            CP_Settings_Fill(CP_Color_Create(200, 200, 200, a2));
            CP_Font_DrawText("   the server loses HP and you lose points.", tx, textY);
            textY += spacing;
        }

        int textDone =
            pg1_line1 >= 255 &&
            pg1_line2 >= 255 &&
            fadeAlpha >= 255;

        float btnW = 180.0f;
        float btnH = 55.0f;

        float bx = cx + panelW / 2.0f - btnW / 2.0f - 30.0f;
        float by = cy + panelH / 2.0f - btnH / 2.0f - 40.0f;

        if (textDone)
            nextFade += dt * 300.0f;

        if (nextFade > 255) 
            nextFade = 255;

        int hovered =
            (mx > bx - btnW / 2 && mx < bx + btnW / 2 &&
                my > by - btnH / 2 && my < by + btnH / 2); 

        nextScaleHTP += (hovered ? (1.2f - nextScaleHTP) : (1.0f - nextScaleHTP)) * 10.0f * dt;

        CP_Settings_StrokeWeight(3);
        CP_Settings_Stroke(CP_Color_Create(107, 210, 224, (int)nextFade));
        CP_Settings_NoFill();
        CP_Graphics_DrawRect(bx, by, btnW* nextScaleHTP, btnH* nextScaleHTP);

        CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
        CP_Settings_TextSize(26 * nextScaleHTP);
        CP_Settings_Fill(CP_Color_Create(255, 255, 255, (int)nextFade));
        CP_Font_DrawText("Next", bx, by);


        if (hovered && CP_Input_MouseClicked() && nextFade >= 255)
        {
            Audio_PlayClick();
            g_showHTP = 2; // switch to page 2
            ResetHowToPlayFades();
            return;
        }
    }

    // page 2
    if (g_showHTP == 2)
    {
        float tx = cx - panelW / 2 + 35.0f;
        float ty = cy - panelH / 2 + 90.0f;
        float lineY = ty;

        float dt = CP_System_GetDt();

        // update line fades
        pg2_line1 += dt * 120.0f;
        if (pg2_line1 > 255)
            pg2_line1 = 255;

        pg2_line2 += dt * 120.0f;
        if (pg2_line2 > 255)
            pg2_line2 = 255;

        pg2_line3 += dt * 120.0f;
        if (pg2_line3 > 255)
            pg2_line3 = 255;

        pg2_line4 += dt * 120.0f;
        if (pg2_line4 > 255)
            pg2_line4 = 255;

        pg2_line5 += dt * 120.0f;
        if (pg2_line5 > 255)
            pg2_line5 = 255;

        pg2_line6 += dt * 120.0f;
        if (pg2_line6 > 255)
            pg2_line6 = 255;

        CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_TOP);
        CP_Settings_TextSize(25);

        CP_Settings_Fill(CP_Color_Create(220, 220, 220, (int)pg2_line1));
        CP_Font_DrawText("> Each bug kill rewards points based on your", tx, lineY);
        lineY += spacing;
        CP_Font_DrawText("   streak tier.", tx, lineY);
        lineY += spacing;

        CP_Settings_Fill(CP_Color_Create(220, 220, 220, (int)pg2_line2));
        CP_Font_DrawText("> Build streaks by eliminating bugs without missing.", tx, lineY);
        lineY += spacing;

        CP_Settings_Fill(CP_Color_Create(220, 220, 220, (int)pg2_line3));
        CP_Font_DrawText("> Higher streak tiers give bonus points.", tx, lineY);
        lineY += spacing;

        CP_Settings_Fill(CP_Color_Create(220, 220, 220, (int)pg2_line4));
        CP_Font_DrawText("> Misclicks reset your streak and reduce your score.", tx, lineY);
        lineY += spacing;

        CP_Settings_Fill(CP_Color_Create(220, 220, 220, (int)pg2_line5));
        CP_Font_DrawText("> If all servers crash, the system shuts down", tx, lineY);
        lineY += spacing;
        CP_Font_DrawText("   and game over.", tx, lineY);
        lineY += spacing;

        CP_Settings_Fill(CP_Color_Create(220, 220, 220, (int)pg2_line6));
        CP_Font_DrawText("> Survive until the timer ends to complete the level.", tx, lineY);

        // buttons
        float btnW = 180.0f;
        float btnH = 55.0f;

        float bxBack = cx - panelW / 2 + btnW / 2 + 30.0f;
        float by = cy + panelH / 2 - btnH / 2 - 40.0f;

        float bxDone = cx + panelW / 2 - btnW / 2 - 30.0f;

        // back button
        int backHover =
            (mx > bxBack - btnW / 2 && mx < bxBack + btnW / 2 &&
                my > by - btnH / 2 && my < by + btnH / 2);

        if (backHover)
            backScale = 1.20f;
        else
            backScale += (1.0f - backScale) * 10.0f * dt;

        CP_Settings_StrokeWeight(3);
        CP_Settings_Stroke(CP_Color_Create(107, 210, 224, 255));
        CP_Settings_NoFill();
        CP_Graphics_DrawRect(bxBack, by, btnW* backScale, btnH* backScale);

        CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
        CP_Settings_TextSize(26 * backScale);
        CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
        CP_Font_DrawText("Back", bxBack, by);

        if (backHover && CP_Input_MouseClicked())
        {
            Audio_PlayClick();
            g_showHTP = 1;      // return to page 1
            ResetHowToPlayFades();
            return;
        }

        // done button
        int page2Done =
            pg2_line1 >= 255.0f &&
            pg2_line2 >= 255.0f &&
            pg2_line3 >= 255.0f &&
            pg2_line4 >= 255.0f &&
            pg2_line5 >= 255.0f &&
            pg2_line6 >= 255.0f;

        if (page2Done)
            doneFade += dt * 500.0f;

        if (doneFade > 255) 
            doneFade = 255;

        int doneHover =
            (mx > bxDone - btnW / 2 && mx < bxDone + btnW / 2 &&
                my > by - btnH / 2 && my < by + btnH / 2);

        if (doneHover)
            doneScaleHTP = 1.20f;
        else
            doneScaleHTP += (1.0f - doneScaleHTP) * 10.0f * dt;

        CP_Settings_StrokeWeight(3);
        CP_Settings_Stroke(CP_Color_Create(107, 210, 224, (int)doneFade));
        CP_Settings_NoFill();
        CP_Graphics_DrawRect(bxDone, by, btnW * doneScaleHTP, btnH * doneScaleHTP);

        CP_Settings_TextSize(26 * doneScaleHTP);
        CP_Settings_Fill(CP_Color_Create(255, 255, 255, (int)doneFade));
        CP_Font_DrawText("Done", bxDone, by);

        if (doneHover && CP_Input_MouseClicked() && doneFade >= 255.0f)
        {
            Audio_PlayClick();
            g_showHTP = 0;
            g_currentTab = 1;
            ResetHowToPlayFades();
            return;
        }
    }

    // esc closes htp
    if (CP_Input_KeyTriggered(KEY_ESCAPE))
    {
        g_showHTP = 0;
        g_currentTab = 1;
        ResetHowToPlayFades();
    }
}

// pause menu button
int DrawPauseButton(const char* label, float bx, float by, float* scale, float btnW, float btnH)
{
    float mx = CP_Input_GetMouseX();
    float my = CP_Input_GetMouseY();

    float dt = CP_System_GetDt();

    int hovered = (mx >= bx - btnW / 2 && mx <= bx + btnW / 2 &&
        my >= by - btnH / 2 && my <= by + btnH / 2);

    if (hovered)
        *scale = 1.15f;
    else
        *scale += (1.0f - *scale) * 10.0f * dt;

	// button outline
    CP_Settings_StrokeWeight(4);
    CP_Settings_Stroke(CP_Color_Create(107, 210, 224, 255));
    CP_Settings_NoFill();
    CP_Graphics_DrawRect(bx, by, btnW * (*scale), btnH * (*scale));

    // text
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
    CP_Settings_TextSize(32 * (*scale));
    CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
    CP_Font_DrawText(label, bx, by);

    CP_Settings_NoStroke();

    return hovered;
}

// draw psuse menu
// draws pause overlay, pause menu buttons, and handles clicks
void DrawPauseMenu(void)
{
    float cx = g_winW / 2.0f;
    float cy = g_winH / 2.0f;

    // button layout
    float btnW = 380.0f;
    float btnH = 75.0f;

    float spacing = 35.0f;

    // vertical space per section
    float totalBlockH = 100 + (btnH * 4) + (spacing * 4);   // full screen height

    float startY = cy - (totalBlockH / 2.0f);

    // button positions
    float titleY = startY + 40.0f;
    float resumeY = titleY + 120.0f + spacing;
    float restartY = resumeY + btnH + spacing;
    float settingsY = restartY + btnH + spacing;
    float backY = settingsY + btnH + spacing;

    // background overlay
    CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
    CP_Settings_NoStroke();
    CP_Graphics_DrawRect(g_winW / 2, g_winH / 2, g_winW, g_winH);

    // paused title
    CP_Font_Set(font);
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
    CP_Settings_TextSize(160);

    // shadow (cyan)
    CP_Settings_Fill(CP_Color_Create(107, 210, 224, 255));
    CP_Font_DrawText("PAUSED", cx + 8, cy - 192);  // small offset

    // main text (white)
    CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
    CP_Font_DrawText("PAUSED", cx, cy - 200);

    int hResume = DrawPauseButton("RESUME", cx, resumeY, &pauseScaleResume, btnW, btnH);
    int hRestart = DrawPauseButton("RESTART", cx, restartY, &pauseScaleRestart, btnW, btnH);
    int hSettings = DrawPauseButton("SETTINGS", cx, settingsY, &pauseScaleSettings, btnW, btnH);
    int hBack = DrawPauseButton("LEVEL SELECT", cx, backY, &pauseScaleBack, btnW, btnH);

    // handle clicks
    if (CP_Input_MouseClicked())
    {
        // resume
        if (hResume)
        {
            TogglePause();
            Audio_PlayClick();
        }
        // restart level
        else if (hRestart)
        {
            Audio_PlayClick();
            TogglePause();

            if (!g_isLevel2)
                CP_Engine_SetNextGameStateForced(Bughunt1_Init, Bughunt1_Update, Bughunt1_Exit);
            else
                CP_Engine_SetNextGameStateForced(Bughunt2_Init, Bughunt2_Update, Bughunt2_Exit);
        }
		// open settings
        else if (hSettings)
        {
            Audio_PlayClick();
            fromMain = 0;
            CP_Engine_SetNextGameState(Settings_Init, Settings_Update, Settings_Exit);
        }
		// return to level select

        else if (hBack)
        {
            Audio_PlayClick();
            TogglePause();
            CP_Engine_SetNextGameState(Engineer_Levels_Init, Engineer_Levels_Update, Engineer_Levels_Exit);
        }
    }
}

// main ui
void DrawUI(void)
{
    g_winW = (float)CP_System_GetWindowWidth();
    g_winH = (float)CP_System_GetWindowHeight();

    // ui layout
    float border = 10.0f;
    float titleH = 45.0f;
    float tabH = 40.0f;

    // colors
    CP_Color borderCol = CP_Color_Create(20, 20, 20, 255);
    CP_Color titleBar = CP_Color_Create(20, 20, 20, 255);
    CP_Color textCol = CP_Color_Create(255, 255, 255, 255);
    CP_Color hoverRed = CP_Color_Create(255, 80, 80, 255);

	// remove stroke
    CP_Settings_NoStroke();

    // outer borders
    CP_Settings_Fill(borderCol);
    CP_Graphics_DrawRect(g_winW / 2, g_winH - border / 2, g_winW, border);
    CP_Graphics_DrawRect(border / 2, g_winH / 2, border, g_winH);
    CP_Graphics_DrawRect(g_winW - border / 2, g_winH / 2, border, g_winH);

    // title bar
    CP_Settings_Fill(titleBar);
    CP_Graphics_DrawRect(g_winW / 2, titleH / 2, g_winW - border * 2, titleH);

    // title text
    CP_Font_Set(font);
    CP_Settings_TextSize(24);
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_MIDDLE);
    CP_Settings_Fill(textCol);
    CP_Font_DrawText("Bug Hunt", border + 10, titleH / 2);

    // close button 
    float mx = CP_Input_GetMouseX();
    float my = CP_Input_GetMouseY();

    float xBtnX = g_winW - border - 25;
    float xBtnY = titleH / 2;

    int hoverClose = (mx > g_winW - 70 && mx < g_winW - 10 &&
        my > border && my < titleH);

    CP_Settings_Fill(hoverClose ? hoverRed : textCol);
    CP_Font_DrawText("X", xBtnX, xBtnY);

    if (hoverClose && CP_Input_MouseClicked())
    {
        Audio_PlayClick();
        CP_Engine_SetNextGameState(Main_Menu_Init, Main_Menu_Update, Main_Menu_Exit);
		return;
    }

    // tabs row
    float tabY = titleH + tabH / 2.0f;
    float tabW = 130.0f;
    float gap = 5.0f;

    float tab1X = border + tabW / 2; // level tab
    float tab2X = tab1X + tabW + gap; // howtoplay
    float tab3X = tab2X + tabW + gap; // pause

    // tab strip background
    CP_Settings_Fill(CP_Color_Create(37, 37, 38, 255));
    CP_Graphics_DrawRect(g_winW / 2, tabY, g_winW - border * 2, tabH);

    CP_Color active = CP_Color_Create(107, 210, 224, 255);
    CP_Color inactive = CP_Color_Create(60, 60, 75, 255);

    // hover checks
    int hover1 = (mx >= tab1X - tabW / 2 && mx <= tab1X + tabW / 2 &&
        my >= tabY - tabH / 2 && my <= tabY + tabH / 2);

    int hover2 = (mx >= tab2X - tabW / 2 && mx <= tab2X + tabW / 2 &&
        my >= tabY - tabH / 2 && my <= tabY + tabH / 2);

    int hover3 = (mx >= tab3X - tabW / 2 && mx <= tab3X + tabW / 2 &&
        my >= tabY - tabH / 2 && my <= tabY + tabH / 2);

    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

    // tab 1: level
    CP_Settings_Fill(g_currentTab == 1 ? active : inactive);
    CP_Graphics_DrawRect(tab1X, tabY, tabW, tabH);
    CP_Settings_TextSize(20);
    CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
    CP_Font_DrawText(g_isLevel2 ? "Level2.c" : "Level1.c", tab1X, tabY);

    if (hover1 && CP_Input_MouseClicked())
    {
        g_currentTab = 1; // switch tab
        g_showHTP = 0;  // close howtoplay
        Audio_PlayClick();
    }

    // tab 2: how to play
    CP_Settings_Fill(g_currentTab == 2 ? active : inactive);
    CP_Graphics_DrawRect(tab2X, tabY, tabW, tabH);

    CP_Settings_TextSize(18);
    CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
    CP_Font_DrawText("HowToPlay.c", tab2X, tabY);

    if (hover2 && CP_Input_MouseClicked())
    {
        g_currentTab = 2;
        g_showHTP = 1;
        Audio_PlayClick();
    }

    // tab 3: pause
    CP_Settings_Fill(g_currentTab == 3 ? active : inactive);
    CP_Graphics_DrawRect(tab3X, tabY, tabW, tabH);

    CP_Settings_TextSize(18);
    CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
    CP_Font_DrawText("Pause.c", tab3X, tabY);

    if (hover3 && CP_Input_MouseClicked())
    {
        g_currentTab = 3;
        Audio_PlayClick();
    }

    // draw overlays based on current tab
    if (g_currentTab == 2)
    {
        DrawHowToPlay();
        return;
    }
    else if (g_currentTab == 3)
    {
        DrawPauseMenu();
    }
}

// draw game over buttons
int DrawGameOverButton(const char* label, float x, float y, float* scale)
{
    float mx = CP_Input_GetMouseX();
    float my = CP_Input_GetMouseY();

    float dt = CP_System_GetDt();

    // button hitbox
    float halfW = 60.0f * (*scale);
    int hovered =
        (mx >= x - halfW && mx <= x + halfW &&
            my >= y - 20.0f && my <= y + 20.0f);

    // scale animation
    if (hovered)
        *scale = 1.25f;
    else
        *scale += (1.0f - *scale) * 8.0f * dt;

    // colors
    CP_Color normal = CP_Color_Create(255, 255, 255, 255);
    CP_Color cyan = CP_Color_Create(107, 210, 224, 255);

    // draw left arrow
    if (hovered)
    {
        CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_MIDDLE);
        CP_Settings_TextSize(32 * (*scale));
        CP_Settings_Fill(cyan);
        CP_Font_DrawText(">", x - 95, y);
    }

    // draw text
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
    CP_Settings_TextSize(32 * (*scale));
    CP_Settings_Fill(hovered ? cyan : normal);
    CP_Font_DrawText(label, x, y);

    return hovered;
}

// game over panel
void DrawGameOver(void)
{
    g_winW = (float)CP_System_GetWindowWidth();
    g_winH = (float)CP_System_GetWindowHeight();

    float dt = CP_System_GetDt();

    // handle delayed button transitions
    if (pendingAction != 0)
    {
        // scale animations
        homeScale += (1.0f - homeScale) * 10.0f * dt;
        retryScale += (1.0f - retryScale) * 10.0f * dt;
        nextScale += (1.0f - nextScale) * 10.0f * dt;

        return;
    }

    float panelW = 800.0f;
    float panelH = 400.0f;

    // background overlay
    CP_Settings_Fill(CP_Color_Create(0, 0, 0, 180));
    CP_Graphics_DrawRect(g_winW / 2.0f, g_winH / 2.0f, (float)g_winW, (float)g_winH);

    // panel background
    CP_Settings_Fill(CP_Color_Create(20, 20, 20, 240));
    CP_Graphics_DrawRect(g_winW / 2.0f, g_winH / 2.0f, panelW, panelH);

    // panel top bar
    CP_Settings_Fill(CP_Color_Create(45, 45, 45, 255));
    CP_Graphics_DrawRect(g_winW / 2.0f,
        g_winH / 2.0f - panelH / 2 + 20,
        panelW, 80);

    // blinking header text
    int blink = (((int)(CP_System_GetSeconds() * 2)) % 2 == 0) ? '_' : ' ';
    char header[64];
    sprintf_s(header, sizeof(header), "Debug Summary %c", (char)blink);

    CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
    CP_Settings_TextSize(40);
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_TOP);
    CP_Font_DrawText(header,
        g_winW / 2.0f - panelW / 2 + 15,
        g_winH / 2.0f - panelH / 2 + 5);

    // stats
    int score = Score_Get();
    int levelIndex = g_isLevel2 ? 6 : 5; // level 1 stored at 5, level 2 at 6
    int highScore = Score_GetHigh(levelIndex);
    int streak = Score_GetStreak();
    int highStreak = Score_GetHighStreak(levelIndex);

    int passed = g_isLevel2 ? (score >= 130) : (score >= 90);

    // status message color
    CP_Settings_TextSize(30 );
    CP_Settings_Fill(passed ?
        CP_Color_Create(120, 255, 150, 255) :
        CP_Color_Create(255, 120, 120, 255));

    const char* statusMsg = passed ?
        "> Status: Code executed successfully! See logs below." :
        "> Status: Code failed to run. See logs below.";

    CP_Font_DrawText(statusMsg,
        g_winW / 2 - panelW / 2 + 20,
        g_winH / 2 - 100);

    // summary lines
    CP_Settings_TextSize(28);
    CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));

    float y = g_winH / 2 - 60;
    float leftX = g_winW / 2 - panelW / 2 + 20;
    float spacing = 40;
    char line[128];

    sprintf_s(line, sizeof(line), " - Score: %d", score);
    CP_Font_DrawText(line, leftX, y); y += spacing;

    sprintf_s(line, sizeof(line), " - Highest Score: %d", highScore);
    CP_Font_DrawText(line, leftX, y); y += spacing;

    sprintf_s(line, sizeof(line), " - Streak: %d", streak);
    CP_Font_DrawText(line, leftX, y); y += spacing;

    sprintf_s(line, sizeof(line), " - Highest Streak: %d", highStreak);
    CP_Font_DrawText(line, leftX, y);

    // buttons
    float buttonY = g_winH / 2 + panelH / 2 - 70;

    float homeX, retryX, nextX = 0.0f;

    if (!passed)
    {
        homeX = g_winW * 0.5f - 120;
        retryX = g_winW * 0.5f + 120;
    }
    else
    {
        homeX = g_winW * 0.5f - 200;
        retryX = g_winW * 0.5f;
        nextX = g_winW * 0.5f + 200;
    }

    int hHome = DrawGameOverButton("Home", homeX, buttonY, &homeScale);
    int hRetry = DrawGameOverButton("Retry", retryX, buttonY, &retryScale);

    int hNext = passed ?
        DrawGameOverButton("Next", nextX, buttonY, &nextScale) : 0;

    if (CP_Input_MouseClicked())
    {
        if (hHome)
        {
            homeScale = 1.35f;
            Audio_PlayClick();
            pendingAction = 1;
            actionTimer = 0.30f;
        }
        else if (hRetry)
        {
            retryScale = 1.35f;
            Audio_PlayClick();
            pendingAction = 2;
            actionTimer = 0.30f;
        }
        else if (passed && hNext)
        {
            nextScale = 1.35f;
            Audio_PlayClick();
            pendingAction = 3;
            actionTimer = 0.30f;
        }
    }
}

void DrawTimer(float timerValue)
{
    float border = 10.0f;
    float titleH = 45.0f;
    float tabH = 40.0f;

    float tabY = titleH + tabH / 2.0f;
    float padding = border + 15.0f;

    char timeText[32];
    sprintf_s(timeText, sizeof(timeText), "Time: %.1f", timerValue);

    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_RIGHT, CP_TEXT_ALIGN_V_MIDDLE);
    CP_Settings_TextSize(23);
    CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));

    float timerX = g_winW - padding;
    CP_Font_DrawText(timeText, timerX, tabY);
}


void Bugs_Ui_Exit() {
    CP_Font_Free(font);

    for (int i = 0; i < 3; i++) {
        CP_Image_Free(tutDeath[i]);
    }
}