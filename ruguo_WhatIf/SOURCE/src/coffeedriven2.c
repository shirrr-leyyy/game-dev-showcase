//---------------------------------------------------------
// file:	coffeedriven2.c
// author:	Felicia
// email:	f.goh@digipen.edu
//
// brief:	Software Engineer Level 4 Game - Coffee driven
//---------------------------------------------------------
#include "cprocessing.h"
#include "engineerlevels.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pausegame.h"
#include "score.h"
#include "coffeedriven2.h"
#include "utils.h"
#include "payslip.h"


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
#define SIP_AMOUNT 40.0f
#define MUG_BAR_W 10.0f
#define MUG_BAR_H 50.0f
#define INTERACTION_DIST 100.0f
#define MUG_OFFSET_X 40.0f
#define MUG_OFFSET_Y 40.0f

// --- BANANA DEFINITIONS ---
#define MAX_BANANAS 8
#define BANANA_SIZE 80.0f
#define BANANA_LIFETIME 8.0f
#define BANANA_SPAWN_RATE 2.0f
#define BANANA_DAMAGE 15.0f

// --- LAPTOP MINIGAME DEFINITIONS ---
#define MAX_POSSIBLE_CIRCLES 10
#define CIRCLE_RADIUS 25.0f
#define MAX_PLACEMENT_ATTEMPTS 50

// ID Ranges for Minigame
#define ID_RANGE_MAX 66

typedef struct {
    float x, y;
    int id;
    int is_active;
} CircleNode;

typedef struct {
    float x, y, w, h;
} Obstacle;

typedef struct {
    float x, y;
    float timer;
    int active;
} Banana;

Obstacle pause;

// --- GLOBAL VARIABLES ---
static CP_Image background_image;
static CP_Image player_image;
static CP_Image laptop_image;
static CP_Image coffee_machine_image;
static CP_Image mug_image;
static CP_Image banana_image;
static CP_Image pause_image;
static CP_Font myFont;

// Player variables
static float player_x, player_y;
static float player_speed;
static float player_width, player_height;

// Collision Variables
static Obstacle walls[MAX_OBSTACLES];
static int wall_count = 0;

// Energy Variables
static float current_energy;

// Game State Variables
static int is_tutorial_active = 1;
static int is_sleeping = 0;
static float sleep_timer = 0.0f;
static int sleep_count = 0;
static int game_over = 0;
static int game_over_reason = 0;
static float game_timer = 0.0f;

// Mug Variables
static float mug_coffee_current;
static float mug_x, mug_y;
static int holding_mug = 0;

// Machine Variables
static float machine_x = 1350.0f;
static float machine_y = 85.0f;
static float machine_w = 95.0f;
static float machine_h = 135.0f;

// Laptop Variables
static float laptop_x = 250.0f;
static float laptop_y = 280.0f;
static int is_laptop_open = 0;

// Banana Variables
static Banana bananas[MAX_BANANAS];
static float spawn_timer = 0.0f;

// "Woops!" Text Variables
static float woops_timer = 0.0f;
static float woops_x, woops_y;

// Minigame Variables
static CircleNode game_circles[MAX_POSSIBLE_CIRCLES];
static int current_level_circles = 5;
static int game_completed = 0;
static int current_score = 0;

static int isPaused = 0;
static int level = 8;
static int payslip = 0;

// --- KEY MAPPING ---
static CP_KEY GetKeyForID(int id) {
    if (id >= 0 && id <= 25) return (CP_KEY)(KEY_A + id);
    if (id >= 26 && id <= 35) return (CP_KEY)(KEY_0 + (id - 26));

    switch (id) {
    case 36: return KEY_MINUS;
    case 37: return KEY_EQUAL;
    case 38: return KEY_LEFT_BRACKET;
    case 39: return KEY_RIGHT_BRACKET;
    case 40: return KEY_SEMICOLON;
    case 41: return KEY_APOSTROPHE;
    case 42: return KEY_COMMA;
    case 43: return KEY_PERIOD;
    case 44: return KEY_SLASH;
    case 45: return KEY_BACKSLASH;
    case 46: return KEY_GRAVE_ACCENT;
    case 47: return KEY_1;
    case 48: return KEY_2;
    case 49: return KEY_3;
    case 50: return KEY_4;
    case 51: return KEY_5;
    case 52: return KEY_6;
    case 53: return KEY_7;
    case 54: return KEY_8;
    case 55: return KEY_9;
    case 56: return KEY_0;
    case 57: return KEY_MINUS;
    case 58: return KEY_EQUAL;
    case 59: return KEY_SEMICOLON;
    case 60: return KEY_APOSTROPHE;
    case 61: return KEY_COMMA;
    case 62: return KEY_PERIOD;
    case 63: return KEY_SLASH;
    case 64: return KEY_BACKSLASH;
    case 65: return KEY_GRAVE_ACCENT;
    }
    return KEY_A;
}

