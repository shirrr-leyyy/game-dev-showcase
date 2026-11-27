// -----------------------------------------------------------------------------
// file:    bugs.c
// author:  Yi Ting
// email:   yiting.tin@digipen.edu
//
// brief:   function definitions for the bug system in Bug Hunt games
//          handles spawning, movement, targeting, click detection,
//          collisions, death animations, and state resets
// -----------------------------------------------------------------------------

#include "bugs.h"
#include "servers.h"
#include "bugs_ui.h"
#include "score.h"
#include "utils.h"
#include "bugs_console.h"
#include "audio.h"
#include "cprocessing.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// maximum number of bugs that can be active at once
#define MAX_BUGS 12

// number of bug sprite types
#define BUG_TYPES 4

// number of frames in death animation
#define BUG_DEATH_FRAMES 3

// define pi
#define M_PI 3.14159265358979323846f

// bug drawing size
#define BUG_W 120.0f
#define BUG_H 120.0f

// ellipse hitbox for click & collision
#define BUG_RX 33.0f
#define BUG_RY 35.0f

// time per death animation frame 
#define BUG_DEATH_TIME 0.1f

// store bug sprite images
CP_Image g_bugSprites[BUG_TYPES];
CP_Image g_bugDeath[BUG_TYPES][BUG_DEATH_FRAMES];

// bug data structure
typedef struct {
    float x, y;
    float speed;

    float waveOffset; // phase offset for zigzag movement
    float waveTimer;

    float rotation; // image rotation

    int spriteIndex;
    int targetServer;

    int active;
    int dying;

    int deathFrame;
    float deathTimer;
} Bug;

// array of all bugs
static Bug g_bugs[MAX_BUGS];

// difficulty modifiers
static int g_maxActiveBugs = 5; // max bugs active at once
static float g_spawnRate = 1.0f; // time between spawns
static float g_speedMultiplier = 1.0f; // movement difficulty tweak
static float g_zigzagMultiplier = 1.0f; // wiggle/zigzag difficulty tweak

static float g_spawnTimer = 0.0f;
static int g_bugKills = 0;

extern int g_isLevel2; // 0 = level1, 1 = level2

// load sprite images
static void Bugs_LoadSprites(void)
{
    // alive sprites for all 4 bug types
    g_bugSprites[0] = CP_Image_Load("Assets/bug_1.png");
    g_bugSprites[1] = CP_Image_Load("Assets/bug_2.png");
    g_bugSprites[2] = CP_Image_Load("Assets/bug_3.png");
    g_bugSprites[3] = CP_Image_Load("Assets/bug_4.png");

    // death animation frames for bug type 1
    g_bugDeath[0][0] = CP_Image_Load("Assets/bug_1_dead1.png");
    g_bugDeath[0][1] = CP_Image_Load("Assets/bug_1_dead2.png");
    g_bugDeath[0][2] = CP_Image_Load("Assets/bug_dead3.png");

    // death animation frames for bug type 2
    g_bugDeath[1][0] = CP_Image_Load("Assets/bug_2_dead1.png");
    g_bugDeath[1][1] = CP_Image_Load("Assets/bug_2_dead2.png");
    g_bugDeath[1][2] = CP_Image_Load("Assets/bug_dead3.png");

    // death animation frames for bug type 3
    g_bugDeath[2][0] = CP_Image_Load("Assets/bug_3_dead1.png");
    g_bugDeath[2][1] = CP_Image_Load("Assets/bug_3_dead2.png");
    g_bugDeath[2][2] = CP_Image_Load("Assets/bug_dead3.png");

    // death animation frames for bug type 4
    g_bugDeath[3][0] = CP_Image_Load("Assets/bug_4_dead1.png");
    g_bugDeath[3][1] = CP_Image_Load("Assets/bug_4_dead2.png");
    g_bugDeath[3][2] = CP_Image_Load("Assets/bug_dead3.png");
}

// reset bugs
// ensure all bugs are inactive and ready to spawn
static void Bugs_Reset(void)
{
    for (int i = 0; i < MAX_BUGS; i++)
    {
        g_bugs[i].active = 0;
        g_bugs[i].dying = 0;
        g_bugs[i].deathFrame = 0;
        g_bugs[i].deathTimer = 0.0f;

        g_bugs[i].x = 0.0f;
        g_bugs[i].y = 0.0f;

        g_bugs[i].speed = 0.0f;
        g_bugs[i].rotation = 0.0f;
        g_bugs[i].waveOffset = 0.0f;
        g_bugs[i].waveTimer = 0.0f;

        g_bugs[i].spriteIndex = 0;
        g_bugs[i].targetServer = 0;
    }

    g_spawnTimer = 0.0f;
    g_bugKills = 0;
}

