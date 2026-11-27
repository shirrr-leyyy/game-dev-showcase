//---------------------------------------------------------
// file:	coffeedriven1.c
// author:	Felicia
// email:	f.goh@digipen.edu
//
// brief:	Software Engineer Level 3 Game - Coffee driven
//---------------------------------------------------------
#include "cprocessing.h"
#include "engineerlevels.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pausegame.h"
#include "score.h"
#include "coffeedriven1.h"
#include "coffeedriven2.h"
#include "utils.h"

// --- DEFINITIONS ---
#define DEBUG_HITBOXES 0

#define MAX_OBSTACLES 30

// --- PLAYER PHYSICS ---
#define PLAYER_HITBOX_W 60.0f
#define PLAYER_HITBOX_H 60.0f
#define PLAYER_OFFSET_Y 20.0f

// --- PLAYER ENERGY ---
#define MAX_ENERGY 100.0f
#define ENERGY_DECAY_RATE 5.0f
#define PLAYER_BAR_W 15.0f
#define PLAYER_BAR_H 80.0f

// --- SLEEP MECHANICS ---
#define MAX_SLEEPS_ALLOWED 3
#define SLEEP_DURATION 3.0f
#define REGEN_AMOUNT (MAX_ENERGY / 3.0f)

// --- GAME TIMER ---
#define GAME_TIME_LIMIT 180.0f // 3 Minutes

// --- MUG DEFINITIONS ---
#define MAX_MUG_COFFEE 100.0f
#define SIP_AMOUNT 20.0f
#define MUG_BAR_W 10.0f
#define MUG_BAR_H 50.0f
#define INTERACTION_DIST 100.0f
#define MUG_OFFSET_X 40.0f
#define MUG_OFFSET_Y 40.0f

// --- LAPTOP MINIGAME DEFINITIONS ---
#define MAX_POSSIBLE_CIRCLES 10
#define CIRCLE_RADIUS 25.0f
#define MAX_PLACEMENT_ATTEMPTS 50

typedef struct {
    float x, y;
    int id; // 0-25 = A-Z, 26-35 = 0-9
    int is_active;
} CircleNode;

typedef struct {
    float x, y, w, h;
} Obstacle;

Obstacle pause;

// --- GLOBAL VARIABLES ---
CP_Image background_image;
CP_Image player_image;
CP_Image laptop_image;
CP_Image coffee_machine_image;
CP_Image mug_image;
CP_Image pause_image;
CP_Font myFont;

// Player variables
float player_x, player_y;
float player_speed;
float player_width, player_height;

// Collision Variables
Obstacle walls[MAX_OBSTACLES];
int wall_count = 0;

// Energy Variables
float current_energy;

// Game State Variables
int is_tutorial_active = 1;
int is_sleeping = 0;
float sleep_timer = 0.0f;
int sleep_count = 0;
int game_over = 0;
int game_over_reason = 0; // 0 = None, 1 = Fired (Sleep), 2 = Time's Up (Success)
float game_timer = 0.0f;

// Mug Variables
float mug_coffee_current;
float mug_x, mug_y;
int holding_mug = 0;

// Machine Variables
float machine_x = 1350.0f;
float machine_y = 85.0f;
float machine_w = 95.0f;
float machine_h = 135.0f;

// Laptop Variables
float laptop_x = 250.0f;
float laptop_y = 280.0f;
int is_laptop_open = 0;

// Minigame Variables
CircleNode game_circles[MAX_POSSIBLE_CIRCLES];
int current_level_circles = 5;

int next_target_index = 0;
int game_completed = 0;
int current_score = 0;

static int isPaused = 0;
static int level = 7;

// --- KEY MAPPING ---
// Maps 0-25 to A-Z and 26-35 to 0-9
CP_KEY GetKeyForID(int id) {
    if (id >= 0 && id <= 25) {
        switch (id) {
        case 0: return KEY_A; case 1: return KEY_B; case 2: return KEY_C; case 3: return KEY_D;
        case 4: return KEY_E; case 5: return KEY_F; case 6: return KEY_G; case 7: return KEY_H;
        case 8: return KEY_I; case 9: return KEY_J; case 10: return KEY_K; case 11: return KEY_L;
        case 12: return KEY_M; case 13: return KEY_N; case 14: return KEY_O; case 15: return KEY_P;
        case 16: return KEY_Q; case 17: return KEY_R; case 18: return KEY_S; case 19: return KEY_T;
        case 20: return KEY_U; case 21: return KEY_V; case 22: return KEY_W; case 23: return KEY_X;
        case 24: return KEY_Y; case 25: return KEY_Z;
        }
    }
    else if (id >= 26 && id <= 35) {
        switch (id) {
        case 26: return KEY_0; case 27: return KEY_1; case 28: return KEY_2; case 29: return KEY_3;
        case 30: return KEY_4; case 31: return KEY_5; case 32: return KEY_6; case 33: return KEY_7;
        case 34: return KEY_8; case 35: return KEY_9;
        }
    }
    return KEY_A;
}

