//---------------------------------------------------------
// file:	kitchen2.c
// author:	Jerald
// email:	p.rongjunjerald@digipen.edu 
//
// brief:	Dishwasher Level 4 Game - Kitchen Chaos
//
//---------------------------------------------------------

#include "cprocessing.h"
#include "kitchen2.h"
#include "kitchen_ui.h"
#include "playerselection.h"
#include "dishwasherlevels.h"
#include "math.h"
#include "utils.h"
#include "score.h"
#include "pausegame.h"
#include "audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define WINDOW_W 1600
#define WINDOW_H 900
#define HALF_W 37.5f
#define WATER_OBSTACLE_INDEX 4


typedef struct Player {
    float x, y;
    float width, height;
    float speed;
} Player;

typedef enum {
    OB_TABLE,
    OB_SINK,
    OB_COUNTER,
    OB
} ObType;

typedef struct {
    float x, y, w, h;
} Rect;

typedef struct {
    float x_left, x_right;
    float y_top, y_bottom;
    ObType type;
} Obstacle;

typedef struct {
    int   type;     // 1..4 = Spoon, Fork, Knife, Plate
    float x, y;     // center
    int   active;   // 1 = present
} Spawner;


// Player
Player dishwasher;

// Colors
CP_Color white, bgColor, bgColor_y;

// Font and Images
CP_Font myFont;
CP_Image Dishwasher_Image;
CP_Image Knife_Image;
CP_Image Spoon_Image;
CP_Image Fork_Image;
CP_Image Plate_Image;
CP_Image Pause_Image;
CP_Image Tap_Image;
CP_Image WaterImage;
CP_Image MopImage;
static CP_Image currentItemImage = NULL;

// Collision areas
Rect sinkRect;
Rect UtensilRect;
Rect PlatesRect;
Rect SlotRect;
Rect KnifeRect;

Rect pause;

// Obstacles x_left, x_rigth, y_top, y_botoom
static Obstacle obstacles[] = {
    {0, 1600, 0, 380, OB},        // Wall
    {200, 1290, 0, 420, OB},      // lamp to trash
    {670, 945, 575, 690, OB },     // Tables
    {260, 340, 690, 740, OB},     // Tray
    {0, 0, 0, 0, OB},                 // water
    {710, 905, 525, 575, OB},     // Chair
};

// Spawner
Spawner spawners;

// Game state
static int level = 4;
static int score = 0;
static float gameTimer = 30.0f;
static int gameOver = 0;
static bool showHowTo = true;
static int spam_key = 0;

//Water State
static bool water = false;
static bool waterActive = false;
static bool mopping = false;
static int water_count = 5;
static float waterX = 0;
static float waterY = 0;
static float waterTimer = 0;
static float mopTimer = 0;
static float repositionTimer = 0;
static float REPOSITION_INTERVAL = 10.f;
int animationFrame = 0;
static int ANIMATION_SPEED = 8;


// Item management
static bool holding = false;
static bool isClean = false;
static int random = 0;
static bool spawn = false;
static bool isFacingLeft = false;
static bool isPaused = false;

// Washing system
static int isWashing = 0;
static float washTimer = 0.0f;
static float washDuration = 1.6f;
static float health = 0;

// Helpers
static int rectCollideRect(Rect a, Rect b)
{
    float axMin = a.x - a.w * 0.5f;
    float axMax = a.x + a.w * 0.5f;
    float ayMin = a.y - a.h * 0.5f;
    float ayMax = a.y + a.h * 0.5f;

    float bxMin = b.x - b.w * 0.5f;
    float bxMax = b.x + b.w * 0.5f;
    float byMin = b.y - b.h * 0.5f;
    float byMax = b.y + b.h * 0.5f;

    return !(axMax < bxMin ||
        bxMax < axMin ||
        ayMax < byMin ||
        byMax < ayMin);
}