static int RequiresShift(int id) {
    return (id >= 47 && id <= 65);
}

static void GetCharForID(int id, char* buffer) {
    if (id >= 0 && id <= 25) { sprintf_s(buffer, 5, "%c", 'A' + id); return; }
    if (id >= 26 && id <= 35) { sprintf_s(buffer, 5, "%c", '0' + (id - 26)); return; }

    switch (id) {
    case 36: sprintf_s(buffer, 5, "-"); break;
    case 37: sprintf_s(buffer, 5, "="); break;
    case 38: sprintf_s(buffer, 5, "["); break;
    case 39: sprintf_s(buffer, 5, "]"); break;
    case 40: sprintf_s(buffer, 5, ";"); break;
    case 41: sprintf_s(buffer, 5, "'"); break;
    case 42: sprintf_s(buffer, 5, ","); break;
    case 43: sprintf_s(buffer, 5, "."); break;
    case 44: sprintf_s(buffer, 5, "/"); break;
    case 45: sprintf_s(buffer, 5, "\\"); break;
    case 46: sprintf_s(buffer, 5, "`"); break;

    case 47: sprintf_s(buffer, 5, "!"); break;
    case 48: sprintf_s(buffer, 5, "@"); break;
    case 49: sprintf_s(buffer, 5, "#"); break;
    case 50: sprintf_s(buffer, 5, "$"); break;
    case 51: sprintf_s(buffer, 5, "%%"); break;
    case 52: sprintf_s(buffer, 5, "^"); break;
    case 53: sprintf_s(buffer, 5, "&"); break;
    case 54: sprintf_s(buffer, 5, "*"); break;
    case 55: sprintf_s(buffer, 5, "("); break;
    case 56: sprintf_s(buffer, 5, ")"); break;
    case 57: sprintf_s(buffer, 5, "_"); break;
    case 58: sprintf_s(buffer, 5, "+"); break;
    case 59: sprintf_s(buffer, 5, ":"); break;
    case 60: sprintf_s(buffer, 5, "\""); break;
    case 61: sprintf_s(buffer, 5, "<"); break;
    case 62: sprintf_s(buffer, 5, ">"); break;
    case 63: sprintf_s(buffer, 5, "?"); break;
    case 64: sprintf_s(buffer, 5, "|"); break;
    case 65: sprintf_s(buffer, 5, "~"); break;
    default: sprintf_s(buffer, 5, "?"); break;
    }
}

// --- COLLISION SETUP ---
static void AddWall(float x, float y, float w, float h) {
    if (wall_count < MAX_OBSTACLES) {
        walls[wall_count].x = x;
        walls[wall_count].y = y;
        walls[wall_count].w = w;
        walls[wall_count].h = h;
        wall_count++;
    }
}

