#ifndef CTRLMANG_H_INCLUDED
#define CTRLMANG_H_INCLUDED

/* Sets up callbacks */

#define UP (0)
#define LEFT (1)
#define RIGHT (2)
#define DOWN (3)

#define CROSS (4)
#define CIRCLE (5)
#define TRIANGLE (6)
#define SQUARE (7)

#define LTRIGGER (8)
#define RTRIGGER (9)

#define START (10)
#define SELECT (11)


#define MUSICNOTE (12)


int AnyKeyPressed;

struct ButtonData
{
int Name;
int IsPressed;
int WasPressed;
int IsDown;
};
struct AutomaticPlayData
{
int Name;
int IsPressed;
int IsDown;
int IsReleased;
int Duration;
};

 int Automaticplay ;
int CurrentDuration ;
int Frame;
SceCtrlData pad;
void UpdateControls();
struct ButtonData Botones[12];



#endif