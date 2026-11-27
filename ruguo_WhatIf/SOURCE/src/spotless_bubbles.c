//---------------------------------------------------------
// file:	spotless_bubbles.c
// author:	Dylan
// email:	dylanjoonteck.lim@digipen.edu
//
// brief:	Dishwasher level 1-2 game Bubbles animation
//
//---------------------------------------------------------

#include "spotless_bubbles.h"
#include <math.h>
#include <stdbool.h>

// --------------------------------------------------
// Module-local state
// --------------------------------------------------
static CP_Image imgBubble = 0;
static bool     bubbleImageOK = false;

typedef struct Bubble {
    float x, y;
    float vx, vy;
    float radius;
    float life;     // remaining life (seconds)
    float maxLife;  // starting life (seconds)
} Bubble;

enum { MAX_BUBBLES = 200 };

static Bubble bubbles[MAX_BUBBLES];
static int    bubbleCount = 0;
static float  bubbleEmitAcc = 0.0f;  // emitter accumulator

// --------------------------------------------------
// Internal helpers
// --------------------------------------------------
static void Bubble_Spawn(float x, float y)
{
    if (bubbleCount >= MAX_BUBBLES)
        return;

    Bubble* b = &bubbles[bubbleCount++];

    // small random offset around cursor
    b->x = x + CP_Random_RangeFloat(-8.0f, 8.0f);
    b->y = y + CP_Random_RangeFloat(-8.0f, 8.0f);

    // random sideways velocity
    b->vx = CP_Random_RangeFloat(-60.0f, 60.0f);

    // start slightly moving up; gravity will pull them down
    b->vy = CP_Random_RangeFloat(-140.0f, -60.0f);

    b->radius = CP_Random_RangeFloat(32.0f, 64.0f);

    b->maxLife = CP_Random_RangeFloat(0.6f, 1.2f);
    b->life = b->maxLife;
}

static void Bubbles_EmitFromCursor(void)
{
    float dt = CP_System_GetDt();

    // only emit while mouse is moving or held down
    if (!CP_Input_MouseMoved() && !CP_Input_MouseDown(MOUSE_BUTTON_LEFT))
        return;

    float emitRate = 80.0f;   // bubbles per second when active
    bubbleEmitAcc += emitRate * dt;

    int numToSpawn = (int)bubbleEmitAcc;
    if (numToSpawn <= 0)
        return;

    bubbleEmitAcc -= numToSpawn;

    float mx = CP_Input_GetMouseX();
    float my = CP_Input_GetMouseY();

    for (int i = 0; i < numToSpawn; ++i)
        Bubble_Spawn(mx, my);
}

static void Bubbles_UpdateAndDraw(void)
{
    float dt = CP_System_GetDt();
    float gravity = 350.0f;
    int   screenH = CP_System_GetWindowHeight();

    CP_Settings_Save();

    int i = 0;
    while (i < bubbleCount) {
        Bubble* b = &bubbles[i];

        b->life -= dt;
        if (b->life <= 0.0f || b->y - b->radius > screenH + 50.0f) {
            // remove bubble by swapping with last
            bubbles[i] = bubbles[bubbleCount - 1];
            --bubbleCount;
            continue;
        }

        // simple physics
        b->vy += gravity * dt;
        b->x += b->vx * dt;
        b->y += b->vy * dt;

        float t = b->life / b->maxLife;   // 1 -> 0
        if (t < 0.0f) t = 0.0f;

        // shrink slightly as they die
        float diameter = b->radius * 2.0f * (0.6f + 0.4f * t);
        int   alpha = (int)(255.0f * t);

        if (alpha < 0)   alpha = 0;
        if (alpha > 255) alpha = 255;

        if (bubbleImageOK) {
            CP_Image_Draw(imgBubble, b->x, b->y, diameter, diameter, alpha);
        }
        else {
            // fallback: simple white circle
            CP_Settings_NoStroke();
            CP_Settings_Fill(CP_Color_Create(255, 255, 255, alpha));
            CP_Graphics_DrawCircle(b->x, b->y, diameter);
        }

        ++i;
    }

    CP_Settings_Restore();
}

// --------------------------------------------------
// Public API
// --------------------------------------------------
void Spotless_Bubbles_Init(void)
{
    // Load bubble sprite; fallback to circles if this fails
    imgBubble = CP_Image_Load("assets/bubble.png");
    bubbleImageOK = (imgBubble && CP_Image_GetWidth(imgBubble) > 0);

    bubbleCount = 0;
    bubbleEmitAcc = 0.0f;
}

void Spotless_Bubbles_Reset(void)
{
    bubbleCount = 0;
    bubbleEmitAcc = 0.0f;
}

void Spotless_Bubbles_Update(void)
{
    Bubbles_EmitFromCursor();
    Bubbles_UpdateAndDraw();
}

void Spotless_Bubbles_Exit(void)
{
    if (bubbleImageOK) {
        CP_Image_Free(imgBubble);
        imgBubble = 0;
        bubbleImageOK = false;
    }
}
