//---------------------------------------------------------
// file:	spotless_common.c
// author:	Dylan
// email:	dylanjoonteck.lim@digipen.edu
//
// brief:	Dishwasher level 1-2 Game - Scrub master
//
//---------------------------------------------------------

#include "dishwasher1_2internal.h"
#include "dishwasher1_2public.h"      
#include "score.h"
#include "utils.h"
#include "kitchen.h"
#include "dishwasherlevels.h"
#include "pausegame.h"
#include "audio.h"
#include <math.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include "spotless_bubbles.h"

// ------------------- Private module state -------------------
static CP_Font myFont;
extern int coin;              // defined in their Dishwasher module
extern CP_Font myFont;        // optional
extern CP_Color gold;         // optional
// --- art assets ---
static CP_Image imgPlate;
static CP_Image imgStainSoap;    // stain that requires Soap
static CP_Image imgStainSponge;  // stain that requires Sponge
static CP_Image imgToolSoap;     // toolbar icon
static CP_Image imgToolSponge;   // toolbar icon
static CP_Image bg_img;   // toolbar icon
static int assetsOK = 0;         // 1 if loaded successfully
static int levelEnded = 0;

//dirt spot states
typedef enum SpotState { SPOT_DIRTY = 0, SPOT_FADING = 1, SPOT_GONE = 2 } SpotState;
typedef enum DirtType {
    DIRT_SOAP = 0,   // needs Soap
    DIRT_SPONGE = 1  // needs Sponge
} DirtType;

typedef struct DirtSpot {
    float x, y;
    float radius;
    int   cleaned;
    int   type;      //DIRT_SOAP or DIRT_SPONGE
    SpotState st;       // NEW
    float fade;
} DirtSpot;

enum { MAX_DIRT = 8 };

static DirtSpot dirtSpots[MAX_DIRT];
static int   dirtCount;
static float plateX, plateY, plateR;
static float g_stainScale = 10.0f;

static float timer;
static float timeLimit;
static int   isHard;
static int   showSplash;
static int   gameOver;

//RINSE animation (clean plate overlay)
static int   rinseAnimActive = 0;  // 1 while the clean plate is flying up
static float rinsePlateX, rinsePlateY;
static float rinsePlateR;
static float rinseVelY;            // px/s (negative = up)
static float rinseAccelY;          // px/s^2 (optional)

// how-to screen shown at the start
static int   showHowTo = 1;

// current cleaning tool selected from UI
typedef enum ToolType {
    TOOL_SOAP = 0,
    TOOL_SPONGE = 1
} ToolType;

static int currentTool = TOOL_SOAP;  // default

CP_Color gold;

CP_Color white;
CP_Color bgColor;
CP_Color light_purple;

typedef struct Button {
    float x, y;
    float width, height;
    float degree;

}Button;

CP_Image pause_img;
Button pause;



static bool isPaused = false;

// ------------------- Private helpers -------------------
#define DIRT_PADDING 4.0f
#define MAX_TRIES_PER_SPOT 300
#define HIT_PAD 3.0f

static int OverlapsExisting(int count, float x, float y, float r)
{
    for (int j = 0; j < count; ++j) {
        float dx = x - dirtSpots[j].x;
        float dy = y - dirtSpots[j].y;
        float need = r + dirtSpots[j].radius + DIRT_PADDING;
        if (dx * dx + dy * dy < need * need) return 1;
    }
    return 0;
}

static void Spotless_GeneratePlate(int hard)
{
    dirtCount = hard ? 8 : 4;

    for (int i = 0; i < dirtCount; ++i) {
        float r = hard ? 20.0f : 25.0f;
        float minDist = r;
        float maxDist = plateR - r;
        int tries = 0;
        float x, y;
        do {
            float angle = CP_Random_RangeFloat(0.0f, 6.2831853f);
            float dist = CP_Random_RangeFloat(minDist, maxDist);
            x = plateX + cosf(angle) * dist;
            y = plateY + sinf(angle) * dist;
            ++tries;
        } while (OverlapsExisting(i, x, y, r) && tries < MAX_TRIES_PER_SPOT);

        if (tries >= MAX_TRIES_PER_SPOT) { dirtCount = i; break; }
        dirtSpots[i] = (DirtSpot){
            x, y, r,
            0,                                // cleaned
            (int)CP_Random_RangeInt(0, 1),    // type
            SPOT_DIRTY,                       // st
            1.0f                              // fade
        };
        
    }
}

