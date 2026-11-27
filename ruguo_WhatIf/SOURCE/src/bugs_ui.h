// -----------------------------------------------------------------------------
// file:    bugs_ui.h
// author:  Yi Ting
// email:   yiting.tin@digipen.edu
//
// brief:   function declarations for the user interface system in Bug Hunt game
// -----------------------------------------------------------------------------

#ifndef BUGS_UI_H_
#define BUGS_UI_H_

void UI_Init(void);
void UI_Level1(void);
void UI_Level2(void);
int  DrawPauseButton(const char* label, float bx, float by, float* scale, float btnW, float btnH);
void DrawHowToPlay(void);
void TogglePause(void);
void DrawPauseMenu(void);
void DrawUI(void);
int  DrawGameOverButton(const char* label, float x, float y, float* scale);
void DrawGameOver(void);
void DrawTimer(float timerValue);
void Bugs_Ui_Exit(void);

#endif // BUGS_UI_H_