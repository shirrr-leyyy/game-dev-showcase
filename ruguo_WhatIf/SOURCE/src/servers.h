// -----------------------------------------------------------------------------
// file:    servers.h
// author:  Yi Ting
// email:   yiting.tin@digipen.edu
//
// brief:   function declarations for the server system in Bug Hunt game
// -----------------------------------------------------------------------------

#ifndef SERVERS_H_
#define SERVERS_H_ 

#include "cprocessing.h"

// number of servers
#define NUM_SERVERS 3

// structure for a server
typedef struct {
    float x, y;      // position
    float w, h;      // size
    int   id;        // index
    int   health;    // 0–3 HP
    CP_Color color;

    float shakeTimer;
    float shakeIntensity;
    float shakeX;
    float shakeY;
    float flashTimer;
}Server;

extern Server g_servers[NUM_SERVERS];

void Servers_Init(void);
void Servers_Init_Level2(void);

int GetAliveServers(int* outArray);
Server* GetServerByIndex(int index);

int Servers_GetMissed(void);
int Servers_GetCrashedCount(void);

void UpdateServers(void);
void UpdateServers_Level2(void);

void DamageServer(int index);

#endif // SERVERS_H_