// Kitchen state functions
void Kitchen2_Init(void)
{
    CP_System_SetWindowSize(WINDOW_W, WINDOW_H);

    Kitchen_Ui_Init();
    Score_LevelStart(level);

    white = CP_Color_Create(255, 255, 255, 255);
    bgColor = CP_Color_Create(4, 7, 32, 255);
    bgColor_y = CP_Color_Create(242, 213, 174, 255);

    myFont = CP_Font_Load("Assets/Pixellari.ttf");

    Dishwasher_Image = CP_Image_Load("Assets/dishwasher.png");
    Knife_Image = CP_Image_Load("Assets/knife.png");
    Spoon_Image = CP_Image_Load("Assets/spoon.png");
    Fork_Image = CP_Image_Load("Assets/fork.png");
    Plate_Image = CP_Image_Load("Assets/plate_white.png");
    Tap_Image = CP_Image_Load("Assets/tap.png");
    WaterImage = CP_Image_Load("Assets/water.png");
    
    // dishwasher/player
    dishwasher.x = 250.0f;
    dishwasher.y = 450.0f;
    dishwasher.width = 240.0f;
    dishwasher.height = 240.0f;
    dishwasher.speed = 8.0f;  // a little faster

    sinkRect.x = 800.f;
    sinkRect.y = 360.f;
    sinkRect.w = 75.f;
    sinkRect.h = 50.f;

    PlatesRect.x = 1145.f;
    PlatesRect.y = 360.f;
    PlatesRect.w = 75.f;
    PlatesRect.h = 50.f;

    UtensilRect.x = 454.5f;
    UtensilRect.y = 360.f;
    UtensilRect.w = 75.f;
    UtensilRect.h = 50.f;

    KnifeRect.x = 915.f;
    KnifeRect.y = 360.f;
    KnifeRect.w = 75.f;
    KnifeRect.h = 50.f;

    SlotRect.x = 125.f;
    SlotRect.y = 125.f;
    SlotRect.w = 100.f;
    SlotRect.h = 100.f;

    spawners.type = 1;
    spawners.x = 310.0f;
    spawners.y = 725.0f;
    spawners.active = 1;

    pause.x = 1570.f;
    pause.y = 25.f;
    pause.w = 50.f;
    pause.h = 50.f;

    score = 0;
    gameTimer = 60.0f;
    gameOver = 0;
    holding = false;
    isClean = false;
    spawn = false;
    isFacingLeft = false;
    random = 0;
    currentItemImage = NULL;
    showHowTo = true;

    water_count = 5;

    // washing state
    isWashing = 0;
    washTimer = 0.0f;
    health = 0;
}

