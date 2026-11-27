// ------------------------------------------------------------------------------
// file:    bugs_console.c
// author:  Yi Ting
// email:   yiting.tin@digipen.edu
//
// brief:   function definitions for the console message system in Bug Hunt games
//          handles logging and rendering of messages, including errors,
//          warnings, status updates, and bottom-panel debug output
// -------------------------------------------------------------------------------

#include "bugs_console.h"
#include "cprocessing.h"
#include "score.h"
#include <string.h>
#include <stdio.h>

// maximum number of messages shown
#define maxMsg 4

// structure for a log message
typedef struct {
    char text[128]; // message text
    CP_Color color; // message display color
} LogMsg;

// list of active log messages
static LogMsg g_msg[maxMsg]; 

// number of messages stored
static int g_msgCount = 0;

static CP_Font font;
static float g_winW, g_winH;

// console init
// loads font and gets window size
void Console_Init(void)
{
    font = CP_Font_Load("Assets/Pixellari.ttf");
    g_winW = (float)CP_System_GetWindowWidth();
    g_winH = (float)CP_System_GetWindowHeight();
}

// add status message
// adds a message to the console with a color assigned by severity type
void AddStatusMsg(const char* msg, LogType type)
{
    CP_Color color = CP_Color_Create(255, 255, 255, 255);

    // choose color based on message type
    switch (type)
    {
    case LOG_ERROR:
        color = CP_Color_Create(255, 80, 80, 255); // red for errors
        break;

    case LOG_WARNING:
        color = CP_Color_Create(255, 220, 100, 255); // yellow for warnings
        break;

    case LOG_MESSAGE:
    default:
        color = CP_Color_Create(150, 255, 150, 255); // green for normal messages
        break;
    }

    // if message list is full, shift messages up
    if (g_msgCount >= maxMsg)
    {
        for (int i = 1; i < maxMsg; i++)
        {
            g_msg[i - 1] = g_msg[i];  // move message up by 1 slot
        }
        g_msgCount = maxMsg - 1;
    }

	// insert new message text into available slot
    int i = 0;
    while (msg[i] != '\0' && i < sizeof(g_msg[g_msgCount].text) - 1)
    {
        g_msg[g_msgCount].text[i] = msg[i];
        i++;
    }

    // null terminate the message string
	g_msg[g_msgCount].text[i] = '\0';

    // store the display color
    g_msg[g_msgCount].color = color;

    // increase store message count
    g_msgCount++;
}

// draw status message panel
void DrawStatusMsg(void)
{
    CP_Settings_NoStroke();
    CP_Settings_StrokeWeight(0);

    // height of console area
    float panelH = 180.f;

    // y offset for message list
    float startY = g_winH - panelH + 70;

    // background panel
    CP_Settings_RectMode(CP_POSITION_CENTER);
    CP_Settings_Fill(CP_Color_Create(25, 25, 25, 255));
    CP_Graphics_DrawRect(g_winW / 2.0f, g_winH - panelH / 2.0f, (float)g_winW, panelH);

    // top bar
    CP_Settings_Fill(CP_Color_Create(45, 45, 45, 255));
    CP_Graphics_DrawRect(g_winW / 2.0f, g_winH - panelH + 20, (float)g_winW, 45);

    // title
    CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
    CP_Font_Set(font);
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_TOP);
    CP_Settings_TextSize(25);
    CP_Font_DrawText("Error List", 20, g_winH - panelH + 8);

	// score and streak
    char scoreBuf[128];
    sprintf_s(scoreBuf, sizeof(scoreBuf),
        "Score: %d     Streak: %d",
        Score_Get(),Score_GetStreak());

    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_RIGHT, CP_TEXT_ALIGN_V_TOP);
    CP_Settings_TextSize(25);
    CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
    CP_Font_DrawText(scoreBuf, g_winW - 20, g_winH - panelH + 8);

    // draw messages
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_TOP); // reset alignment

    // draw the stored log messages
    float y = startY;
    for (int i = 0; i < g_msgCount; i++)
    {
        CP_Settings_TextSize(22);
        CP_Settings_Fill(g_msg[i].color);
        CP_Font_DrawText(g_msg[i].text, 25, y);

        // move down for next message
        y += 24;
    }
}

// clears the console by resetting the message count
void Console_Clear(void) 
{
	g_msgCount = 0;
}

void Bugs_Console_Exit(void) {
    CP_Font_Free(font);
}