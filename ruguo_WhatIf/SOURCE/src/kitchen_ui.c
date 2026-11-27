//---------------------------------------------------------
// file:	kitchen_ui.c
// author:	Shirley
// email:	shirleywenhui.l@digipen.edu 
//
// brief:	Dishwasher Level 3 & 4 Game UI
//
//---------------------------------------------------------
#include "kitchen_ui.h"
#include "score.h"
#include "kitchen.h"
#include "kitchen2.h"
#include "cprocessing.h"
#include "dishwasherlevels.h"
#include "utils.h"
#include "payslip.h"
#include <stdio.h>
#include <stdbool.h>

#define img_width 250.f
#define img_height 250.f

static CP_Font myFont;
static float g_winW, g_winH;

static CP_Image Stove_Image;
static CP_Image Table_Image;
static CP_Image Sink_Image;
static CP_Image Shelf_Image;
static CP_Image Tray_Image;
static CP_Image Rack_Image;
static CP_Image Trash_Image;
static CP_Image Knife_Image;
static CP_Image Spoon_Image;
static CP_Image Fork_Image;
static CP_Image Plate_Image;
static CP_Image Floor_Image;
static CP_Image WoodenTray_Image;
static CP_Image Fridge_Image;
static CP_Image TrashBag_Image;
static CP_Image PlatesShelf_Image;
static CP_Image GamePanel_Image;
static CP_Image Window_Image;
static CP_Image Tap_Image;
static CP_Image Clock_Image;
static CP_Image Oven_Image;
static CP_Image lamp_Image;
static CP_Image Pancake_Image;
static CP_Image Cross_Image;
static CP_Image BrownCross_Image;
static CP_Image Water_Image;

static CP_Image Chair1_Image;
static CP_Image Chair2_Image;
static CP_Image Chair3_Image;
static CP_Image Chair4_Image;

static CP_Image Plant_Image;
static CP_Image PlantShelf_Image;
static CP_Image KnifeBoard_Image;
static CP_Image UpperShelf_Image;

static CP_Color white;
static CP_Color dark_brown;
static CP_Color brown;
static CP_Color dim_bg;

static int payslip = 0;

typedef struct {
    float x, y, w, h;
} Rect;

Rect cross;

void Kitchen_Ui_Init(void)
{
    //load image and font
    myFont = CP_Font_Load("Assets/Pixellari.ttf");
    g_winW = (float) CP_System_GetWindowWidth();
    g_winH = (float) CP_System_GetWindowHeight();

    Stove_Image = CP_Image_Load("Assets/stove.png");
    Table_Image = CP_Image_Load("Assets/table.png");
    Sink_Image = CP_Image_Load("Assets/sink.png");
    Shelf_Image = CP_Image_Load("Assets/shelf.png");
    Tray_Image = CP_Image_Load("Assets/tray.png");
    Rack_Image = CP_Image_Load("Assets/knife_rack.png");
    Trash_Image = CP_Image_Load("Assets/trash.png");
    Knife_Image = CP_Image_Load("Assets/knife.png");
    Spoon_Image = CP_Image_Load("Assets/spoon.png");
    Fork_Image = CP_Image_Load("Assets/fork.png");
    Plate_Image = CP_Image_Load("Assets/plate_white.png");
    Floor_Image = CP_Image_Load("Assets/floor.png");
    WoodenTray_Image = CP_Image_Load("Assets/wooden_tray.png");
    Fridge_Image = CP_Image_Load("Assets/fridge.png");
    TrashBag_Image = CP_Image_Load("Assets/trashbag.png");
    PlatesShelf_Image = CP_Image_Load("Assets/plates_shelf.png");
    GamePanel_Image = CP_Image_Load("Assets/gameover_panel_k.png");
    Window_Image = CP_Image_Load("Assets/window.png");
    Tap_Image = CP_Image_Load("Assets/tap.png");
    Clock_Image = CP_Image_Load("Assets/clock.png");
    Oven_Image = CP_Image_Load("Assets/oven.png");
    lamp_Image = CP_Image_Load("Assets/lamp.png");
    Pancake_Image = CP_Image_Load("Assets/pancake.png");

    Cross_Image = CP_Image_Load("Assets/cross.png");
    BrownCross_Image = CP_Image_Load("Assets/brown_cross.png");
    Water_Image = CP_Image_Load("Assets/water.png");

    Chair1_Image = CP_Image_Load("Assets/chair_1.png");
    Chair2_Image = CP_Image_Load("Assets/chair_2.png");
    Chair3_Image = CP_Image_Load("Assets/chair_3.png");
    Chair4_Image = CP_Image_Load("Assets/chair_4.png");

    Plant_Image = CP_Image_Load("Assets/plant.png");
    PlantShelf_Image = CP_Image_Load("Assets/plant_shelf.png");
    KnifeBoard_Image = CP_Image_Load("Assets/knife_board.png");
    UpperShelf_Image = CP_Image_Load("Assets/uppershelf.png");

    //set colors
    white = CP_Color_Create(255,255,255,250);
    dark_brown = CP_Color_Create(80, 49, 37, 255);
    brown = CP_Color_Create(143, 102, 87, 255);
    dim_bg = CP_Color_Create(0, 0, 0, 180);

    //how to play cross btn
    cross.x = 1190.f;
    cross.y = 165.f;
    cross.w = 40.f;
    cross.h = 40.f;
}