static int Spotless_AllCleaned(void)
{
    for (int i = 0; i < dirtCount; ++i)
        if (dirtSpots[i].st != SPOT_GONE) return 0;
    return 1;
}

static void Spotless_DrawHUD(void)
{
    // --- COIN DISPLAY (visual only) ---
    if (myFont) CP_Font_Set(myFont);

    CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));

    CP_Settings_TextSize(50.0f);
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_TOP);

    char coinText[50];
    snprintf(coinText, sizeof(coinText), "Score: %d", Score_Get());
    CP_Font_DrawText(coinText, 50.f, 40.f);

    // --- TIME (unchanged) ---
    float timeLeft = fmaxf(0.0f, timeLimit - timer);
    CP_Settings_TextSize(50.0f);
    CP_Font_DrawText((snprintf(coinText, sizeof(coinText), "Time: %.1f", timeLeft), coinText),
        700.f, 40.f);
}

// NEW: tool selection buttons
static void Spotless_DrawToolBar(void)
{
    float mouseX = CP_Input_GetMouseX();
    float mouseY = CP_Input_GetMouseY();

    // positions/sizes from your existing toolbar:
    float x0 = 50.0f, y0 = 130.0f;
    float w = 160.0f, h = 160.0f;
    float x1 = x0 + w + 16.0f;

    // button backgrounds (optional): keep your rects to show selection
    CP_Settings_RectMode(CP_POSITION_CORNER);
    //Hover Color
    if (IsAreaHovered(x0 + w * 0.5f, y0 + h * 0.5f, w, h, mouseX, mouseY)) {
        CP_Settings_Fill(currentTool == TOOL_SOAP ? CP_Color_Create(255, 210, 90, 255) : CP_Color_Create(255, 210, 140, 255));
    }
    else {
        CP_Settings_Fill(currentTool == TOOL_SOAP ? CP_Color_Create(255, 210, 90, 255) : CP_Color_Create(180, 140, 40, 255));
    }
    CP_Graphics_DrawRect(x0, y0, w, h);
    //Hover Color
    if (IsAreaHovered(x1 + w * 0.5f, y0 + h * 0.5f, w, h, mouseX, mouseY)) {
        CP_Settings_Fill(currentTool == TOOL_SPONGE ? CP_Color_Create(220, 40, 40, 255) : CP_Color_Create(220, 120, 120, 255));
    }
    else {
        CP_Settings_Fill(currentTool == TOOL_SPONGE ? CP_Color_Create(220, 40, 40, 255) : CP_Color_Create(180, 80, 80, 255));
    }
    CP_Graphics_DrawRect(x1, y0, w, h);

    // draw icons centered in those rects
    if (CP_Image_GetWidth(imgToolSoap))
        CP_Image_Draw(imgToolSoap, x0 + w * 0.5f, y0 + h * 0.5f, w * 1.5f, h * 1.5f, 255);
    if (CP_Image_GetWidth(imgToolSponge))
        CP_Image_Draw(imgToolSponge, x1 + w * 0.5f, y0 + h * 0.5f, w * 1.5f , h * 1.6f, 255);


    if (CP_Input_MouseClicked()) {                                       // CP mouse helpers :contentReference[oaicite:2]{index=2}
        float mx = CP_Input_GetMouseX(), my = CP_Input_GetMouseY();
        if (IsAreaClicked(x0 + w * 0.5f, y0 + h * 0.5f, w, h, mx, my))       // utils helper :contentReference[oaicite:3]{index=3}
            currentTool = TOOL_SOAP;
        else if (IsAreaClicked(x1 + w * 0.5f, y0 + h * 0.5f, w, h, mx, my))  // utils helper :contentReference[oaicite:4]{index=4}
            currentTool = TOOL_SPONGE;
    }
}

