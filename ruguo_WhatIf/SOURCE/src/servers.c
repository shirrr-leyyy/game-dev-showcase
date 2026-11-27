// -----------------------------------------------------------------------------
// file:    servers.c
// author:  Yi Ting
// email:   yiting.tin@digipen.edu
//
// brief:   function definitions for the server system in Bug Hunt game
//          handles server initialization, damage, health bar rendering,
//          shake/flash effects, and crash/missed tracking.
// -----------------------------------------------------------------------------

#include "servers.h"
#include "cprocessing.h"
#include "bugs_console.h"
#include "bugs_ui.h"
#include "score.h"

#include <stdio.h>

 // each server has 3 HP
#define MAX_HEALTH 3

// images
CP_Image imgServer;
CP_Image imgBarEmpty;
CP_Image imgBarGreen;   
CP_Image imgBarYellow;
CP_Image imgBarRed;

// server array
Server g_servers[NUM_SERVERS];

// gameplay counters
int g_serversDamagedCount = 0;
int g_serversCrashedCount = 0;
int g_bugsMissed = 0;

extern int g_isLevel2;

// draw server health bar above server
static void Servers_DrawHealthBar(Server* s)
{
    float barW = 120.0f;
    float barH = 28.0f;

    CP_Image bar = imgBarEmpty;

    if (s->health == 3)      bar = imgBarGreen;
    else if (s->health == 2) bar = imgBarYellow;
    else if (s->health == 1) bar = imgBarRed;
    else                     bar = imgBarEmpty;

    CP_Image_Draw(bar, s->x, s->y - (s->h * 0.55f), barW, barH, 255);
}

// servers init
// loads sprites, positions servers evenly across the screen, and resets state
void Servers_Init(void)
{
    imgServer     = CP_Image_Load("Assets/Server.png");
    imgBarEmpty   = CP_Image_Load("Assets/bar_empty.png");
    imgBarGreen   = CP_Image_Load("Assets/bar_green.png");
    imgBarYellow  = CP_Image_Load("Assets/bar_yellow.png");
    imgBarRed     = CP_Image_Load("Assets/bar_red.png");

    int winW = CP_System_GetWindowWidth();

    // spacing between server positions
    float gap = (float)winW / (NUM_SERVERS + 1);

    float serverW = gap * 0.60f; // server width
    float serverH = 200.0f; // server height

    float y = 200.0f;

    // position each server
    for (int i = 0; i < NUM_SERVERS; i++)
    {
        float anchorX = gap * (i + 1); // even spacing

        g_servers[i].x = anchorX;
        g_servers[i].y = y;
        g_servers[i].w = serverW;
        g_servers[i].h = serverH;
        g_servers[i].id = i;
        g_servers[i].health = MAX_HEALTH;

        g_servers[i].color = CP_Color_Create(255, 255, 255, 255);

        g_servers[i].shakeTimer = 0;
        g_servers[i].shakeIntensity = 0;
        g_servers[i].shakeX = 0;
        g_servers[i].shakeY = 0;
    }

	// reset level counters
    g_serversDamagedCount = 0;
    g_serversCrashedCount = 0;
    g_bugsMissed = 0;
}

void Servers_Init_Level2(void)
{
    Servers_Init();
}

int GetAliveServers(int* outArray)
{
    int count = 0;

    for (int i = 0; i < NUM_SERVERS; i++)
    {
        if (g_servers[i].health > 0)
            outArray[count++] = i;
    }
    return count;
}

Server* GetServerByIndex(int index)
{
    if (index < 0 || index >= NUM_SERVERS) return NULL;
    return &g_servers[index];
}