void Kitchen_Ui(void)
{
    //Kitchen chaos ui
    CP_Settings_AntiAlias(FALSE);
    CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_NEAREST);
    float centerW = g_winW / 2;
    float centerH = g_winH / 2;


    float half_width = img_width / 2;
    float bottomX = g_winW / 8;
    float bottomY = 7 * (g_winH / 8);
    float wallY = 385;
    CP_Image_Draw(Floor_Image, centerW, centerH, g_winW, g_winH, 255);
    CP_Image_Draw(lamp_Image, centerW - (half_width * 4) - 70, wallY + 20, img_width, img_height, 255);
    CP_Image_Draw(Fridge_Image, centerW - (img_width * 2) + 34, wallY - 35, img_width, img_height, 255);
    CP_Image_Draw(Shelf_Image, centerW - (half_width * 3) + 30, wallY, img_width, img_height, 255);
    CP_Image_Draw(Stove_Image, centerW - img_width + 20, wallY, img_width, img_height, 255);
    CP_Image_Draw(Shelf_Image, centerW - half_width + 10, wallY, img_width, img_height, 255);
    CP_Image_Draw(Sink_Image, centerW, wallY, img_width, img_height, 255);
    CP_Image_Draw(Shelf_Image, centerW + half_width - 10, wallY, img_width, img_height, 255);
    CP_Image_Draw(Shelf_Image, centerW + img_width - 20, wallY, img_width, img_height, 255);
    CP_Image_Draw(Shelf_Image, centerW + (half_width * 3) - 30, wallY, img_width, img_height, 255);

    CP_Image_Draw(PlantShelf_Image, centerW - (half_width * 2) - 40, wallY - 160, img_width, img_height, 255);
    CP_Image_Draw(Plant_Image, centerW - (half_width * 2) - 40, wallY - 170, img_width, img_height, 255);
    CP_Image_Draw(UpperShelf_Image, centerW + half_width - 10, wallY - 220, img_width, img_height, 255);
    CP_Image_Draw(UpperShelf_Image, centerW + img_width - 20, wallY - 220, img_width, img_height, 255);
    CP_Image_Draw(PlatesShelf_Image, centerW + (half_width * 3) - 30, wallY - 220, img_width, img_height, 255);
    
    CP_Image_Draw(Clock_Image, centerW + (half_width * 4) - 40, wallY - 220, img_width, img_height, 255);

    CP_Image_Draw(Trash_Image, centerW + (half_width * 4) - 60, wallY + 20, 100, 100, 255);
    CP_Image_Draw(TrashBag_Image, centerW + (half_width * 4) - 30, wallY + 40, 100, 100, 255);

    CP_Image_Draw(Rack_Image, centerW + half_width - 10, wallY - 100, half_width, half_width, 255);

    CP_Image_Draw(WoodenTray_Image, bottomX + 110, bottomY - 50, 200, 200, 255);

    CP_Image_Draw(Chair4_Image, centerW + 50, centerH + 130, img_width, img_height, 255);
    CP_Image_Draw(Chair4_Image, centerW - 50, centerH + 130, img_width, img_height, 255);
    CP_Image_Draw(Chair3_Image, centerW - half_width, centerH + 200, img_width, img_height, 255);
    CP_Image_Draw(Chair2_Image, centerW + half_width, centerH + 200, img_width, img_height, 255);
    CP_Image_Draw(Table_Image, centerW, centerH + 200, img_width, img_height, 255);
    CP_Image_Draw(Chair1_Image, centerW + 50, centerH + img_height, img_width, img_height, 255);
    CP_Image_Draw(Chair1_Image, centerW - 50, centerH + img_height, img_width, img_height, 255);

    CP_Image_Draw(Tray_Image, centerW - (half_width * 3) + 30, wallY - 50, 150, 150, 255);
    CP_Image_Draw(Plate_Image, centerW + (half_width * 3) - 30, wallY - 20, 75, 75, 255);

    CP_Image_Draw(Window_Image, centerW - half_width + 10, wallY - 200, img_width, img_height, 255);
    
    CP_Image_Draw(Pancake_Image, centerW * 2 - half_width, wallY - 100, img_width, img_height, 255);
    
    CP_Image_Draw(Oven_Image, centerW - half_width + 10, wallY - 50, img_width, img_height, 255);
    CP_Image_Draw(KnifeBoard_Image, centerW + half_width - 10, wallY - 25, 350, 350, 255);

}