// init for level 1
void Bugs_Init_1(void)
{
    Bugs_LoadSprites();
    Bugs_Reset();

    g_maxActiveBugs = 5;
    g_spawnRate = 0.8f;
    g_speedMultiplier = 1.25f;
    g_zigzagMultiplier = 1.0f;
}

// init for level 2
void Bugs_Init_2(void)
{
    Bugs_LoadSprites();
    Bugs_Reset();

    g_maxActiveBugs = 7;
    g_spawnRate = 0.6f;
    g_speedMultiplier = 1.8f;
    g_zigzagMultiplier = 1.4f;
}

// spawn bug
static void Bugs_Spawn(void)
{
    // fetch alive server indices 
    int alive[NUM_SERVERS];
    int aliveCount = GetAliveServers(alive);
    if (aliveCount == 0) return;

    for (int i = 0; i < g_maxActiveBugs; i++)
    {
        Bug* b = &g_bugs[i];
        if (b->active || b->dying) continue;

        int w = CP_System_GetWindowWidth();
        int h = CP_System_GetWindowHeight();

        // spawn x randomly across width
        b->x = (float)(rand() % w);

        // spawn y near the bottom
        b->y = h - 300.0f + (rand() % 100);

		// randomise speed
        b->speed = 80.0f + (rand() % 60);

		// mark this bug as active
        b->active = 1;
        b->dying = 0;

		// choose random bug type
        b->spriteIndex = rand() % BUG_TYPES;

        b->rotation = 0.0f;

        // initialize zigzag behavior
        b->waveOffset = (float)(rand() % 360); // random phase
		b->waveTimer = (float)(rand() % 100); // random start time

        // pick a random target server that is still alive
        b->targetServer = alive[rand() % aliveCount];

        return;
    }
}

// update bug death animation
static void Bug_UpdateDeath(Bug* b)
{
    b->deathTimer += CP_System_GetDt();

    if (b->deathTimer >= BUG_DEATH_TIME)
    {
        b->deathTimer = 0.0f;
        b->deathFrame++;

        if (b->deathFrame >= BUG_DEATH_FRAMES)
        {
            b->dying = 0;
            return;
        }
    }

    CP_Image_Draw(
        g_bugDeath[b->spriteIndex][b->deathFrame],
        b->x, b->y,
        BUG_W, BUG_H,
        255
    );
}

// update alive bug
// handles zigzag movement, target tracking and collision with servers
static void Bug_UpdateAlive(Bug* b)
{
    Server* target = GetServerByIndex(b->targetServer);

    // retarget if target died mid flight
    if (!target || target->health <= 0)
    {
        int alive[NUM_SERVERS];
        int n = GetAliveServers(alive);

		// if no servers alive, bug is inactive
        if (n == 0)
        {
            b->active = 0;
            return;
        }

        // choose a new random server
        b->targetServer = alive[rand() % n];
        target = GetServerByIndex(b->targetServer);
    }

    // direction vector from bug to server
    float dx = target->x - b->x;
    float dy = target->y - b->y;

    // distance for normalization
    float dist = sqrtf(dx * dx + dy * dy);

    // avoid divide by zero
    if (dist < 0.001f) dist = 0.001f;

    // convert into unit direction
    dx /= dist;
    dy /= dist;

    // zigzag motion
    // zig = sin(time * frequency + phaseOffset) * amplitude

    b->waveTimer += CP_System_GetDt();
    float zig = sinf(b->waveTimer * 2.5f + b->waveOffset) * (0.6f * g_zigzagMultiplier);

    // compute rotation matrix
    float cosA = cosf(zig);
    float sinA = sinf(zig);

	// apply zigzag rotation to movement vector
    float moveDx = dx * cosA - dy * sinA;
    float moveDy = dx * sinA + dy * cosA;

    float moveSpeed = b->speed * g_speedMultiplier;

    // move based on direction * speed * deltaTime
    b->x += moveDx * moveSpeed * CP_System_GetDt();
    b->y += moveDy * moveSpeed * CP_System_GetDt();

    // rotate sprite
    b->rotation = atan2f(moveDy, moveDx) * (180.0f / M_PI) + 90.0f;

    // server hitbox
    float hitW = target->w * 0.6f;
    float hitH = target->h * 0.5f;

	// check bug collision with server
    if (EllipseRectCollide(b->x, b->y, BUG_RX, BUG_RY, target->x, target->y, hitW, hitH))
    {
        b->active = 0;

		// deduct points on server hit based on level
        if (g_isLevel2) Score_DeductPoints(8);
        else Score_DeductPoints(4);

		// damage the server
        DamageServer(b->targetServer);
        return;
    }

	// draw alive bug
    CP_Image_DrawAdvanced(g_bugSprites[b->spriteIndex], b->x, b->y, BUG_W, BUG_H, 255, b->rotation
    );
}