static void Spotless_HandleCleaning(void)
{
    float mx = CP_Input_GetMouseX();
    float my = CP_Input_GetMouseY();
    float dt = CP_System_GetDt();

    const float fadeSpeed = 3.0f;    // seconds^-1 (about 0.33s fade)
    const float shrinkAmt = 0.25f;   // shrink up to 25 percent while fading

    for (int i = 0; i < dirtCount; ++i) {
        // Skip fully gone
        if (dirtSpots[i].st == SPOT_GONE) continue;

        // Base (unscaled) diameter and radius
        float baseD = dirtSpots[i].radius * 2.0f;
        float baseR = dirtSpots[i].radius;

        // Visual scale (existing global) plus optional fading shrink
        float extraShrink = 0.0f;
        if (dirtSpots[i].st == SPOT_FADING)
            extraShrink = shrinkAmt * (1.0f - dirtSpots[i].fade); // grows from 0 to shrinkAmt
        float drawD = baseD * g_stainScale * (1.0f - extraShrink);

        // Compute current alpha
        int a = 255;
        if (dirtSpots[i].st == SPOT_FADING) {
            dirtSpots[i].fade -= fadeSpeed * dt;
            if (dirtSpots[i].fade <= 0.0f) {
                dirtSpots[i].fade = 0.0f;
                dirtSpots[i].st = SPOT_GONE;
                dirtSpots[i].cleaned = 1; // keep old flag in sync
                continue; // nothing to draw this frame
            }
            a = (int)(255.0f * dirtSpots[i].fade);
        }

        // Draw stain (image or fallback)
        if (assetsOK) {
            CP_Image img = (dirtSpots[i].type == DIRT_SOAP) ? imgStainSoap : imgStainSponge;
            CP_Image_Draw(img, dirtSpots[i].x, dirtSpots[i].y, drawD, drawD, a);
        }
        else {
            CP_Color base = (dirtSpots[i].type == DIRT_SOAP)
                ? CP_Color_Create(120, 200, 255, a)
                : CP_Color_Create(130, 100, 40, a);
            CP_Settings_Fill(base);
            CP_Graphics_DrawCircle(dirtSpots[i].x, dirtSpots[i].y, drawD);
        }

        // No hit-tests while fading
        if (dirtSpots[i].st != SPOT_DIRTY) continue;

        // Hit stays unscaled (gameplay size)
        float dx = mx - dirtSpots[i].x, dy = my - dirtSpots[i].y;
        float hitR = baseR + HIT_PAD;
        if (dx * dx + dy * dy <= hitR * hitR) {
            // Hover ring at original size
            CP_Settings_Fill(CP_Color_Create(255, 255, 255, 50));
            CP_Graphics_DrawCircle(dirtSpots[i].x, dirtSpots[i].y, baseD + 6.0f);

            if ((CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT) || CP_Input_MouseDown(MOUSE_BUTTON_LEFT)) &&
                ((currentTool == TOOL_SOAP && dirtSpots[i].type == DIRT_SOAP) ||
                    (currentTool == TOOL_SPONGE && dirtSpots[i].type == DIRT_SPONGE))) {
                // Start fading instead of disappearing instantly
                Audio_Scrub();
                dirtSpots[i].st = SPOT_FADING;
                dirtSpots[i].fade = 1.0f;
                Score_AddPoints(1);
            }
        }
    }
}