// --- WALL COORDINATES ---
static void InitializeObstacles(void) {
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
static int CheckCollision(float new_x, float new_y) {
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

// --- CHECK IF POINT IS IN VALID FLOOR ZONE ---
static int IsPointInSpawnZone(float x, float y) {
    // Zone 1: Main Room (Left Large Area)
    if (x > 100 && x < 940 && y > 100 && y < 740) return 1;

    // Zone 2: Horizontal Hallway (Middle)
    if (x > 940 && x < 1600 && y > 350 && y < 560) return 1;

    // Zone 3: Top Right Hall (Vertical)
    if (x > 1295 && x < 1410 && y > 70 && y < 350) return 1;

    // Zone 4: Bottom Right Hall (Vertical)
    if (x > 1180 && x < 1550 && y > 560 && y < 830) return 1;

    return 0;
}

// --- SPAWN BANANA ---
static void TrySpawnOneBanana(void) {
    float screen_w = (float)CP_System_GetWindowWidth();
    float screen_h = (float)CP_System_GetWindowHeight();

    // Find first inactive slot
    int slot = -1;
    for (int i = 0; i < MAX_BANANAS; i++) {
        if (!bananas[i].active) {
            slot = i;
            break;
        }
    }

    if (slot == -1) return; // No space

    int placed = 0;
    int attempts = 0;
    while (!placed && attempts < 50) {
        float tx = CP_Random_RangeFloat(50.0f, screen_w - 50.0f);
        float ty = CP_Random_RangeFloat(50.0f, screen_h - 50.0f);


        if (IsPointInSpawnZone(tx, ty) && CheckCollision(tx, ty) == 0) {
            bananas[slot].x = tx;
            bananas[slot].y = ty;
            bananas[slot].active = 1;
            bananas[slot].timer = BANANA_LIFETIME;
            placed = 1;
        }
        attempts++;
    }
}

// --- RESET LAPTOP MINIGAME ---
static void ResetLaptopGame(void) {
    float screen_w = (float)CP_System_GetWindowWidth();
    float screen_h = (float)CP_System_GetWindowHeight();
    float win_w = 600.0f, win_h = 400.0f;
    float win_x = (screen_w / 2.0f) - (win_w / 2.0f);
    float win_y = (screen_h / 2.0f) - (win_h / 2.0f);

    float min_x = win_x + CIRCLE_RADIUS + 20.0f;
    float max_x = win_x + win_w - CIRCLE_RADIUS - 20.0f;
    float min_y = win_y + 60.0f + CIRCLE_RADIUS + 10.0f;
    float max_y = win_y + win_h - 40.0f - CIRCLE_RADIUS - 10.0f;

    current_level_circles = CP_Random_RangeInt(4, MAX_POSSIBLE_CIRCLES);
    if (current_level_circles > MAX_POSSIBLE_CIRCLES) current_level_circles = MAX_POSSIBLE_CIRCLES;

    int char_pool[ID_RANGE_MAX];
    for (int i = 0; i < ID_RANGE_MAX; i++) char_pool[i] = i;

    for (int i = ID_RANGE_MAX - 1; i > 0; i--) {
        int j = CP_Random_RangeInt(0, i);
        int temp = char_pool[i];
        char_pool[i] = char_pool[j];
        char_pool[j] = temp;
    }

    for (int i = 0; i < current_level_circles; i++) {
        game_circles[i].id = char_pool[i];
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

    game_completed = 0;
}

// --- RESET ALL GAME VARIABLES ---
static void ResetGameVariables(void) {
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

    // Init Bananas
    for (int i = 0; i < MAX_BANANAS; i++) bananas[i].active = 0;
    spawn_timer = 0.0f;
    woops_timer = 0.0f;
    
    Score_LevelStart(level);
    InitializeObstacles();
    ResetLaptopGame();
}

// --- GAME FUNCTIONS ---
void Coffee_Driven2_Init(void) {
    CP_System_SetWindowSize(1600, 900);
    CP_Settings_AntiAlias(0);
    CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_NEAREST);

    background_image = CP_Image_Load("./Assets/OfficeBG2.png");
    player_image = CP_Image_Load("./Assets/Player.png");
    laptop_image = CP_Image_Load("./Assets/ComputerOn.png");
    coffee_machine_image = CP_Image_Load("./Assets/CoffeeMachine.png");
    mug_image = CP_Image_Load("./Assets/CoffeeCup.png");
    banana_image = CP_Image_Load("./Assets/BananaPeel.png");

    myFont = CP_Font_Load("./Assets/Pixellari.ttf");
    if (myFont == NULL) printf("ERROR: Could not load font Assets/Pixellari.ttf\n");

    ResetGameVariables();

    Score_LevelStart(level);

    pause.x = 1550.f;
    pause.y = 50.f;
    pause.w = 75.f;
    pause.h = 75.f;
}

void Coffee_Driven2_Update(void) {
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
            CP_Engine_SetNextGameStateForced(Coffee_Driven2_Init, Coffee_Driven2_Update, Coffee_Driven2_Exit);
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

    if (payslip) {
        Payslip_Init();
        int continue_game = Payslip_Update(2);
        if (continue_game == 1) {
            payslip = 0;
        }
    }

    if (!isPaused && !payslip) {
        CP_Settings_StrokeWeight(4.0f);
        CP_Settings_RectMode(CP_POSITION_CENTER);
        CP_Settings_ImageMode(CP_POSITION_CORNER);
        CP_Image_Draw(background_image, 0.f, 0.f, (float)CP_System_GetWindowWidth(), (float)CP_System_GetWindowHeight(), 255);

        float dt = CP_System_GetDt();
        CP_Font_Set(myFont);
        float w = (float)CP_System_GetWindowWidth();
        float h = (float)CP_System_GetWindowHeight();

        // --- TUTORIAL WINDOW ---
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
            CP_Font_DrawText("- Type letters, numbers, AND SYMBOLS!", text_center_x, text_start_y + line_h * 6);
            CP_Font_DrawText("- WATCH OUT: Slipping on bananas spills your coffee!", text_center_x, text_start_y + line_h * 7);

            CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
            CP_Settings_Fill(CP_Color_Create(0, 180, 0, 255));
            CP_Font_DrawText("GOAL: Finish projects fast! No sleep allowed!", w / 2.0f, h / 2.0f + box_h / 2.0f - 180.0f);

            CP_Settings_Fill(CP_Color_Create(200, 0, 0, 255));
            CP_Font_DrawText("WARNING: Falling asleep 3 times = FIRED!", w / 2.0f, h / 2.0f + box_h / 2.0f - 140.0f);

            float btn_w = 300.0f, btn_h = 70.0f;
            float btn_x = w / 2.0f;
            float btn_y = h / 2.0f + box_h / 2.0f - 80.0f;
            float mx = CP_Input_GetMouseX(), my = CP_Input_GetMouseY();

            if (mx > btn_x - btn_w / 2.0f && mx < btn_x + btn_w / 2.0f && my > btn_y - btn_h / 2.0f && my < btn_y + btn_h / 2.0f) {
                CP_Settings_Fill(CP_Color_Create(0, 200, 0, 255));
                if (CP_Input_MouseClicked()) is_tutorial_active = 0;
            }
            else CP_Settings_Fill(CP_Color_Create(0, 120, 0, 255));

            CP_Settings_Stroke(CP_Color_Create(0, 0, 0, 255));
            CP_Graphics_DrawRect(btn_x, btn_y, btn_w, btn_h);
            CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
            CP_Settings_TextSize(40.0f);
            CP_Font_DrawText("START SHIFT", btn_x, btn_y);
            return;
        }

        // --- CHECK GAME OVER STATUS ---
        if (game_over) {
            if (game_over_reason == 2) { //success
                CP_Settings_Fill(CP_Color_Create(0, 150, 0, 255));
                CP_Graphics_DrawRect(w / 2.0f, h / 2.0f, w, h);
                CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
                CP_Settings_TextSize(80.0f);
                CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
                CP_Font_DrawText("STAGE 2 COMPLETE!", w / 2.0f, h / 2.0f - 80.0f);
                CP_Settings_TextSize(40.0f);
                CP_Font_DrawText("Excellent work under pressure.", w / 2.0f, h / 2.0f);
                char end_score[50]; sprintf_s(end_score, 50, "Final Score: %d", Score_Get());
                CP_Font_DrawText(end_score, w / 2.0f, h / 2.0f + 40.0f);

                float btn_w = 250.0f, btn_h = 60.0f;
                float btn_x = w / 2.0f - btn_w / 2.0f;
                float btn_y = h / 2.0f + 100.0f;
                float mx = CP_Input_GetMouseX(), my = CP_Input_GetMouseY();

                if (mx > btn_x && mx < btn_x + btn_w && my > btn_y && my < btn_y + btn_h) {
                    CP_Settings_Fill(CP_Color_Create(50, 200, 50, 255));
                    if (CP_Input_MouseClicked()) payslip = 1; //Link to total score, total amount earned page
                }
                else CP_Settings_Fill(CP_Color_Create(0, 100, 0, 255));

                CP_Settings_Stroke(CP_Color_Create(255, 255, 255, 255));
                CP_Graphics_DrawRect(btn_x + btn_w / 2.0f, btn_y + btn_h / 2.0f, btn_w, btn_h);
                CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
                CP_Settings_TextSize(30.0f);
                CP_Font_DrawText("RESULTS", w / 2.0f, btn_y + btn_h / 2.0f);
                
                // --- MENU BUTTON ---
                float menu_btn_y = btn_y + 80.0f;

                if (mx > btn_x && mx < btn_x + btn_w && my > menu_btn_y && my < menu_btn_y + btn_h) {
                    CP_Settings_Fill(CP_Color_Create(200, 50, 50, 255));
                    if (CP_Input_MouseClicked()) CP_Engine_SetNextGameState(Engineer_Levels_Init, Engineer_Levels_Update, Engineer_Levels_Exit);
                }
                else {
                    CP_Settings_Fill(CP_Color_Create(150, 0, 0, 255));
                }

                CP_Settings_Stroke(CP_Color_Create(255, 255, 255, 255));
                CP_Graphics_DrawRect(btn_x + btn_w / 2.0f, menu_btn_y + btn_h / 2.0f, btn_w, btn_h);

                CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
                CP_Settings_TextSize(30.0f);
                CP_Font_DrawText("MENU", w / 2.0f, menu_btn_y + btn_h / 2.0f);
            }
            else { //Fired
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
                else CP_Settings_Fill(CP_Color_Create(100, 100, 100, 255));

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
                Score_LevelEnd();
                game_timer = 0.0f;
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
            // Player Update
            current_energy -= ENERGY_DECAY_RATE * dt;
            if (current_energy <= 0.0f) {
                current_energy = 0.0f;
                sleep_count++;
                is_laptop_open = 0;
                if (sleep_count >= MAX_SLEEPS_ALLOWED) {
                    Score_LevelEnd();
                    game_over = 1;
                    game_over_reason = 1;
                }
                else {
                    is_sleeping = 1;
                    sleep_timer = SLEEP_DURATION;
                }
            }

            // --- BANANA LOGIC ---
            spawn_timer -= dt;
            if (spawn_timer <= 0.0f) {
                TrySpawnOneBanana();
                spawn_timer = BANANA_SPAWN_RATE;
            }

            for (int i = 0; i < MAX_BANANAS; i++) {
                if (bananas[i].active) {
                    float dx = player_x - bananas[i].x;
                    float dy = player_y - bananas[i].y;

                    if (sqrt(dx * dx + dy * dy) < 50.0f) {
                        if (holding_mug == 1) {
                            mug_coffee_current = 0.0f;
                            holding_mug = 0;
                        }
                        current_energy -= BANANA_DAMAGE;
                        if (current_energy < 0) current_energy = 0;

                        bananas[i].active = 0;

                        woops_timer = 1.0f;
                        woops_x = player_x + 20.0f;
                        woops_y = player_y - 50.0f;
                    }

                    bananas[i].timer -= dt;
                    if (bananas[i].timer <= 0.0f) {
                        bananas[i].active = 0;
                    }
                }
            }

            if (woops_timer > 0) woops_timer -= dt;

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
        } // <--- THIS WAS THE MISSING CLOSING BRACE

        // --- DEBUGGING PURPOSES ---
        if (DEBUG_HITBOXES) {
            CP_Settings_NoFill();
            CP_Settings_Stroke(CP_Color_Create(255, 0, 0, 255));
            CP_Settings_StrokeWeight(2.0f);
            for (int i = 0; i < wall_count; i++) CP_Graphics_DrawRect(walls[i].x + walls[i].w / 2.0f, walls[i].y + walls[i].h / 2.0f, walls[i].w, walls[i].h);
            CP_Settings_Stroke(CP_Color_Create(0, 255, 0, 255));
            CP_Graphics_DrawRect(player_x, player_y + PLAYER_OFFSET_Y, PLAYER_HITBOX_W, PLAYER_HITBOX_H);
        }

        CP_Settings_ImageMode(CP_POSITION_CENTER);
        CP_Image_Draw(laptop_image, laptop_x, laptop_y, 150.f, 150.f, 255);
        CP_Image_Draw(coffee_machine_image, machine_x, machine_y, machine_w, machine_h, 255);
        CP_Image_Draw(player_image, player_x, player_y, player_width, player_height, 255);

        for (int i = 0; i < MAX_BANANAS; i++) {
            if (bananas[i].active) {
                CP_Image_Draw(banana_image, bananas[i].x, bananas[i].y, BANANA_SIZE, BANANA_SIZE, 255);
            }
        }

        CP_Image_Draw(mug_image, mug_x, mug_y, 100.f, 100.f, 255);

        if (woops_timer > 0.0f) {
            CP_Settings_Fill(CP_Color_Create(255, 0, 0, 255));
            CP_Settings_TextSize(30.0f);
            CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_BOTTOM);
            CP_Font_DrawText("Woops!", woops_x, woops_y);
        }

        // --- UI BARS ---
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
        int active_circles = 0;
        for (int i = 0; i < current_level_circles; i++) if (game_circles[i].is_active) active_circles++;
        int percent = 0;
        if (current_level_circles > 0) percent = (int)(((float)(current_level_circles - active_circles) / (float)current_level_circles) * 100.0f);
        CP_Settings_TextSize(50.0f);
        CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_TOP);
        char proj_buffer[50]; sprintf_s(proj_buffer, 50, "Project: %d%%", percent);
        CP_Font_DrawText(proj_buffer, 20.0f, 20.0f);

        int minutes = (int)game_timer / 60;
        int seconds = (int)game_timer % 60;
        char timer_buffer[50]; sprintf_s(timer_buffer, 50, "%02d:%02d", minutes, seconds);
        if (game_timer < 30.0f) CP_Settings_Fill(CP_Color_Create(255, 50, 50, 255));
        else CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
        CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_TOP);
        CP_Font_DrawText(timer_buffer, (float)CP_System_GetWindowWidth() * 0.25f, 20.0f);

        CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
        char score_buffer[50]; sprintf_s(score_buffer, 50, "Score: %d", Score_Get());
        CP_Font_DrawText(score_buffer, (float)CP_System_GetWindowWidth() / 2.0f, 20.0f);

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

            if (active_circles == 0) game_completed = 1;
            else game_completed = 0;

            if (game_completed == 0) {
                int shift_held = CP_Input_KeyDown(KEY_LEFT_SHIFT) || CP_Input_KeyDown(KEY_RIGHT_SHIFT);

                for (int i = 0; i < current_level_circles; i++) {
                    if (game_circles[i].is_active) {
                        int id = game_circles[i].id;
                        CP_KEY target_key = GetKeyForID(id);
                        int needs_shift = RequiresShift(id);

                        if (CP_Input_KeyTriggered(target_key)) {
                            if (needs_shift && shift_held) {
                                game_circles[i].is_active = 0;
                            }
                            else if (!needs_shift && !shift_held) {
                                game_circles[i].is_active = 0;
                            }
                        }
                    }
                }

                for (int i = 0; i < current_level_circles; i++) {
                    if (game_circles[i].is_active) {
                        CP_Settings_Fill(CP_Color_Create(255, 105, 180, 255));
                        CP_Settings_Stroke(CP_Color_Create(255, 255, 255, 255));
                        CP_Settings_StrokeWeight(2.0f);
                        CP_Graphics_DrawCircle(game_circles[i].x, game_circles[i].y, CIRCLE_RADIUS * 2.0f);

                        CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
                        CP_Settings_TextSize(30.0f);
                        CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

                        char buffer[5];
                        GetCharForID(game_circles[i].id, buffer);
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
            if (fill_w > 0) CP_Graphics_DrawRect(bar_x + fill_w / 2.0f, bar_y + bar_h / 2.0f, fill_w, bar_h);
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

void Coffee_Driven2_Exit(void) {
    CP_Image_Free(background_image);
    CP_Image_Free(player_image);
    CP_Image_Free(laptop_image);
    CP_Image_Free(coffee_machine_image);
    CP_Image_Free(mug_image);
    CP_Image_Free(banana_image);
    CP_Image_Free(pause_image);
    CP_Font_Free(myFont);
    CP_Graphics_ClearBackground(CP_Color_Create(245, 222, 179, 255));
    Payslip_Exit();
}