void Kitchen2_Update(void)
{
    CP_Graphics_ClearBackground(bgColor_y);

    Kitchen_Ui();

    CP_Settings_AntiAlias(FALSE);
    CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_NEAREST);

    if (isPaused) {
        CP_Graphics_ClearBackground(bgColor_y);

        Pause_Game_Init();

        int game_type = Pause_Game_Update();

        if (game_type == 0) {
            if (!gameOver) {
                Score_LevelStart(level);
                Score_LevelEnd();
                gameOver = 1;
            }
            isPaused = false;
            CP_Engine_SetNextGameStateForced(Kitchen2_Init, Kitchen2_Update, Kitchen2_Exit);
        }
        else if (game_type == 1) {
            if (!gameOver) {
                Score_LevelStart(level);
                Score_LevelEnd();
                gameOver = 1;
            }
            isPaused = false;
            CP_Engine_SetNextGameStateForced(Dishwasher_Levels_Init, Dishwasher_Levels_Update, Dishwasher_Levels_Exit);
        }
        else if (game_type == -1) {
            isPaused = false;
        }
    }

    if (!isPaused) {
        
        if (showHowTo) {
            showHowTo = Game_Menu_ui(4);
            return;
        }

        // Slot 1
        CP_Settings_StrokeWeight(0.f);
        CP_Settings_Fill(white);
        CP_Graphics_DrawRectAdvanced(SlotRect.x, SlotRect.y, SlotRect.w, SlotRect.h, 0, 5.f);

        float dt = CP_System_GetDt();

        // global timer
        gameTimer -= dt;
        if (gameTimer <= 0)
        {
            gameTimer = 0;
            gameOver = 1;   // Activate game over state
        }

        // Washing timer update
        if (isWashing)
        {
            washTimer -= dt;
            if (washTimer <= 0.0f)
            {
                washTimer = 0.0f;
                isClean = true;

                if (CP_Input_KeyDown(KEY_SPACE)) {
                    Audio_PickUp();
                    isWashing = 0;
                }
            }


        }

        // Movement (WASD) + obstacle checks
        // NOTE: Player CANNOT MOVE while washing
        if (!isWashing && !mopping)
        {
            if (CP_Input_KeyDown(KEY_W) && ((dishwasher.y - dishwasher.speed) > HALF_W)) {
                dishwasher.y -= dishwasher.speed;
                for (int i = 0; i < (int)(sizeof(obstacles) / sizeof(obstacles[0])); ++i) {
                    if (dishwasher.y - HALF_W < obstacles[i].y_bottom &&
                        dishwasher.y + HALF_W > obstacles[i].y_bottom &&
                        dishwasher.x - HALF_W < obstacles[i].x_right &&
                        dishwasher.x + HALF_W > obstacles[i].x_left) {
                        dishwasher.y = obstacles[i].y_bottom + HALF_W;
                    }
                }
            }
            if (CP_Input_KeyDown(KEY_A) && ((dishwasher.x - dishwasher.speed) > HALF_W)) {
                dishwasher.x -= dishwasher.speed;
                isFacingLeft = true;
                for (int i = 0; i < (int)(sizeof(obstacles) / sizeof(obstacles[0])); ++i) {
                    if (dishwasher.y - HALF_W < obstacles[i].y_bottom &&
                        dishwasher.y + HALF_W > obstacles[i].y_top &&
                        dishwasher.x - HALF_W < obstacles[i].x_right &&
                        dishwasher.x + HALF_W > obstacles[i].x_right) {
                        dishwasher.x = obstacles[i].x_right + HALF_W;
                    }
                }
            }
            if (CP_Input_KeyDown(KEY_S) && ((dishwasher.y + dishwasher.speed) < (900 - HALF_W))) {
                dishwasher.y += dishwasher.speed;
                for (int i = 0; i < (int)(sizeof(obstacles) / sizeof(obstacles[0])); ++i) {
                    if (dishwasher.y - HALF_W < obstacles[i].y_top &&
                        dishwasher.y + HALF_W > obstacles[i].y_top &&
                        dishwasher.x - HALF_W < obstacles[i].x_right &&
                        dishwasher.x + HALF_W > obstacles[i].x_left) {
                        dishwasher.y = obstacles[i].y_top - HALF_W;
                    }
                }
            }
            if (CP_Input_KeyDown(KEY_D) && ((dishwasher.x + dishwasher.speed) < (1600 - HALF_W))) {
                dishwasher.x += dishwasher.speed;
                isFacingLeft = false;
                for (int i = 0; i < (int)(sizeof(obstacles) / sizeof(obstacles[0])); ++i) {
                    if (dishwasher.y - HALF_W < obstacles[i].y_bottom &&
                        dishwasher.y + HALF_W > obstacles[i].y_top &&
                        dishwasher.x + HALF_W > obstacles[i].x_left &&
                        dishwasher.x - HALF_W < obstacles[i].x_left) {
                        dishwasher.x = obstacles[i].x_left - HALF_W;
                    }
                }
            }
        }

        // --- Cutlery interaction (SPACE) ---
        static int spacePrev = 0;
        int spaceNow = CP_Input_KeyDown(KEY_SPACE);

        Rect playerBox = { dishwasher.x - HALF_W, dishwasher.y - HALF_W, 75.0f, 75.0f };

        if (spaceNow && !spacePrev)
        {
            //int startedWash = 0;
            // Start washing at sink if holding dirty item
            if (!isWashing && holding && !isClean)
            {
                if (rectCollideRect(playerBox, sinkRect))
                {
                    Audio_Rinse();
                    health = 0;
                    isWashing = 1;
                    washTimer = washDuration;
                    Score_AddPoints(5);
                    spam_key = 0;
                }
                else {
                    spam_key = 1;
                }
            }
            // Pick up item from tray (only if not holding anything)
            else if (!isWashing && !holding)
            {
                Rect trayRect = { spawners.x - 50, spawners.y - 50, 110.0f, 70.0f };

                if (rectCollideRect(trayRect, playerBox) && spawners.active)
                {
                    Audio_PickUp();
                    holding = true;
                    isClean = false;
                    spawners.active = 0;
                    spawn = false;
                    spam_key = 0;
                }
                else {
                    spam_key = 1;
                }
            }
            // Return clean item to designated area
            else if (!isWashing && holding && isClean)
            {
                int delivered = 0;

                // Check utensil return area (types 1, 2, 3)
                if (random == 1 || random == 2)
                {
                    if (rectCollideRect(playerBox, UtensilRect))
                    {
                        Audio_PlateDrop();
                        holding = false;
                        isClean = false;
                        delivered = 1;
                        Score_AddPoints(5);
                        spam_key = 0;
                    }
                    else {
                        spam_key = 1;
                    }
                }
                // Check plate return area (type 4)
                else if (random == 3)
                {
                    if (rectCollideRect(playerBox, KnifeRect))
                    {
                        Audio_PlateDrop();
                        holding = false;
                        isClean = false;
                        delivered = 1;
                        Score_AddPoints(5);
                        spam_key = 0;
                    }
                    else {
                        spam_key = 1;
                    }
                }
                // Check plate return area (type 4)
                else if (random == 4)
                {
                    if (rectCollideRect(playerBox, PlatesRect))
                    {
                        Audio_PlateDrop();
                        holding = false;
                        isClean = false;
                        delivered = 1;
                        Score_AddPoints(5);
                        spam_key = 0;
                    }
                    else {
                        spam_key = 1;
                    }
                }

                // Reset spawner if item was delivered
                if (delivered)
                {
                    spawners.active = 1;
                    spawn = false;
                }
            }

            if (spam_key && !rectCollideRect(playerBox, sinkRect) && holding) {
                Score_DeductPoints(1);
                spam_key = 0;
            }

        }
        spacePrev = spaceNow;

        // --- Washing Progress Bar (above player) ---
        if (isWashing)
        {

            float barW = 80.0f;
            float barH = 8.0f;
            float barX = 800.f;
            float barY = 300.f;
            health = health < 80 ? health + 1 : health;

            CP_Image_Draw(Tap_Image, 800, 385, 250, 250, 255);

            CP_Settings_StrokeWeight(0.f);
            CP_Settings_Fill(white);
            CP_Graphics_DrawRectAdvanced(barX, barY, barW, barH, 0, 2.f);

            CP_Settings_Fill(CP_Color_Create(0, 255, 0, 255));
            CP_Graphics_DrawRectAdvanced(barX + health / 2 - 40, barY, health, barH - 2, 0, 2.f);

        }

        // --- Draw spawners ---
        if (spawners.active && !spawn)
        {

            random = CP_Random_RangeInt(1, 4);

            switch (random)
            {
            case 1: currentItemImage = Spoon_Image; break;
            case 2: currentItemImage = Fork_Image;  break;
            case 3: currentItemImage = Knife_Image; break;
            case 4: currentItemImage = Plate_Image; break;
            default:
                currentItemImage = Plate_Image; break;
            }
            spawn = true;
        }

        if (currentItemImage && spawners.active) {
            CP_Image_Draw(currentItemImage, spawners.x, spawners.y, 50, 50, 255);
        }

        if (currentItemImage && holding) {
            CP_Image_Draw(currentItemImage, SlotRect.x, SlotRect.y, SlotRect.w, SlotRect.h, 255);
        }


        if (gameOver)
        {
            obstacles[WATER_OBSTACLE_INDEX].x_left = 0;
            obstacles[WATER_OBSTACLE_INDEX].x_right = 0;
            obstacles[WATER_OBSTACLE_INDEX].y_top = 0;
            obstacles[WATER_OBSTACLE_INDEX].y_bottom = 0;
            gameTimer = 0;
            waterTimer = 0;
            waterActive = 0;
            water_count = 5;
            Game_Over_ui(level);
            Score_LevelEnd();

            return; // stop gameplay update
        }

        // Input handling
        if (CP_Input_KeyDown(KEY_R))
        {
            if (!gameOver) {
                Score_LevelStart(level);
                Score_LevelEnd();
                gameOver = 1;
            }
            CP_Engine_SetNextGameStateForced(Kitchen2_Init, Kitchen2_Update, Kitchen2_Exit);
            return;
        }
        if (CP_Input_KeyDown(KEY_Q))
        {
            if (!gameOver) {
                Score_LevelStart(level);
                Score_LevelEnd();
                gameOver = 1;
            }
            CP_Engine_SetNextGameStateForced(Dishwasher_Levels_Init, Dishwasher_Levels_Update, Dishwasher_Levels_Exit);
            return;
        }

        // --- HUD ---
        CP_Settings_Fill(white);
        CP_Font_Set(myFont);
        CP_Settings_TextSize(36.0f);
        CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_MIDDLE);

        char scoreText[64];
        sprintf_s(scoreText, sizeof(scoreText), "Score: %d", Score_Get());
        CP_Font_DrawText(scoreText, 50, 27.0f);

        char timeText[64];
        sprintf_s(timeText, sizeof(timeText), "Time: %4.1f", gameTimer);
        CP_Font_DrawText(timeText, 1350, 27.0f);

        // Draw character (flip horizontally when facing left)
        if (isFacingLeft)
        {
            CP_Image_Draw(Dishwasher_Image, dishwasher.x, dishwasher.y,
                -dishwasher.width, dishwasher.height, 255);
        }
        else
        {
            CP_Image_Draw(Dishwasher_Image, dishwasher.x, dishwasher.y,
                dishwasher.width, dishwasher.height, 255);
        }

        float mouseX = CP_Input_GetMouseX();
        float mouseY = CP_Input_GetMouseY();

        /* ---------------- PAUSE GAMEPLAY ---------------- */
        if (IsAreaHovered(pause.x, pause.y, pause.w, pause.h, mouseX, mouseY)) {
            Pause_Image = CP_Image_Load("Assets/brown_pause.png");
        }
        else {
            Pause_Image = CP_Image_Load("Assets/pause.png");
        }

        CP_Image_Draw(Pause_Image, pause.x, pause.y, 30.f, 30.f, 255);

        if (IsAreaClicked(pause.x, pause.y, pause.w, pause.h, mouseX, mouseY)) {
            if (CP_Input_MouseClicked())
            {
                isPaused = true;
            }
        }


        // Hard Add water
        if (water_count > 0) {
            if (gameTimer < 60.0) {

                repositionTimer += CP_System_GetDt();

                if (repositionTimer >= REPOSITION_INTERVAL) {
                    water = CP_Random_GetBool();

                    if (water) {
                        waterActive = 1;

                        int ranX = CP_Random_RangeInt(400, 1200);
                        int ranY;
                        if (ranX >= 620 && ranX <= 975) {
                            int choice = CP_Random_RangeInt(0, 1);
                            if (choice == 0) {
                                ranY = CP_Random_RangeInt(490, 500);
                            }
                            else {
                                ranY = CP_Random_RangeInt(770, 820);
                            }
                        }
                        else {
                            ranY = CP_Random_RangeInt(520, 820);
                        }
                        waterX = (float) ranX;
                        waterY = (float) ranY;

                        obstacles[WATER_OBSTACLE_INDEX].x_left = waterX - 25;
                        obstacles[WATER_OBSTACLE_INDEX].x_right = waterX + 25;
                        obstacles[WATER_OBSTACLE_INDEX].y_top = waterY - 25;
                        obstacles[WATER_OBSTACLE_INDEX].y_bottom = waterY + 25;

                        repositionTimer = 0.0f;
                    }
                }

                // Draw water at stored position
                if (waterActive) {
                    waterTimer = waterTimer <= 50 ? waterTimer + 0.2f : waterTimer;

                    CP_Settings_StrokeWeight(0.f);
                    CP_Settings_Fill(white);
                    CP_Graphics_DrawRectAdvanced(waterX, waterY - 35, 50.f, 8.f, 0, 2.f);

                    CP_Color bar_color = waterTimer > 40 ? CP_Color_Create(255, 0, 0, 255) : (waterTimer > 25 ? CP_Color_Create(255, 255, 0, 255) : CP_Color_Create(0, 255, 0, 255));
                    CP_Settings_Fill(bar_color);
                    CP_Graphics_DrawRectAdvanced(waterX + waterTimer / 2 - 25, waterY - 35, waterTimer, 8.f, 0, 2.f);

                    CP_Image_Draw(WaterImage, waterX, waterY, 50, 50, 255);
                }
            }
        }

        if (waterActive) {
            if (CP_Input_KeyDown(KEY_ENTER)) {
                Rect playerBox_water = { dishwasher.x, dishwasher.y, 75.0f, 75.0f };
                Rect waterRect = { waterX, waterY, 50.f, 50.f };
                if (rectCollideRect(playerBox_water, waterRect))
                {
                    Audio_Mopping();
                    waterTimer = 0;
                    waterActive = 0;
                    water_count -= 1;
                    mopping = true;
                    Score_AddPoints(8);
                }
            }
        }

        if (mopping) {
            float barW = 80.0f;
            float barH = 8.0f;
            mopTimer = mopTimer <= 80 ? mopTimer + 1 : mopTimer;

            animationFrame++;

            int sequenceIndex = (animationFrame / ANIMATION_SPEED) % 4;

            switch (sequenceIndex)
            {
            case 0: MopImage = CP_Image_Load("Assets/mop1.png"); break;
            case 1: MopImage = CP_Image_Load("Assets/mop2.png");  break;
            case 2: MopImage = CP_Image_Load("Assets/mop3.png"); break;
            case 3: MopImage = CP_Image_Load("Assets/mop2.png");  break;
            default:
                MopImage = CP_Image_Load("Assets/mop1.png"); break;
            }

            CP_Image_Draw(MopImage, waterX, waterY, 100, 100, 255);

            CP_Settings_StrokeWeight(0.f);
            CP_Settings_Fill(white);
            CP_Graphics_DrawRectAdvanced(waterX, waterY - 40, barW, barH, 0, 2.f);

            CP_Settings_Fill(CP_Color_Create(0, 255, 0, 255));
            CP_Graphics_DrawRectAdvanced(waterX + mopTimer / 2 - 40, waterY - 40, mopTimer, barH, 0, 2.f);

            if (mopTimer >= 80) {
                mopping = false;
                mopTimer = 0;
                animationFrame = 0;

                obstacles[WATER_OBSTACLE_INDEX].x_left = 0;
                obstacles[WATER_OBSTACLE_INDEX].x_right = 0;
                obstacles[WATER_OBSTACLE_INDEX].y_top = 0;
                obstacles[WATER_OBSTACLE_INDEX].y_bottom = 0;

            }
        }

        if (waterTimer >= 50.f) {
            obstacles[WATER_OBSTACLE_INDEX].x_left = 0;
            obstacles[WATER_OBSTACLE_INDEX].x_right = 0;
            obstacles[WATER_OBSTACLE_INDEX].y_top = 0;
            obstacles[WATER_OBSTACLE_INDEX].y_bottom = 0;
            waterTimer = 0;
            waterActive = 0;
            water_count -= 1;
            Score_DeductPoints(5);
        }
    }
}


void Kitchen2_Exit(void)
{
    if (!gameOver) {
        Score_LevelEnd();
        gameOver = 1;
    }
    CP_Font_Free(myFont);
    CP_Graphics_ClearBackground(bgColor);
    CP_Image_Free(Dishwasher_Image);
    CP_Image_Free(Knife_Image);
    CP_Image_Free(Spoon_Image);
    CP_Image_Free(Fork_Image);
    CP_Image_Free(Plate_Image);
    CP_Image_Free(Pause_Image);
    CP_Image_Free(Tap_Image);
    CP_Image_Free(WaterImage);

    Kitchen_Ui_Exit();
}