static void Spotless_SplashScreen(int level)
{
    float w = (float) CP_System_GetWindowWidth();
    float h = (float) CP_System_GetWindowHeight();

    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
    CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
    CP_Settings_TextSize(60.0f);
    CP_Font_DrawText("Time's Up!", w / 2, h * 0.3f);

    char txt[64];
    snprintf(txt, sizeof(txt), "Game Score: %d", Score_Final());
    CP_Settings_TextSize(40.0f);
    CP_Font_DrawText(txt, w / 2, h * 0.38f);
	 
    snprintf(txt, sizeof(txt), "High Score: %d", Score_GetHigh(level));
    CP_Settings_TextSize(40.0f);
    CP_Font_DrawText(txt, w / 2, h * 0.38f + 40.f);

    snprintf(txt, sizeof(txt), "Total Score: %d", Score_GetHighTotal());
    CP_Settings_TextSize(40.0f);
    CP_Font_DrawText(txt, w / 2, h * 0.38f + 80.f);

    float btnW = 250, btnH = 80, btnX = w / 2, btnY = h * 0.65f;
    CP_Settings_RectMode(CP_POSITION_CENTER);
    CP_Settings_Fill(CP_Color_Create(100, 180, 100, 255));
    CP_Graphics_DrawRect(btnX, btnY, btnW, btnH);
    CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
    CP_Settings_TextSize(32.0f);
    CP_Font_DrawText("Next Level", btnX, btnY);

    if (CP_Input_MouseClicked()) {
        float mx = CP_Input_GetMouseX(), my = CP_Input_GetMouseY();
        if (IsAreaClicked(btnX, btnY, btnW, btnH, mx, my)) { // utils helper :contentReference[oaicite:6]{index=6}
            if (!isHard)
                CP_Engine_SetNextGameStateForced(Dishwasher_2_Init, Dishwasher_2_Update, Dishwasher_2_Exit);
            else
                CP_Engine_SetNextGameStateForced(Kitchen_Init, Kitchen_Update, Kitchen_Exit);
        }
    }
}

// first-start instruction overlay with X to close
static void Spotless_DrawHowTo(void)
{
    float w = (float) CP_System_GetWindowWidth();
    float h = (float) CP_System_GetWindowHeight();

    // dim background
    CP_Settings_Fill(CP_Color_Create(0, 0, 0, 160));
    CP_Settings_RectMode(CP_POSITION_CORNER);
    CP_Graphics_DrawRect(0, 0, w, h);

    // panel
    float pw = 680, ph = 380;
    float px = (w - pw) * 0.5f, py = (h - ph) * 0.5f;

    CP_Settings_Fill(CP_Color_Create(240, 240, 240, 255));
    CP_Graphics_DrawRect(px, py, pw, ph);

    // X button
    float xw = 40, xh = 40;
    float xc = px + pw - xw * 0.5f;
    float yc = py + xh * 0.5f;

    CP_Settings_RectMode(CP_POSITION_CENTER);
    CP_Settings_Fill(CP_Color_Create(200, 60, 60, 255));
    CP_Graphics_DrawRect(xc, yc, xw, xh);
    CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
    CP_Settings_TextSize(22.0f);
    CP_Font_DrawText("X", xc, yc);

    // text
    CP_Settings_TextSize(32.0f);
    CP_Settings_Fill(CP_Color_Create(30, 30, 30, 255));
    CP_Font_DrawText("How to Clean", px + pw * 0.5f, py + 80);

    CP_Settings_TextSize(24.0f);
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_TOP);
    float tx = px + 20;
    float ty = py + 120;

    // First bullet point
    float currentX = tx;
    CP_Settings_Fill(CP_Color_Create(30, 30, 30, 255));
    CP_Font_DrawText("- Click a tool: Soap (", currentX, ty);
    currentX += 210; // Approximate width of previous text

    CP_Settings_Fill(CP_Color_Create(255, 0, 0, 255)); // RED
    CP_Font_DrawText("  RED", currentX, ty);
    currentX += 50; // Width of "RED"

    CP_Settings_Fill(CP_Color_Create(30, 30, 30, 255));
    CP_Font_DrawText("  stains) or Sponge(", currentX, ty);
    currentX += 175; // Width of middle text

    CP_Settings_Fill(CP_Color_Create(255, 255, 0, 255)); // YELLOW
    CP_Font_DrawText("     YELLOW", currentX, ty);
    currentX += 70; // Width of "YELLOW"

    CP_Settings_Fill(CP_Color_Create(30, 30, 30, 255));
    CP_Font_DrawText("        stains)", currentX, ty);

    // Other bullet points (same as before)
    CP_Font_DrawText("- Clean spots with the matching tool", tx, ty + 34);

    CP_Settings_Fill(CP_Color_Create(30, 30, 30, 255));
    CP_Font_DrawText("- When all spots are clean, press ", tx, ty + 68);
    CP_Settings_Fill(CP_Color_Create(100, 200, 255, 255)); // Light blue for RINSE
    CP_Font_DrawText("    RINSE", tx + 340, ty + 68);
    CP_Settings_Fill(CP_Color_Create(30, 30, 30, 255));

    CP_Font_DrawText("- Press R to restart. Press Q to exit.", tx, ty + 102);
    if (CP_Input_MouseClicked()) { // CP input click :contentReference[oaicite:7]{index=7}
        float mx = CP_Input_GetMouseX(), my = CP_Input_GetMouseY();
        if (IsAreaClicked(xc, yc, xw, xh, mx, my))    // utils helper :contentReference[oaicite:8]{index=8}
            showHowTo = 0;
    }
}