void Game_Over_ui(int level) {
    if (payslip) {
        Payslip_Init();
        int continue_game = Payslip_Update(1);
        if (continue_game == 1) {
            payslip = 0;
        }
    }

    if (!payslip) {
        //game over panel
        float panelW = 1000.f;
        float panelH = 1000.f;
        float panelX = g_winW / 2;
        float panelY = g_winH / 2;

        float mouseX = CP_Input_GetMouseX();
        float mouseY = CP_Input_GetMouseY();

        CP_Settings_Fill(dim_bg);
        CP_Graphics_DrawRect(panelX, panelY, g_winW, g_winH);

        CP_Image_Draw(GamePanel_Image, panelX, panelY, panelW, panelH, 255);

        CP_Font_Set(myFont);
        CP_Settings_TextSize(90.0f);
        CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

        CP_Settings_Fill(CP_Color_Create(143, 102, 87, 255));
        CP_Font_DrawText("GAME OVER", panelX + 3, panelY - 117);

        CP_Settings_Fill(CP_Color_Create(114, 81, 69, 255));
        CP_Font_DrawText("GAME OVER", panelX, panelY - 120);


        CP_Settings_TextSize(40.0f);
        CP_Settings_Fill(brown);

        char scoreText[64];
        sprintf_s(scoreText, sizeof(scoreText), "Final Score: %d", Score_Final());
        CP_Font_DrawText(scoreText, panelX, panelY - 30);

        char scoreText_H[64];
        sprintf_s(scoreText_H, sizeof(scoreText_H), "Highest Score: %d", Score_GetHigh(level));
        CP_Font_DrawText(scoreText_H, panelX, panelY + 10);

        CP_Settings_TextSize(36.0f);
        //Home
        if (IsAreaHovered(panelX - 150, panelY + 110, 120.f, 40.f, mouseX, mouseY)) {
            CP_Settings_Fill(dark_brown);
        }
        else {
            CP_Settings_Fill(brown);

        }
        CP_Font_DrawText(">Home", panelX - 150, panelY + 110);

        //Retry
        if (IsAreaHovered(panelX, panelY + 110, 120.f, 40.f, mouseX, mouseY)) {
            CP_Settings_Fill(dark_brown);
        }
        else {
            CP_Settings_Fill(brown);

        }
        CP_Font_DrawText(">Retry", panelX, panelY + 110);

        //Next
        if (level == 3) {
            if (IsAreaHovered(panelX + 150, panelY + 110, 120.f, 40.f, mouseX, mouseY)) {
                CP_Settings_Fill(dark_brown);
            }
            else {
                CP_Settings_Fill(brown);

            }
            CP_Font_DrawText(">Next", panelX + 150, panelY + 110);

            if (IsAreaClicked(panelX + 150, panelY + 110, 120.f, 40.f, mouseX, mouseY)) {
                if (CP_Input_MouseClicked()) {
                    CP_Engine_SetNextGameStateForced(Kitchen2_Init, Kitchen2_Update, Kitchen2_Exit);
                }
            }

        }

        //Home
        if (IsAreaClicked(panelX - 150, panelY + 110, 120.f, 40.f, mouseX, mouseY)) {
            if (CP_Input_MouseClicked()) {
                CP_Engine_SetNextGameStateForced(Dishwasher_Levels_Init, Dishwasher_Levels_Update, Dishwasher_Levels_Exit);
            }
        }

        //Restart
        if (IsAreaClicked(panelX, panelY + 110, 120.f, 40.f, mouseX, mouseY)) {
            if (CP_Input_MouseClicked()) {
                if (level == 3) {
                    Kitchen_Ui_Exit();
                    CP_Engine_SetNextGameStateForced(Kitchen_Init, Kitchen_Update, Kitchen_Exit);
                }
                else {
                    Kitchen_Ui_Exit();
                    CP_Engine_SetNextGameStateForced(Kitchen2_Init, Kitchen2_Update, Kitchen2_Exit);
                }
            }
        }

        //Result
        if (level == 4) {
            if (IsAreaHovered(panelX + 150, panelY + 110, 120.f, 40.f, mouseX, mouseY)) {
                CP_Settings_Fill(dark_brown);
            }
            else {
                CP_Settings_Fill(brown);

            }
            CP_Font_DrawText(">Result", panelX + 150, panelY + 110);

            if (IsAreaClicked(panelX + 150, panelY + 110, 120.f, 40.f, mouseX, mouseY)) {
                if (CP_Input_MouseClicked()) {
                    payslip = 1;
                }
            }
        }
    }

}

