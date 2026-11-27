// -----------------------------------------------------------------------------
// file:    bughunt1.c
// author:  Yi Ting
// email:   yiting.tin@digipen.edu
//
// brief:   function definitions for Bug Hunt Level 1 gameplay
//          handles initialization, gameplay loop, UI updates, timers,
//          delayed transitions, server checks, bug logic, and level completion
// -----------------------------------------------------------------------------


#include "cprocessing.h"
#include "engineerlevels.h"
#include "bughunt1.h"
#include "bughunt2.h"
#include "score.h"
#include "bugs.h"
#include "servers.h"
#include "bugs_console.h"
#include "bugs_ui.h"
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

// initialize level 1
void Bughunt1_Init(void)
{
    g_winW = CP_System_GetWindowWidth();
    g_winH = CP_System_GetWindowHeight();

    // initialise ui and console
    UI_Init();
    UI_Level1();
    Console_Init();
    Console_Clear();

    // initialize gameplay systems
    Servers_Init();
    Bugs_Init_1(); // level 1 bugs

    // use level index 5 for level 1
    Score_LevelStart(5);

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
        g_currentTab = 1; // show gameplay tab
    }
    
    AddStatusMsg("[MESSAGE] Level 1 Initialized", LOG_MESSAGE);
}

void Bughunt1_Update(void)
{
    // handle delayed transitions triggered from game over panel
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
                // retry level 1
                CP_Engine_SetNextGameStateForced(
                    Bughunt1_Init, Bughunt1_Update, Bughunt1_Exit);
            }
            else if (pendingAction == 3)
            {
                // proceed level 2
                CP_Engine_SetNextGameState(
                    Bughunt2_Init, Bughunt2_Update, Bughunt2_Exit);
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

    // gameplay only runs while on level 1 tab and not game over
    if (!g_gameOver && g_currentTab == 1)
    {
        g_timer -= CP_System_GetDt();
        if (g_timer <= 0.0f)
        {
            g_timer = 0.0f;
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
            UpdateBugs_1();
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
        Score_LevelEnd(); // update high score for level 1
        DrawGameOver();
    }
}

void Bughunt1_Exit(void)
{
    Bugs_Exit();
    Bugs_Ui_Exit();
    Bugs_Console_Exit();
}