// update bug
// decides which update path to use based on state
static void Bug_Update(Bug* b)
{
    if (b->dying)
    {
        b->active = 0;
        Bug_UpdateDeath(b);
        return;
    }

    if (!b->active)
        return;

    Bug_UpdateAlive(b);
}

// collider between bugs
// prevents bugs from stacking on top of each other
static void Bugs_Collider(void)
{
    const float minDist = 60.0f;
    const float push = 0.5f;

    for (int i = 0; i < g_maxActiveBugs; i++)
    {
		// skip inactive bugs
        if (!g_bugs[i].active) 
            continue;

        for (int j = i + 1; j < g_maxActiveBugs; j++)
        {
            if (!g_bugs[j].active) 
                continue;

            // compute vector between the two bugs
            float dx = g_bugs[i].x - g_bugs[j].x;
            float dy = g_bugs[i].y - g_bugs[j].y;

            float distSq = dx * dx + dy * dy;
            float minSq = minDist * minDist;

			// only push if they are too close
            if (distSq < minSq && distSq > 0.01f)
            {
                float dist = sqrtf(distSq);

				// normalize direction
                dx /= dist;
                dy /= dist;

				// compute overlap amount
                float overlap = (minDist - dist) * push;

				// push bugs apart
                g_bugs[i].x += dx * overlap;
                g_bugs[i].y += dy * overlap;
                g_bugs[j].x -= dx * overlap;
                g_bugs[j].y -= dy * overlap;
            }
        }
    }
}

// bug kill logic
// handles scoring, streaks, and death state
static int Bug_HandleKill(Bug* b, int base, int* tierPts)
{
    b->active = 0;

	// start death animation
    b->dying = 1;
    b->deathFrame = 0;
    b->deathTimer = 0;

	// incremenmt kill count
    g_bugKills++;

	// calculate points based on streak tier
    int tier = Score_GetStreakTier();
    int pts = base;

    if (tier >= 1 && tier <= 4)
        pts = tierPts[tier - 1];

    Score_AddPoints(pts);
    Score_IncrementStreak();

    Audio_PlayBugDeath();

    char m[128];
    snprintf(m, sizeof(m), "[MESSAGE] Bug eliminated! (%d)", g_bugKills);
    AddStatusMsg(m, LOG_MESSAGE);

    return 1;
}

// handle mouse killing
static void Bugs_HandleClick(int basePoints, int misPenalty, int* tierList)
{
	// get mouse position
    float mx = CP_Input_GetMouseX();
    float my = CP_Input_GetMouseY();

	int hitBug = 0; // 1 if a bug was hit

	// loop through all active bugs
    for (int i = 0; i < g_maxActiveBugs; i++)
    {
        Bug* b = &g_bugs[i];

		// skip inactive or dying bugs
        if (!b->active || b->dying) 
            continue;

        if (IsEllipseClicked(b->x, b->y, BUG_RX, BUG_RY, mx, my))
        {
            hitBug = Bug_HandleKill(b, basePoints, tierList);
			break; // only one bug per click
        }
    }

	// if no bug was hit, apply misclick penalty
    if (!hitBug)
    {
        Score_DeductPoints(misPenalty);
        Score_ResetStreak();
    }
}

// update for level 1
// handle spawning, updating, clicking, and collision
void UpdateBugs_1(void)
{
    g_spawnTimer += CP_System_GetDt();

	// if timer exceeds spawn rate, spawn a new bug
    if (g_spawnTimer > g_spawnRate)
    {
        Bugs_Spawn();
        g_spawnTimer = 0;
    }

	// update all bugs
    for (int i = 0; i < g_maxActiveBugs; i++)
        Bug_Update(&g_bugs[i]);

    if (CP_Input_MouseClicked())
    {
		// level 1 tiered structure
        int tierList[4] = { 4, 4, 5, 6 };
        Bugs_HandleClick(3, 2, tierList); // base points=3, miss penalty = 2
    }

    Bugs_Collider();
}

// update for level 2
void UpdateBugs_2(void)
{
    g_spawnTimer += CP_System_GetDt();
    if (g_spawnTimer > g_spawnRate)
    {
        Bugs_Spawn();
        g_spawnTimer = 0;
    }

	// update all bugs
    for (int i = 0; i < g_maxActiveBugs; i++)
        Bug_Update(&g_bugs[i]);

    if (CP_Input_MouseClicked())
    {
		// level 2 tiered structure
        int t2[4] = { 5, 6, 7, 8 };
        Bugs_HandleClick(4, 3, t2); // base points = 4, miss penalty = 3
    }

	// push bugs apart to avoid stacking
    Bugs_Collider();
}
void Bugs_Exit(void)
{
	// loop through all bug sprites and free images
    for (int i = 0; i < BUG_TYPES; i++)
    {
        CP_Image_Free(g_bugSprites[i]);

        for (int f = 0; f < BUG_DEATH_FRAMES; f++)
            CP_Image_Free(g_bugDeath[i][f]);
    }
}