// ------------------- Common entry points (used by wrappers) -------------------

void Spotless_CommonInit(int hard)
{
    myFont = CP_Font_Load("Assets/Pixellari.ttf");

    // --- load art (PNG with alpha recommended) ---
    imgPlate = CP_Image_Load("Assets/plate.png");
    imgStainSoap = CP_Image_Load("Assets/stain_1.png");
    imgStainSponge = CP_Image_Load("Assets/stain_2.png");
    imgToolSoap = CP_Image_Load("Assets/soap.png");
    imgToolSponge = CP_Image_Load("Assets/sponge.png");
    bg_img = CP_Image_Load("Assets/dwbg.png");

    // consider it OK if the essentials exist
    assetsOK = (CP_Image_GetWidth(imgPlate) > 0) &&
        (CP_Image_GetWidth(imgStainSoap) > 0) &&
        (CP_Image_GetWidth(imgStainSponge) > 0);

    CP_System_SetWindowSize(1600, 900);
    // turn smoothing OFF
    CP_Settings_AntiAlias(FALSE);                   // primitives/text AA off  :contentReference[oaicite:2]{index=2}
    CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_NEAREST); // images use nearest 
    levelEnded = 0;
    int level = hard ? 2 : 1;
    Score_LevelStart(level);
    timer = 0.0f;
    timeLimit = hard ? 20.0f : 30.0f;
    isHard = hard;
    showSplash = gameOver = 0;
    showHowTo = 1;              // show instructions on level start
    currentTool = TOOL_SOAP;    // default tool

    plateX = 1600 / 2.0f;
    plateY = 900 / 2.0f;
    plateR = 260.0f;

    white = CP_Color_Create(255, 255, 255, 255);
    light_purple = CP_Color_Create(201, 160, 220, 255);

    pause.x = 1525.f;
    pause.y = 65.f;
    pause.width = 50.f;
    pause.height = 50.f;
    pause.degree = 0;

    Spotless_GeneratePlate(hard);
    
    Spotless_Bubbles_Init();
    Spotless_Bubbles_Reset();
}