// --- COLLISION SETUP ---
void AddWall(float x, float y, float w, float h) {
    if (wall_count < MAX_OBSTACLES) {
        walls[wall_count].x = x;
        walls[wall_count].y = y;
        walls[wall_count].w = w;
        walls[wall_count].h = h;
        wall_count++;
    }
}

// --- WALL COORDINATES ---
void InitializeObstacles(void) {
    wall_count = 0;
    AddWall(74, 75, 898, 24);
    AddWall(74, 75, 25, 692);
    AddWall(73, 741, 900, 27);
    AddWall(948, 568, 26, 199);
    AddWall(948, 568, 343, 27);
    AddWall(1265, 569, 29, 112);
    AddWall(1153, 655, 138, 95);
    AddWall(1151, 658, 26, 200);
    AddWall(1150, 830, 435, 28);
    AddWall(1558, 657, 27, 201);
    AddWall(1417, 656, 142, 92);
    AddWall(1416, 570, 27, 111);
    AddWall(1416, 570, 182, 26);
    AddWall(1416, 183, 184, 157);
    AddWall(1416, 0, 27, 208);
    AddWall(1216, 0, 77, 208);
    AddWall(950, 180, 343, 28);
    AddWall(946, 76, 30, 132);
    AddWall(100, 100, 843, 148);
    AddWall(947, 209, 345, 137);
    AddWall(103, 246, 314, 96);
    AddWall(425, 245, 285, 37);
    AddWall(717, 249, 226, 79);
    AddWall(714, 330, 73, 74);
    AddWall(372, 673, 261, 66);
    AddWall(947, 209, 347, 137);
    AddWall(1076, 339, 67, 55);
    AddWall(1295, 47, 121, 69);
    AddWall(1560, 200, 29, 500);
}

// --- CHECK COLLISION ---
int CheckCollision(float new_x, float new_y) {

    float p_left = new_x - (PLAYER_HITBOX_W / 2.0f);
    float p_right = new_x + (PLAYER_HITBOX_W / 2.0f);
    float p_top = new_y - (PLAYER_HITBOX_H / 2.0f) + PLAYER_OFFSET_Y;
    float p_bottom = new_y + (PLAYER_HITBOX_H / 2.0f) + PLAYER_OFFSET_Y;

    for (int i = 0; i < wall_count; i++) {
        Obstacle w = walls[i];
        if (p_left < w.x + w.w && p_right > w.x && p_top < w.y + w.h && p_bottom > w.y) {
            return 1;
        }
    }
    return 0;
}

// --- RESET LAPTOP MINIGAME ---
void ResetLaptopGame(void) {
    float screen_w = (float)CP_System_GetWindowWidth();
    float screen_h = (float)CP_System_GetWindowHeight();
    float win_w = 600.0f, win_h = 400.0f;
    float win_x = (screen_w / 2.0f) - (win_w / 2.0f);
    float win_y = (screen_h / 2.0f) - (win_h / 2.0f);

    float min_x = win_x + CIRCLE_RADIUS + 20.0f;
    float max_x = win_x + win_w - CIRCLE_RADIUS - 20.0f;
    float min_y = win_y + 60.0f + CIRCLE_RADIUS + 10.0f;
    float max_y = win_y + win_h - 40.0f - CIRCLE_RADIUS - 10.0f;

    // 1. Determine number of circles
    current_level_circles = CP_Random_RangeInt(4, MAX_POSSIBLE_CIRCLES);
    if (current_level_circles > MAX_POSSIBLE_CIRCLES) current_level_circles = MAX_POSSIBLE_CIRCLES;
    // 2. Select unique random IDs (0-35)
    int char_pool[36];
    for (int i = 0; i < 36; i++) char_pool[i] = i;

    // Fisher-Yates shuffle
    for (int i = 35; i > 0; i--) {
        int j = CP_Random_RangeInt(0, i);
        int temp = char_pool[i];
        char_pool[i] = char_pool[j];
        char_pool[j] = temp;
    }

    // Take the first 'current_level_circles' from the shuffled array
    int chosen_ids[MAX_POSSIBLE_CIRCLES];
    for (int i = 0; i < current_level_circles; i++) {
        chosen_ids[i] = char_pool[i];
    }

    // 3. Sort the chosen IDs for sequential visual order
    for (int i = 0; i < current_level_circles - 1; i++) {
        for (int j = 0; j < current_level_circles - i - 1; j++) {
            if (chosen_ids[j] > chosen_ids[j + 1]) {
                int temp = chosen_ids[j];
                chosen_ids[j] = chosen_ids[j + 1];
                chosen_ids[j + 1] = temp;
            }
        }
    }

    // 4. Assign IDs and Random Positions
    for (int i = 0; i < current_level_circles; i++) {
        game_circles[i].id = chosen_ids[i];
        game_circles[i].is_active = 1;

        int placed = 0;
        int attempts = 0;
        while (!placed && attempts < MAX_PLACEMENT_ATTEMPTS) {
            float new_x = CP_Random_RangeFloat(min_x, max_x);
            float new_y = CP_Random_RangeFloat(min_y, max_y);

            int intersects = 0;
            for (int j = 0; j < i; j++) {

                float dx = new_x - game_circles[j].x;
                float dy = new_y - game_circles[j].y;
                float distance = (float)sqrt(dx * dx + dy * dy);
                if (distance < (2 * CIRCLE_RADIUS) + 5.0f) { intersects = 1; break; }
            }
            if (!intersects) { game_circles[i].x = new_x; game_circles[i].y = new_y; placed = 1; }
            attempts++;
        }
    }

    next_target_index = 0;
    game_completed = 0;
}

