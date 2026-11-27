// -------------------------------------------------------------------------------
// file:    bugs_console.h
// author:  Yi Ting
// email:   yiting.tin@digipen.edu
//
// brief:   function definitions for the console message system in Bug Hunt games
// -------------------------------------------------------------------------------

#ifndef BUGS_CONSOLE_H_
#define BUGS_CONSOLE_H_

typedef enum
{
    LOG_ERROR,
    LOG_WARNING,
    LOG_MESSAGE
} LogType;

void Console_Init(void);
void AddStatusMsg(const char* msg, LogType type);
void DrawStatusMsg(void);
void Console_Clear(void);
void Bugs_Console_Exit(void);

#endif // BUGS_CONSOLE_H_