void Spotless_CommonUpdate(int hard)
{
    CP_Graphics_ClearBackground(white);

    CP_Image_Draw(bg_img, 800.f, 450.f, 1600.f, 900.f, 255);
    
    float mouseX = CP_Input_GetMouseX();
    float mouseY = CP_Input_GetMouseY();
    if (isPaused) {
        Pause_Game_Init();

        int game_type = Pause_Game_Update();

        if (game_type == 0) {
            CP_Settings_Stroke(white);
            if (!levelEnded) {
                int level = hard ? 2 : 1;
                Score_LevelStart(level);
                Score_LevelEnd();
                levelEnded = 1;
            }
            isPaused = false;
            if (hard == 1)
                CP_Engine_SetNextGameStateForced(Dishwasher_2_Init, Dishwasher_2_Update, Dishwasher_2_Exit);
            else
                CP_Engine_SetNextGameStateForced(Dishwasher_1_Init, Dishwasher_1_Update, Dishwasher_1_Exit);
            return;
        } else if (game_type == 1) {
            if (!levelEnded) {
                int level = hard ? 2 : 1;
                Score_LevelStart(level);
                Score_LevelEnd();
                levelEnded = 1;
            }
            isPaused = false;
            printf("Player exited early.\n");
            CP_Engine_SetNextGameState(Dishwasher_Levels_Init, Dishwasher_Levels_Update, Dishwasher_Levels_Exit);
            return;
        } else if (game_type == -1){
            CP_Settings_Stroke(white);
            isPaused = false;
        }
    }
    if (!isPaused) {
        int level = hard ? 2 : 1;
        if (showSplash) 
        { 
        Spotless_SplashScreen(level);
        return; 
        }

        // Timer only runs when instructions are closed AND not animating
        if (!showHowTo && !rinseAnimActive)
            timer += CP_System_GetDt();

        // --- Draw the plate UNDER everything (this is the current/next dirty plate) ---
        if (assetsOK) {
            CP_Image_Draw(imgPlate, plateX, plateY, plateR * 2.2f, plateR * 2.2f, 255);
        }
        else {
            CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
            CP_Graphics_DrawCircle(plateX, plateY, plateR * 2.0f); // fallback
        }

        // --- HUD is fine to show anytime ---
        Spotless_DrawHUD();

        // --- If instructions are up, draw them and stop here (no gameplay/timer) ---
        if (showHowTo) {
            Spotless_DrawHowTo();
            return;
        }

        // --- If NOT animating: normal gameplay (cleaning, toolbar, rinse button) ---
        if (!rinseAnimActive) {
            // dirt + tools
            Spotless_HandleCleaning();
            Spotless_DrawToolBar();

            // RINSE button
            float btnW = 200, btnH = 60, btnX = 1600 / 2.0f, btnY = 900 - 100.0f;
            CP_Settings_RectMode(CP_POSITION_CENTER);
            CP_Settings_Fill(CP_Color_Create(10, 180, 220, 255));
            CP_Graphics_DrawRect(btnX, btnY, btnW, btnH);

            CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
            CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
            CP_Settings_TextSize(32.0f);
            CP_Font_DrawText("RINSE", btnX, btnY);

            if (CP_Input_MouseClicked()) {
                float mx = CP_Input_GetMouseX(), my = CP_Input_GetMouseY();
                if (IsAreaClicked(btnX, btnY, btnW, btnH, mx, my) && Spotless_AllCleaned()) {
                    // score for finishing the plate
                    Score_AddPoints(5);
                    // Play Rinse audio
                    Audio_Rinse();
                    // 1) Spawn the NEXT dirty plate immediately (underlay)
                    Spotless_GeneratePlate(isHard);

                    // 2) Start the CLEAN plate overlay animation using the OLD plate pose
                    rinseAnimActive = 1;
                    rinsePlateX = plateX;
                    rinsePlateY = plateY;
                    rinsePlateR = plateR;
                    rinseVelY = -900.0f;  // tune speed
                    rinseAccelY = -300.0f;  // optional ease-up
                }
            }
        }

        // --- If animating: draw the CLEAN plate overlay flying up and stop input ---
        if (rinseAnimActive) {
            float dt = CP_System_GetDt();
            rinseVelY += rinseAccelY * dt;
            rinsePlateY += rinseVelY * dt;
            

            // clean plate overlay on top of everything
            if (assetsOK) {
                CP_Image_Draw(imgPlate, rinsePlateX, rinsePlateY, rinsePlateR * 2.2f, rinsePlateR * 2.2f, 255);
            }
            else {
                CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
                CP_Graphics_DrawCircle(rinsePlateX, rinsePlateY, rinsePlateR * 2.0f);
            }

            // finish when the clean plate is fully off-screen
            if (rinsePlateY + rinsePlateR < 0.0f) {
                rinseAnimActive = 0;
                // (Optional) timer = 0.0f; // if you want a fresh timer per plate
            }

            // No gameplay, keys, or time-up while animating
            return;
        }
        // cursor bubble effect (only when not paused)
        Spotless_Bubbles_Update();
        // --- Normal keys only when not animating and not showing instructions ---
        // exit level
        if (CP_Input_KeyTriggered(KEY_Q)) {
            if (!levelEnded) {
                Score_LevelStart(level);
                Score_LevelEnd();
                levelEnded = 1;
            }
            printf("Player exited early.\n");
            CP_Engine_SetNextGameState(Dishwasher_Levels_Init, Dishwasher_Levels_Update, Dishwasher_Levels_Exit);
            return;
        }

        // restart (triggered once)
        if (CP_Input_KeyTriggered(KEY_R)) {
            if (!levelEnded) {
                Score_LevelStart(level);
                Score_LevelEnd();
                levelEnded = 1;
            }
            if (hard == 1)
                CP_Engine_SetNextGameStateForced(Dishwasher_2_Init, Dishwasher_2_Update, Dishwasher_2_Exit);
            else
                CP_Engine_SetNextGameStateForced(Dishwasher_1_Init, Dishwasher_1_Update, Dishwasher_1_Exit);
            return;
        }

        // time up -> score splash
        if (timer >= timeLimit) {
            if (!levelEnded) {
                Score_LevelEnd();             // updates high score for this level
                Score_SetFinal(Score_Get());  // mirror for legacy UIs if you still want it
                levelEnded = 1;
            }
            showSplash = 1;
            return;
        }

        //pause btn
        if (IsAreaHovered(pause.x, pause.y, pause.width, pause.height, mouseX, mouseY)) {
            pause_img = CP_Image_Load("Assets/pause.png");
            CP_Settings_Fill(CP_Color_Create(129,207,234,255));
        }
        else {
            pause_img = CP_Image_Load("Assets/blue_pause.png");
            CP_Settings_Fill(white);

        }

        CP_Graphics_DrawRect(pause.x, pause.y, pause.width, pause.height);
        CP_Image_Draw(pause_img, pause.x, pause.y, 50.f, 50.f, 255);

        // Go back to previous menu
        if (IsAreaClicked(pause.x, pause.y, pause.width, pause.height, mouseX, mouseY)) {
            if (CP_Input_MouseClicked())
            {
                isPaused = true;
            }
        }

    }
}
void Spotless_CommonExit(void)
{
    if (!levelEnded) {
        Score_LevelEnd();
        levelEnded = 1;
    }
    CP_Graphics_ClearBackground(bgColor);
    if (CP_Image_GetWidth(imgPlate))       CP_Image_Free(imgPlate);
    if (CP_Image_GetWidth(imgStainSoap))   CP_Image_Free(imgStainSoap);
    if (CP_Image_GetWidth(imgStainSponge)) CP_Image_Free(imgStainSponge);
    if (CP_Image_GetWidth(imgToolSoap))    CP_Image_Free(imgToolSoap);
    if (CP_Image_GetWidth(imgToolSponge))  CP_Image_Free(imgToolSponge);
    if (CP_Image_GetWidth(pause_img))  CP_Image_Free(pause_img);
    if (CP_Image_GetWidth(bg_img))  CP_Image_Free(bg_img);
    CP_Font_Free(myFont);
    Spotless_Bubbles_Exit(); 
}