// --- RESET ALL GAME VARIABLES ---
void ResetGameVariables(void) {
    player_x = CP_System_GetWindowWidth() / 2.0f;
    player_y = CP_System_GetWindowHeight() / 2.0f;
    player_speed = 400.0f;
    player_width = 250.0f;
    player_height = 250.0f;

    current_energy = MAX_ENERGY;
    mug_coffee_current = MAX_MUG_COFFEE;
    mug_x = 500.0f;
    mug_y = 690.0f;
    holding_mug = 0;
    is_laptop_open = 0;
    current_score = 0;

    is_sleeping = 0;
    sleep_timer = 0.0f;
    sleep_count = 0;
    game_over = 0;
    game_over_reason = 0;

    game_timer = GAME_TIME_LIMIT;
    is_tutorial_active = 1;
    Score_LevelStart(level);
    InitializeObstacles();
    ResetLaptopGame();
}

// --- GAME FUNCTIONS ---

void Coffee_Driven1_Init(void) {
    CP_System_SetWindowSize(1600, 900);
    CP_Settings_AntiAlias(0);
    CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_NEAREST);

    background_image = CP_Image_Load("./Assets/OfficeBG2.png");
    player_image = CP_Image_Load("./Assets/Player.png");
    laptop_image = CP_Image_Load("./Assets/ComputerOn.png");
    coffee_machine_image = CP_Image_Load("./Assets/CoffeeMachine.png");
    mug_image = CP_Image_Load("./Assets/CoffeeCup.png");

    myFont = CP_Font_Load("./Assets/Pixellari.ttf");
    if (myFont == NULL) printf("ERROR: Could not load font Assets/Pixellari.ttf\n");

    ResetGameVariables();

    Score_LevelStart(level);

    pause.x = 1550.f;
    pause.y = 50.f;
    pause.w = 75.f;
    pause.h = 75.f;
}