// damage server
// applies damage to a server, triggers effects, penalties, and crash logic
void DamageServer(int index)
{
    if (index < 0 || index >= NUM_SERVERS) 
        return;

    Server* s = &g_servers[index];

    int hpBefore = s->health;
    int penalty = 0;

    g_bugsMissed++;

	// damage shake effect
    s->shakeTimer = 0.25f; // duration
    s->shakeIntensity = 4.0f; // shake strength

    // red flash when damaged
    s->flashTimer = 0.15f; // flash duration

    // reduce health if alive
    if (s->health > 0)
    {
        s->health--;

        if (s->health < 0)
            s->health = 0;

        g_serversDamagedCount++;

		// scoring penalty depending on health lost and level
        if (!g_isLevel2)
        {
            if (hpBefore == 3) penalty = 2;
            else if (hpBefore == 2) penalty = 4;
            else if (hpBefore == 1) penalty = 10;
        }
        else
        {
            if (hpBefore == 3) penalty = 4;
            else if (hpBefore == 2) penalty = 8;
            else if (hpBefore == 1) penalty = 20;
        }

        Score_DeductPoints(penalty);

        char msg[256];

        snprintf(msg, sizeof(msg),
            "[WARNING] A bug damaged a server! (%d)",
            g_serversDamagedCount);
        AddStatusMsg(msg, LOG_WARNING);

		// server crash when health reaches 0
        if (s->health == 0)
        {
            g_serversCrashedCount++;

            snprintf(msg, sizeof(msg),
                "[ERROR] A server has crashed! (%d/%d)",
                g_serversCrashedCount, NUM_SERVERS);

            AddStatusMsg(msg, LOG_ERROR);

            int destroyPenalty = g_isLevel2 ? 25 : 12;
            Score_DeductPoints(destroyPenalty);

            if (g_serversCrashedCount >= NUM_SERVERS)
            {
                AddStatusMsg("[ERROR] All servers are down! GAME OVER!", LOG_ERROR);
            }
        }
    }
}

// update servers
// updates shake/flash timers and draws each server with its health bar
void UpdateServers(void)
{
    for (int i = 0; i < NUM_SERVERS; i++)
    {
        Server* s = &g_servers[i];

		// shake animation
        if (s->shakeTimer > 0)
        {
            s->shakeTimer -= CP_System_GetDt();
            s->shakeX = CP_Random_RangeFloat(-s->shakeIntensity, s->shakeIntensity);
            s->shakeY = CP_Random_RangeFloat(-s->shakeIntensity, s->shakeIntensity);
        }
        else
        {
            s->shakeX = 0;
            s->shakeY = 0;
        }

        // update flash timer
        if (s->flashTimer > 0)
        {
            s->flashTimer -= CP_System_GetDt();

            // red flash tint
            CP_Settings_Tint(CP_Color_Create(255, 80, 80, 255));
        }
        else
        {
            if (s->health <= 0)
				CP_Settings_Tint(CP_Color_Create(130, 130, 130, 255)); // gray tint for crashed
            else
				CP_Settings_Tint(CP_Color_Create(255, 255, 255, 255)); // normal tint
        }


        // draw server with shake
        CP_Image_Draw(imgServer,
            s->x + s->shakeX,
            s->y + s->shakeY,
            s->w, s->h, 255);

        // select health bar sprite
        CP_Image barImg =
            (s->health == 3) ? imgBarGreen :
            (s->health == 2) ? imgBarYellow :
            (s->health == 1) ? imgBarRed :
                               imgBarEmpty;

        float barW = s->w; // bar width
		float barH = 200.0f; // bar height
        float barX = s->x - 15.0f;
        float barY = s->y - (s->h / 2) + 20.0f;

        // draw bar with same shake offset
        CP_Image_Draw(
            barImg,
            barX + s->shakeX,
            barY + s->shakeY,
            barW,
            barH,
            255
        );
    }

    // reset tint
    CP_Settings_Tint(CP_Color_Create(255, 255, 255, 255));
}

void UpdateServers_Level2(void)
{
    UpdateServers();
}

int Servers_GetMissed(void)
{
    return g_bugsMissed;
}

int Servers_GetCrashedCount(void)
{
    return g_serversCrashedCount;
}