//how to play instruction
bool Game_Menu_ui(int level) {
    float panelW = 1000;
    float panelH = 1000;
    float panelX = g_winW / 2;
    float panelY = g_winH / 2;

    float image_width = 150;

    float mouseX = CP_Input_GetMouseX();
    float mouseY = CP_Input_GetMouseY();

    int incY_H = level == 4 ? 400 : 0;
    int incY = level == 4 ? 70 : 0;
    int incY_C = level == 4 ? 85 : 0;

    CP_Settings_Fill(dim_bg);
    CP_Graphics_DrawRect(panelX, panelY, g_winW, g_winH);

    CP_Image_Draw(GamePanel_Image, panelX, panelY, panelW + 400, panelH + 400 + incY_H, 255);

    CP_Font_Set(myFont);
    CP_Settings_TextSize(90.0f);
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

    CP_Settings_Fill(CP_Color_Create(143, 102, 87, 255));
    CP_Font_DrawText("How to Play", panelX + 3, panelY - 197 - incY);

    CP_Settings_Fill(CP_Color_Create(114, 81, 69, 255));
    CP_Font_DrawText("How to Play", panelX, panelY - 200 - incY);

    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_MIDDLE);
    CP_Settings_TextSize(40.0f);
    CP_Settings_Fill(brown);

    CP_Font_DrawTextBox("- Pick up the item from the tray\n- Wash the item in the sink\n- Place it in the designated area\n- WASD KEY for player movement\n- SPACE KEY for pick/drop of item\nDesignated Area:", panelX - 350, panelY - 110 -incY,800);
    
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
    CP_Font_DrawText("Cutlery       Knife       Plate", panelX, panelY + 230 - incY);

    CP_Image_Draw(Tray_Image, panelX - 180, panelY + 170 - incY, image_width, image_width, 255);
    CP_Image_Draw(Rack_Image, panelX + 20, panelY + 170 - incY, image_width, image_width, 255);
    CP_Image_Draw(Shelf_Image, panelX + 200, panelY + 162 - incY, image_width, image_width, 255);
    CP_Image_Draw(Plate_Image, panelX + 200, panelY + 150 - incY, 50, 50, 255);

    if (level == 4) {
        CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_MIDDLE);
        CP_Settings_Fill(brown);
        CP_Font_DrawTextBox("- ENTER KEY to mop the floor\nLook Out For:", panelX - 350, panelY + 270 - incY, 800);

        CP_Image_Draw(Water_Image, panelX - 70, panelY + 320 - incY, 50, 50, 255);
    }

    // Exit
    if (IsAreaHovered(cross.x, cross.y - incY_C, cross.w, cross.h, mouseX, mouseY)) {
        CP_Image_Draw(BrownCross_Image, cross.x, cross.y - incY_C, cross.w, cross.h, 255);
    }
    else {
        CP_Image_Draw(Cross_Image, cross.x, cross.y - incY_C, cross.w, cross.h, 255);

    }
    
    if (IsAreaClicked(cross.x, cross.y - incY_C, cross.w, cross.h, mouseX, mouseY)) {
        if (CP_Input_MouseClicked()) {
            return false;
        }
    }
    return true;
}

