// -----------------------------------------------------------------------------
// file:    bughunt2.c
// author:  Yi Ting
// email:   yiting.tin@digipen.edu
//
// brief:   function definitions for Bug Hunt Level 2 gameplay
//          handles initialization, gameplay loop, UI updates, timers,
//          delayed transitions, server checks, bug logic, and level completion
// -----------------------------------------------------------------------------

#include "cprocessing.h"
#include "bughunt2.h"
#include "servers.h"
#include "bugs.h"
#include "bugs_ui.h"
#include "bugs_console.h"
#include "score.h"
#include "engineerlevels.h"
#include "coffeedriven1.h"
#include "audio.h"
#include <stdio.h>

#define game_time 30.0f

static float g_timer = game_time;
static int g_gameOver = 0;
static int g_winW, g_winH;

// animation delayed action
extern int pendingAction;
extern float actionTimer;

// from bugs_ui.c
extern int g_seenHTP;
extern int g_showHTP;
extern int g_currentTab; // 1 = level, 2 = howtoplay, 3 = pause

// initialize level 2
void Bughunt2_Init(void)
{
    g_winW = CP_System_GetWindowWidth();
    g_winH = CP_System_GetWindowHeight();

    // initialise ui and console
    UI_Init();
    UI_Level2();
    Console_Init();
    Console_Clear();

    // initialize gameplay systems
    Servers_Init();
    Bugs_Init_2(); // level 2 bugs

    // use level index 6 for level 2
    Score_LevelStart(6);

    // reset level state
    pendingAction = 0;
    g_timer = game_time;
    g_gameOver = 0;

    // show howtoplay on first entry
    if (!g_seenHTP)
    {
        g_showHTP = 1;
        g_seenHTP = 1;
        g_currentTab = 2; // show HowToPlay.c tab
    }
    else
    {
        g_showHTP = 0;
        g_currentTab = 1;
    }

    AddStatusMsg("[MESSAGE] Level 2 Initialized", LOG_MESSAGE);
}

void Bughunt2_Update(void)
{
    // handle delayed transitions triggered from game-over panel
    if (pendingAction != 0)
    {
        actionTimer -= CP_System_GetDt();
        if (actionTimer <= 0.0f)
        {
            if (pendingAction == 1)
            {
                // home
                CP_Engine_SetNextGameState(
                    Engineer_Levels_Init, Engineer_Levels_Update, Engineer_Levels_Exit);
            }
            else if (pendingAction == 2)
            {
                // retry level 2
                CP_Engine_SetNextGameStateForced(
                    Bughunt2_Init, Bughunt2_Update, Bughunt2_Exit);
            }
            else if (pendingAction == 3)
            {
                //  proceed to coffee driven
                CP_Engine_SetNextGameState(
                    Coffee_Driven1_Init, Coffee_Driven1_Update, Coffee_Driven1_Exit);
            }

            pendingAction = 0;
            return;
        }
    }
    // esc handling: close howtoplay or toggle pause tab
    if (!g_gameOver)
    {
        if (CP_Input_KeyTriggered(KEY_ESCAPE))
        {
            if (g_currentTab == 2)
            {
                // close howtoplay and return to gameplay
                g_showHTP = 0;
                g_currentTab = 1;
            }
            else
            {
                // toggle pause menu
                TogglePause();
            }
        }
    }
    // background
    CP_Graphics_ClearBackground(CP_Color_Create(30, 30, 30, 255));

    // gameplay only runs while on level 2 tab and not game over
    if (!g_gameOver && g_currentTab == 1)
    {
        g_timer -= CP_System_GetDt();
        if (g_timer <= 0.0f)
        {
            g_timer = 0;
            g_gameOver = 1;
            AddStatusMsg("[ERROR] Time's up!", LOG_ERROR);
        }

        // check remaining servers
        int alive[NUM_SERVERS];
        if (!g_gameOver && GetAliveServers(alive) == 0)
        {
            g_gameOver = 1;
            AddStatusMsg("[ERROR] All servers destroyed!", LOG_ERROR);
        }

        // update entities if still running
        if (!g_gameOver)
        {
            UpdateServers();
            UpdateBugs_2();
        }
    }

    // draw bottom console and top ui
    DrawStatusMsg();
    DrawUI();

    // draw timer only during gameplay
    if (g_currentTab == 1)
    {
        DrawTimer(g_timer);
    }

    // game over summary panel
    if (g_gameOver)
    {
        Score_LevelEnd(); // update high score for level 2
        DrawGameOver();
    }
}

void Bughunt2_Exit(void)
{
    Bugs_Exit();
    Bugs_Ui_Exit();
    Bugs_Console_Exit();
}