void Coffee_Driven1_Update(void) {
    if (isPaused) {
        CP_Graphics_ClearBackground(CP_Color_Create(245, 222, 179, 255));

        Pause_Game_Init();

        int game_type = Pause_Game_Update();

        if (game_type == 0) {
            if (!game_over) {
                Score_LevelStart(level);
                Score_LevelEnd();
                game_over = 1;
            }
            isPaused = 0;
            CP_Engine_SetNextGameStateForced(Coffee_Driven1_Init, Coffee_Driven1_Update, Coffee_Driven1_Exit);
        }
        else if (game_type == 1) {
            if (!game_over) {
                Score_LevelStart(level);
                Score_LevelEnd();
                game_over = 1;
            }
            isPaused = 0;
            CP_Engine_SetNextGameStateForced(Engineer_Levels_Init, Engineer_Levels_Update, Engineer_Levels_Exit);
        }
        else if (game_type == -1) {
            isPaused = 0;
        }
    }

    if (!isPaused) {
        CP_Settings_RectMode(CP_POSITION_CENTER);
        CP_Settings_StrokeWeight(4.0f);

        // Draw Static Background 
        CP_Settings_ImageMode(CP_POSITION_CORNER);
        CP_Image_Draw(background_image, 0.f, 0.f, (float)CP_System_GetWindowWidth(), (float)CP_System_GetWindowHeight(), 255);

        float dt = CP_System_GetDt();
        CP_Font_Set(myFont);
        float w = (float)CP_System_GetWindowWidth();
        float h = (float)CP_System_GetWindowHeight();

        // --- 0. TUTORIAL WINDOW ---
        if (is_tutorial_active) {

            CP_Settings_Fill(CP_Color_Create(0, 0, 0, 220));
            CP_Settings_NoStroke();
            CP_Graphics_DrawRect(w / 2.0f, h / 2.0f, w, h);


            float box_w = 800.0f;
            float box_h = 600.0f;
            CP_Settings_ImageMode(CP_POSITION_CENTER);

            CP_Settings_Fill(CP_Color_Create(245, 222, 179, 255));
            CP_Settings_Stroke(CP_Color_Create(0, 0, 0, 255));
            CP_Settings_StrokeWeight(4.0f);
            CP_Graphics_DrawRect(w / 2.0f, h / 2.0f, box_w, box_h);


            CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
            CP_Settings_TextSize(60.0f);
            CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_TOP);
            CP_Font_DrawText("EMPLOYEE ORIENTATION", w / 2.0f, h / 2.0f - box_h / 2.0f + 30.0f);


            CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
            CP_Settings_TextSize(30.0f);
            CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_TOP);

            float text_x_offset = 50.0f;
            float text_center_x = w / 2.0f - box_w / 2.0f + text_x_offset;
            float text_start_y = h / 2.0f - 200.0f;
            float line_h = 40.0f;

            // --- TUTORIAL ---
            CP_Font_DrawText("- Use WASD to Move", text_center_x, text_start_y);
            CP_Font_DrawText("- Press E near Laptop to WORK", text_center_x, text_start_y + line_h);
            CP_Font_DrawText("- Press E near Machine to REFILL COFFEE", text_center_x, text_start_y + line_h * 2);
            CP_Font_DrawText("- Press E to Pick Up / Drop Coffee Mug", text_center_x, text_start_y + line_h * 3);
            CP_Font_DrawText("- Press SPACE to Drink (Regain Energy)", text_center_x, text_start_y + line_h * 4);
            CP_Font_DrawText("- Press ESC to Close Laptop", text_center_x, text_start_y + line_h * 5);
            CP_Font_DrawText("- WORK: Type characters in circles!", text_center_x, text_start_y + line_h * 6);

            CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

            CP_Settings_Fill(CP_Color_Create(0, 180, 0, 255));
            CP_Font_DrawText("GOAL: Try and finish as many projects as possible!", w / 2.0f, h / 2.0f + box_h / 2.0f - 180.0f);

            CP_Settings_Fill(CP_Color_Create(200, 0, 0, 255));
            CP_Font_DrawText("WARNING: If you fall asleep 3 times, you will be FIRED!", w / 2.0f, h / 2.0f + box_h / 2.0f - 140.0f);


            float btn_w = 300.0f, btn_h = 70.0f;
            float btn_x = w / 2.0f;
            float btn_y = h / 2.0f + box_h / 2.0f - 80.0f;
            float mx = CP_Input_GetMouseX(), my = CP_Input_GetMouseY();


            if (mx > btn_x - btn_w / 2.0f && mx < btn_x + btn_w / 2.0f && my > btn_y - btn_h / 2.0f && my < btn_y + btn_h / 2.0f) {
                CP_Settings_Fill(CP_Color_Create(0, 200, 0, 255));
                if (CP_Input_MouseClicked()) {
                    is_tutorial_active = 0;
                }
            }
            else {
                CP_Settings_Fill(CP_Color_Create(0, 120, 0, 255));
            }

            CP_Settings_Stroke(CP_Color_Create(0, 0, 0, 255));
            CP_Graphics_DrawRect(btn_x, btn_y, btn_w, btn_h);

            CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
            CP_Settings_TextSize(40.0f);
            CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
            CP_Font_DrawText("START SHIFT", btn_x, btn_y);

            return;
        }

        // --- CHECK GAME OVER STATUS ---
        if (game_over) {
            if (game_over_reason == 2) { // Success
                CP_Settings_Fill(CP_Color_Create(0, 150, 0, 255));
                CP_Graphics_DrawRect(w / 2.0f, h / 2.0f, w, h);

                CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
                CP_Settings_TextSize(80.0f);
                CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

                CP_Font_DrawText("SHIFT COMPLETE!", w / 2.0f, h / 2.0f - 80.0f);
                CP_Settings_TextSize(40.0f);
                CP_Font_DrawText("Good job surviving a hard day of work!", w / 2.0f, h / 2.0f);

                char end_score[50]; sprintf_s(end_score, 50, "Final Score: %d", Score_Get());
                CP_Font_DrawText(end_score, w / 2.0f, h / 2.0f + 40.0f);

                float btn_w = 250.0f, btn_h = 60.0f;
                float btn_x = w / 2.0f - btn_w / 2.0f;
                float btn_y = h / 2.0f + 100.0f;
                float mx = CP_Input_GetMouseX(), my = CP_Input_GetMouseY();

                if (mx > btn_x && mx < btn_x + btn_w && my > btn_y && my < btn_y + btn_h) {
                    CP_Settings_Fill(CP_Color_Create(50, 200, 50, 255));
                    if (CP_Input_MouseClicked()) {
                        CP_Engine_SetNextGameState(Coffee_Driven2_Init, Coffee_Driven2_Update, Coffee_Driven2_Exit);
                    }
                }
                else {
                    CP_Settings_Fill(CP_Color_Create(0, 100, 0, 255));
                }

                CP_Settings_Stroke(CP_Color_Create(255, 255, 255, 255));
                CP_Graphics_DrawRect(btn_x + btn_w / 2.0f, btn_y + btn_h / 2.0f, btn_w, btn_h);
                CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
                CP_Settings_TextSize(30.0f);
                CP_Font_DrawText("NEXT STAGE", w / 2.0f, btn_y + btn_h / 2.0f);
            }
            else { // Fired
                CP_Settings_Fill(CP_Color_Create(150, 0, 0, 255));
                CP_Graphics_DrawRect(w / 2.0f, h / 2.0f, w, h);

                CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
                CP_Settings_TextSize(80.0f);
                CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

                CP_Font_DrawText("FIRED!", w / 2.0f, h / 2.0f - 80.0f);
                CP_Settings_TextSize(40.0f);
                CP_Font_DrawText("You fell asleep 3 times. You are fired!", w / 2.0f, h / 2.0f);

                char end_score[50]; sprintf_s(end_score, 50, "Final Score: %d", Score_Get());
                CP_Font_DrawText(end_score, w / 2.0f, h / 2.0f + 40.0f);

                // --- RETRY BUTTON ---
                float btn_w = 200.0f, btn_h = 60.0f;
                float btn_x = w / 2.0f - btn_w / 2.0f;
                float btn_y = h / 2.0f + 100.0f;
                float mx = CP_Input_GetMouseX(), my = CP_Input_GetMouseY();

                if (mx > btn_x && mx < btn_x + btn_w && my > btn_y && my < btn_y + btn_h) {
                    CP_Settings_Fill(CP_Color_Create(50, 200, 50, 255));
                    if (CP_Input_MouseClicked()) ResetGameVariables();
                }
                else {
                    CP_Settings_Fill(CP_Color_Create(100, 100, 100, 255));
                }
                CP_Settings_Stroke(CP_Color_Create(255, 255, 255, 255));
                CP_Graphics_DrawRect(btn_x + btn_w / 2.0f, btn_y + btn_h / 2.0f, btn_w, btn_h);
                CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
                CP_Settings_TextSize(30.0f);
                CP_Font_DrawText("RETRY", w / 2.0f, btn_y + btn_h / 2.0f);

                // --- MENU BUTTON ---
                float menu_btn_y = btn_y + 80.0f;

                if (mx > btn_x && mx < btn_x + btn_w && my > menu_btn_y && my < menu_btn_y + btn_h) {
                    CP_Settings_Fill(CP_Color_Create(50, 50, 200, 255));
                    if (CP_Input_MouseClicked()) CP_Engine_SetNextGameState(Engineer_Levels_Init, Engineer_Levels_Update, Engineer_Levels_Exit);
                }
                else {
                    CP_Settings_Fill(CP_Color_Create(0, 0, 150, 255));
                }

                CP_Settings_Stroke(CP_Color_Create(255, 255, 255, 255));
                CP_Graphics_DrawRect(btn_x + btn_w / 2.0f, menu_btn_y + btn_h / 2.0f, btn_w, btn_h);

                CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
                CP_Settings_TextSize(30.0f);
                CP_Font_DrawText("MENU", w / 2.0f, menu_btn_y + btn_h / 2.0f);
            }
            return;
        }

        // --- LOGIC & TIMERS ---

        if (!game_over && !is_sleeping) {
            game_timer -= dt;
            if (game_timer <= 0.0f) {
                game_timer = 0.0f;
                Score_LevelEnd();
                game_over = 1;
                game_over_reason = 2;
            }
        }

        if (is_sleeping) {
            sleep_timer -= dt;
            if (sleep_timer <= 0.0f) {
                is_sleeping = 0;
                current_energy = REGEN_AMOUNT;
            }
        }
        else {
            current_energy -= ENERGY_DECAY_RATE * dt;
            if (current_energy <= 0.0f) {
                current_energy = 0.0f;
                sleep_count++;
                is_laptop_open = 0;
                if (sleep_count >= MAX_SLEEPS_ALLOWED) {
                    Score_LevelStart(level);
                    Score_LevelEnd();
                    game_over = 1;
                    game_over_reason = 1; // Fired
                }
                else {
                    is_sleeping = 1;
                    sleep_timer = SLEEP_DURATION;
                }
            }

            // --- MOVEMENT ---
            if (is_laptop_open == 0) {
                float proposed_x = player_x;
                float proposed_y = player_y;

                if (CP_Input_KeyDown(KEY_A)) proposed_x -= player_speed * dt;
                if (CP_Input_KeyDown(KEY_D)) proposed_x += player_speed * dt;
                if (CheckCollision(proposed_x, player_y) == 0) player_x = proposed_x;
                if (CP_Input_KeyDown(KEY_W)) proposed_y -= player_speed * dt;
                if (CP_Input_KeyDown(KEY_S)) proposed_y += player_speed * dt;
                if (CheckCollision(player_x, proposed_y) == 0) player_y = proposed_y;
            }

            // --- DISTANCES FOR ZONING ---
            float dist_to_laptop = (float)sqrt(pow(player_x - laptop_x, 2) + pow(player_y - laptop_y, 2));
            float dist_to_mug = (float)sqrt(pow(player_x - mug_x, 2) + pow(player_y - mug_y, 2));
            float dist_to_machine = (float)sqrt(pow(player_x - machine_x, 2) + pow(player_y - machine_y, 2));

            // --- CLOSE LAPTOP LOGIC ---
            if (is_laptop_open == 1 && CP_Input_KeyTriggered(KEY_ESCAPE)) {
                is_laptop_open = 0;
            }

            // --- CONSOLIDATED "E" KEY INTERACTION ---
            if (CP_Input_KeyTriggered(KEY_E) && is_laptop_open == 0) {

                //OPEN LAPTOP
                if (dist_to_laptop < 150.0f) {
                    is_laptop_open = 1;
                }
                //REFILL COFFEE 
                else if (dist_to_machine < 120.0f && holding_mug == 1) {
                    mug_coffee_current = MAX_MUG_COFFEE;
                }
                //PICK UP / DROP MUG 
                else {
                    if (holding_mug == 1) {
                        holding_mug = 0;
                    }
                    else if (dist_to_mug < INTERACTION_DIST) {
                        holding_mug = 1; // Pick it up
                    }
                }
            }

            // --- MUG POSITION UPDATE ---
            if (holding_mug == 1) {
                mug_x = player_x + MUG_OFFSET_X;
                mug_y = player_y + MUG_OFFSET_Y;
            }

            // --- DRINKING LOGIC ---
            if (holding_mug == 1 && CP_Input_KeyTriggered(KEY_SPACE) && mug_coffee_current > 0 && is_laptop_open == 0) {
                current_energy += SIP_AMOUNT;
                if (current_energy > MAX_ENERGY) current_energy = MAX_ENERGY;
                mug_coffee_current -= SIP_AMOUNT;
                if (mug_coffee_current < 0) mug_coffee_current = 0.0f;
            }
        } // <--- THIS WAS THE MISSING BRACKET

        // --- DEBUGGING PURPOSES ---
        if (DEBUG_HITBOXES) {
            CP_Settings_NoFill();
            CP_Settings_Stroke(CP_Color_Create(255, 0, 0, 255));
            CP_Settings_StrokeWeight(2.0f);
            for (int i = 0; i < wall_count; i++) {
                CP_Graphics_DrawRect(walls[i].x + walls[i].w / 2.0f, walls[i].y + walls[i].h / 2.0f, walls[i].w, walls[i].h);
            }

            CP_Settings_Stroke(CP_Color_Create(0, 255, 0, 255));
            CP_Graphics_DrawRect(player_x, player_y + PLAYER_OFFSET_Y, PLAYER_HITBOX_W, PLAYER_HITBOX_H);
        }

        CP_Settings_ImageMode(CP_POSITION_CENTER);
        CP_Image_Draw(laptop_image, laptop_x, laptop_y, 150.f, 150.f, 255);
        CP_Image_Draw(coffee_machine_image, machine_x, machine_y, machine_w, machine_h, 255);
        CP_Image_Draw(player_image, player_x, player_y, player_width, player_height, 255);
        CP_Image_Draw(mug_image, mug_x, mug_y, 100.f, 100.f, 255);

        // --- UI BARS ---

        // Player Bar
        float p_bar_left = player_x + 50.0f;
        float p_bar_top = player_y - (PLAYER_BAR_H / 2.0f);

        CP_Settings_Fill(CP_Color_Create(255, 0, 0, 255));
        CP_Settings_Stroke(CP_Color_Create(0, 0, 0, 255));
        CP_Graphics_DrawRect(p_bar_left + PLAYER_BAR_W / 2.0f, p_bar_top + PLAYER_BAR_H / 2.0f, PLAYER_BAR_W, PLAYER_BAR_H);

        if (current_energy > 0) {
            float p_fill_h = (current_energy / MAX_ENERGY) * PLAYER_BAR_H;
            CP_Settings_Fill(CP_Color_Create(0, 255, 0, 255));
            CP_Settings_NoStroke();
            CP_Graphics_DrawRect(p_bar_left + PLAYER_BAR_W / 2.0f, p_bar_top + PLAYER_BAR_H - (p_fill_h / 2.0f), PLAYER_BAR_W, p_fill_h);
        }

        // Mug Bar
        float m_bar_left = mug_x + 40.0f;
        float m_bar_top = mug_y - (MUG_BAR_H / 2.0f);

        CP_Settings_Stroke(CP_Color_Create(0, 0, 0, 255));
        CP_Settings_Fill(CP_Color_Create(50, 50, 50, 255));
        CP_Graphics_DrawRect(m_bar_left + MUG_BAR_W / 2.0f, m_bar_top + MUG_BAR_H / 2.0f, MUG_BAR_W, MUG_BAR_H);

        if (mug_coffee_current > 0) {
            float m_fill_h = (mug_coffee_current / MAX_MUG_COFFEE) * MUG_BAR_H;
            CP_Settings_Fill(CP_Color_Create(0, 191, 255, 255));
            CP_Settings_NoStroke();
            CP_Graphics_DrawRect(m_bar_left + MUG_BAR_W / 2.0f, m_bar_top + MUG_BAR_H - (m_fill_h / 2.0f), MUG_BAR_W, m_fill_h);
        }

        // --- HUD ---
        CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
        // Project
        int percent = 0;
        if (current_level_circles > 0) percent = (int)(((float)(next_target_index) / (float)current_level_circles) * 100.0f);
        if (percent > 100) percent = 100;
        CP_Settings_TextSize(50.0f);
        CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_TOP);
        char proj_buffer[50]; sprintf_s(proj_buffer, 50, "Project: %d%%", percent);
        CP_Font_DrawText(proj_buffer, 20.0f, 20.0f);

        // Timer
        int minutes = (int)game_timer / 60;
        int seconds = (int)game_timer % 60;
        char timer_buffer[50]; sprintf_s(timer_buffer, 50, "%02d:%02d", minutes, seconds);
        if (game_timer < 30.0f) CP_Settings_Fill(CP_Color_Create(255, 50, 50, 255));
        else CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
        CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_TOP);
        CP_Font_DrawText(timer_buffer, (float)CP_System_GetWindowWidth() * 0.25f, 20.0f);

        // Score
        CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
        char score_buffer[50]; sprintf_s(score_buffer, 50, "Score: %d", Score_Get());
        CP_Font_DrawText(score_buffer, (float)CP_System_GetWindowWidth() / 2.0f, 20.0f);

        // Naps
        char sleep_buffer[50]; sprintf_s(sleep_buffer, 50, "Naps: %d/%d", sleep_count, MAX_SLEEPS_ALLOWED);
        CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_BOTTOM);
        if (sleep_count == 2) CP_Settings_Fill(CP_Color_Create(255, 100, 100, 255));
        else CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
        CP_Font_DrawText(sleep_buffer, 20.0f, (float)CP_System_GetWindowHeight() - 20.0f);

        if (is_sleeping) {

            CP_Settings_Fill(CP_Color_Create(0, 0, 50, 150));
            CP_Settings_NoStroke();
            CP_Graphics_DrawRect((float)CP_System_GetWindowWidth() / 2.0f, (float)CP_System_GetWindowHeight() / 2.0f, (float)CP_System_GetWindowWidth(), (float)CP_System_GetWindowHeight());
            CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
            CP_Settings_TextSize(60.0f);
            CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
            CP_Font_DrawText("Zzz... Regenerating...", (float)CP_System_GetWindowWidth() / 2.0f, (float)CP_System_GetWindowHeight() / 2.0f);
        }

        // --- LAPTOP MINIGAME ---
        if (is_laptop_open == 1 && !is_sleeping) {
            float screen_w = (float)CP_System_GetWindowWidth();
            float screen_h = (float)CP_System_GetWindowHeight();
            float win_w = 600.0f, win_h = 400.0f;
            float win_x = (screen_w - win_w) / 2.0f, win_y = (screen_h - win_h) / 2.0f;

            CP_Settings_Fill(CP_Color_Create(0, 0, 0, 180));
            CP_Settings_NoStroke();
            CP_Graphics_DrawRect(screen_w / 2.0f, screen_h / 2.0f, screen_w, screen_h);

            CP_Settings_Fill(CP_Color_Create(240, 240, 240, 255));
            CP_Settings_Stroke(CP_Color_Create(0, 0, 0, 255));
            CP_Settings_StrokeWeight(3.0f);
            CP_Graphics_DrawRect(win_x + win_w / 2.0f, win_y + win_h / 2.0f, win_w, win_h);

            CP_Settings_Fill(CP_Color_Create(0, 0, 128, 255));
            CP_Graphics_DrawRect(win_x + win_w / 2.0f, win_y + 20.0f, win_w, 40.0f);

            // Check if index reached end
            if (next_target_index >= current_level_circles) game_completed = 1;

            if (game_completed == 0) {
                for (int i = 0; i < current_level_circles; i++) {
                    if (game_circles[i].is_active == 1) {
                        CP_KEY target_key = GetKeyForID(game_circles[i].id);
                        if (CP_Input_KeyTriggered(target_key)) {
                            game_circles[i].is_active = 0;
                            next_target_index++;
                        }
                    }
                }

                // Drawing Circles
                for (int i = 0; i < current_level_circles; i++) {
                    if (game_circles[i].is_active == 1) {
                        CP_Settings_Fill(CP_Color_Create(255, 105, 180, 255));
                        CP_Settings_Stroke(CP_Color_Create(255, 255, 255, 255));
                        CP_Settings_StrokeWeight(2.0f);
                        CP_Graphics_DrawCircle(game_circles[i].x, game_circles[i].y, CIRCLE_RADIUS * 2.0f);

                        CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
                        CP_Settings_TextSize(30.0f);
                        CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);


                        char buffer[5];
                        if (game_circles[i].id < 26) {
                            sprintf_s(buffer, 5, "%c", 'A' + game_circles[i].id);
                        }
                        else {
                            sprintf_s(buffer, 5, "%c", '0' + (game_circles[i].id - 26));
                        }
                        CP_Font_DrawText(buffer, game_circles[i].x, game_circles[i].y);
                    }
                }
            }
            else {
                CP_Settings_Fill(CP_Color_Create(0, 150, 0, 255));
                CP_Settings_TextSize(50.0f);
                CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
                CP_Font_DrawText("Project Completed!", win_x + win_w / 2.0f, win_y + 150.0f);

                float btn_w = 220.0f, btn_h = 60.0f;
                float btn_x = win_x + (win_w - btn_w) / 2.0f;
                float btn_y = win_y + 250.0f;
                float mx = CP_Input_GetMouseX(), my = CP_Input_GetMouseY();

                if (mx > btn_x && mx < btn_x + btn_w && my > btn_y && my < btn_y + btn_h) {
                    CP_Settings_Fill(CP_Color_Create(50, 50, 200, 255));
                    if (CP_Input_MouseClicked()) { Score_AddPoints(10); ResetLaptopGame(); }
                }
                else CP_Settings_Fill(CP_Color_Create(0, 0, 150, 255));

                CP_Settings_Stroke(CP_Color_Create(0, 0, 0, 255));
                CP_Graphics_DrawRect(btn_x + btn_w / 2.0f, btn_y + btn_h / 2.0f, btn_w, btn_h);
                CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
                CP_Settings_TextSize(30.0f);
                CP_Font_DrawText("Next Project", btn_x + btn_w / 2.0f, btn_y + btn_h / 2.0f);
            }

            float bar_max_w = win_w - 40.0f, bar_h = 20.0f;
            float bar_x = win_x + 20.0f, bar_y = win_y + win_h - 40.0f;

            CP_Settings_Fill(CP_Color_Create(200, 200, 200, 255));
            CP_Settings_NoStroke();
            CP_Graphics_DrawRect(bar_x + bar_max_w / 2.0f, bar_y + bar_h / 2.0f, bar_max_w, bar_h);

            float progress_pct = (float)percent / 100.0f;

            float fill_w = bar_max_w * progress_pct;
            CP_Settings_Fill(CP_Color_Create(255, 215, 0, 255));
            if (fill_w > 0) {
                CP_Graphics_DrawRect(bar_x + fill_w / 2.0f, bar_y + bar_h / 2.0f, fill_w, bar_h);
            }
        }

        float mouseX = CP_Input_GetMouseX();
        float mouseY = CP_Input_GetMouseY();

        /* ---------------- PAUSE GAMEPLAY ---------------- */
        if (IsAreaHovered(pause.x, pause.y, pause.w, pause.h, mouseX, mouseY)) {
            pause_image = CP_Image_Load("Assets/brown_pause.png");
        }
        else {
            pause_image = CP_Image_Load("Assets/pause.png");
        }

        CP_Image_Draw(pause_image, pause.x, pause.y, pause.w, pause.w, 255);

        if (IsAreaClicked(pause.x, pause.y, pause.w, pause.h, mouseX, mouseY)) {
            if (CP_Input_MouseClicked())
            {
                isPaused = 1;
            }
        }
    }
}

void Coffee_Driven1_Exit(void) {
    CP_Image_Free(background_image);
    CP_Image_Free(player_image);
    CP_Image_Free(laptop_image);
    CP_Image_Free(coffee_machine_image);
    CP_Image_Free(mug_image);
    CP_Image_Free(pause_image);
    CP_Font_Free(myFont);
    CP_Graphics_ClearBackground(CP_Color_Create(245, 222, 179, 255));
}