void Kitchen_Ui_Exit(void)
{
    CP_Image_Free(Stove_Image);
    CP_Image_Free(Table_Image);
    CP_Image_Free(Sink_Image);
    CP_Image_Free(Shelf_Image);
    CP_Image_Free(Tray_Image);
    CP_Image_Free(Rack_Image);
    CP_Image_Free(Trash_Image);
    CP_Image_Free(Knife_Image);
    CP_Image_Free(Spoon_Image);
    CP_Image_Free(Fork_Image);
    CP_Image_Free(Plate_Image);
    CP_Image_Free(Floor_Image);
    CP_Image_Free(WoodenTray_Image);
    CP_Image_Free(Fridge_Image);
    CP_Image_Free(TrashBag_Image);
    CP_Image_Free(PlatesShelf_Image);
    CP_Image_Free(GamePanel_Image);
    CP_Image_Free(Window_Image);
    CP_Image_Free(Tap_Image);
    CP_Image_Free(Clock_Image);
    CP_Image_Free(Oven_Image);
    CP_Image_Free(lamp_Image);
    CP_Image_Free(Pancake_Image);
    CP_Image_Free(Cross_Image);
    CP_Image_Free(BrownCross_Image);
    CP_Image_Free(Water_Image);
    CP_Image_Free(Chair1_Image);
    CP_Image_Free(Chair2_Image);
    CP_Image_Free(Chair3_Image);
    CP_Image_Free(Chair4_Image);
    CP_Image_Free(Plant_Image);
    CP_Image_Free(PlatesShelf_Image);
    CP_Image_Free(KnifeBoard_Image);
    CP_Image_Free(UpperShelf_Image);

    CP_Font_Free(myFont);
    CP_Graphics_ClearBackground(dim_bg);

    Payslip_Exit();
}
