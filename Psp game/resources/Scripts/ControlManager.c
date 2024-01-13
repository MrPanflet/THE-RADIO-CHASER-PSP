#include <pspkernel.h>
#include <pspctrl.h>
#include <resources\Scripts\ControlManager.h>
#define Frames (30)


struct ButtonData Botones[12] = 
{
{PSP_CTRL_UP,2,0,0},
{PSP_CTRL_LEFT,2,0,0},
{PSP_CTRL_RIGHT,2,0,0},
{PSP_CTRL_DOWN,2,0,0},

{PSP_CTRL_CROSS,2,0,0},
{PSP_CTRL_CIRCLE,2,0,0},
{PSP_CTRL_TRIANGLE,2,0,0},
{PSP_CTRL_SQUARE,2,0,0},

{PSP_CTRL_LTRIGGER,2,0,0},
{PSP_CTRL_RTRIGGER,2,0,0},

{PSP_CTRL_START,2,0,0},
{PSP_CTRL_SELECT,2,0,0},

{PSP_CTRL_NOTE,2,0,0},

};

struct AutomaticPlayData AutomaticPlayButtons[Frames] = 
{
{RIGHT,2,0,0,1},
{20,0,0,0,250},
{RIGHT,2,1,0,160},
{0,2,2,1,0},
{CROSS,1,1,0,0},
{CROSS,2,0,0,0},
{RIGHT,2,1,0,235},
{CROSS,1,1,0,1},
{CROSS,0,0,0,1},
{RIGHT,2,1,0,150},
{CROSS,1,1,0,1},
{CROSS,0,0,0,1},
{RIGHT,2,1,0,75},
{CROSS,1,1,0,1},
{CROSS,0,0,0,1},
{RIGHT,2,1,0,10},
{CROSS,1,1,0,1},
{CROSS,0,0,0,1},
{RIGHT,2,1,0,25},
{CROSS,1,1,0,1},
{CROSS,0,0,0,1},
{RIGHT,2,1,0,500},


{RIGHT,2,0,0,1},



};


  SceCtrlData pad;
  int AnyKeyPressed;
 
 // AutomaticPlay = 0;
  


//int AutomaticPlay = 1;

void UpdateControls(){
    if(Automaticplay != 0){
       if(CurrentDuration <= 0 && Frame<Frames){
       CurrentDuration = AutomaticPlayButtons[Frame].Duration;
       Botones[AutomaticPlayButtons[Frame].Name].IsPressed = AutomaticPlayButtons[Frame].IsPressed;
       Botones[AutomaticPlayButtons[Frame].Name].IsDown = AutomaticPlayButtons[Frame].IsDown;
       if(Frame + 1 < Frames+1)
       {
        Frame = Frame+1;
       }
       else{
      //  Frame = 0;
        //  CurrentDuration = AutomaticPlayButtons[0].Duration;
             //  Botones[CROSS].IsDown = 0;
            //   Botones[CROSS].IsPressed = 0;
       }
       
       }
       else{
        CurrentDuration --;
       }
    }
    else{
AnyKeyPressed = 0;
 Botones[0].IsPressed = 2;
 Botones[1].IsPressed = 2;
 Botones[2].IsPressed = 2;
 Botones[3].IsPressed = 2;
 Botones[4].IsPressed = 2;
 Botones[5].IsPressed = 2;
 Botones[6].IsPressed = 2;
 Botones[7].IsPressed = 2;
 Botones[8].IsPressed = 2;
 Botones[9].IsPressed = 2;
 Botones[10].IsPressed = 2;
 Botones[11].IsPressed = 2;
 Botones[12].IsPressed = 2;

sceCtrlPeekBufferPositive(&pad,1);

if (pad.Buttons & PSP_CTRL_UP) {Botones[0].IsDown = 1; AnyKeyPressed = 1;}
if (pad.Buttons & PSP_CTRL_LEFT) {Botones[1].IsDown = 1; AnyKeyPressed = 1;}
if (pad.Buttons & PSP_CTRL_RIGHT) {Botones[2].IsDown = 1; AnyKeyPressed = 1;}
if (pad.Buttons & PSP_CTRL_DOWN) {Botones[3].IsDown = 1; AnyKeyPressed = 1;}

if (pad.Buttons & PSP_CTRL_CROSS) {Botones[4].IsDown = 1; AnyKeyPressed = 1;}
if (pad.Buttons & PSP_CTRL_CIRCLE) {Botones[5].IsDown = 1; AnyKeyPressed = 1;}
if (pad.Buttons & PSP_CTRL_TRIANGLE) {Botones[6].IsDown = 1; AnyKeyPressed = 1;}
if (pad.Buttons & PSP_CTRL_SQUARE) {Botones[7].IsDown = 1; AnyKeyPressed = 1;}

if (pad.Buttons & PSP_CTRL_LTRIGGER) {Botones[8].IsDown = 1; AnyKeyPressed = 1;}
if (pad.Buttons & PSP_CTRL_RTRIGGER) {Botones[9].IsDown = 1; AnyKeyPressed = 1;}

if (pad.Buttons & PSP_CTRL_START) {Botones[10].IsDown = 1; }
if (pad.Buttons & PSP_CTRL_SELECT) {Botones[11].IsDown = 1; }

if (pad.Buttons & PSP_CTRL_NOTE) {Botones[12].IsDown = 1; }


sceCtrlReadBufferNegative(&pad,1);

if (pad.Buttons & PSP_CTRL_UP) {Botones[0].IsDown = 0; Botones[0].WasPressed = 0; Botones[0].IsPressed == 0;}
if (pad.Buttons & PSP_CTRL_LEFT) {Botones[1].IsDown = 0; Botones[1].WasPressed = 0; Botones[1].IsPressed == 0;}
if (pad.Buttons & PSP_CTRL_RIGHT) {Botones[2].IsDown = 0;  Botones[2].WasPressed = 0; Botones[2].IsPressed == 0;}
if (pad.Buttons & PSP_CTRL_DOWN) {Botones[3].IsDown = 0;  Botones[3].WasPressed = 0; Botones[3].IsPressed == 0;}

if (pad.Buttons & PSP_CTRL_CROSS) {Botones[4].IsDown = 0;  Botones[4].WasPressed = 0; Botones[4].IsPressed == 0;}
if (pad.Buttons & PSP_CTRL_CIRCLE) {Botones[5].IsDown = 0;  Botones[5].WasPressed = 0; Botones[5].IsPressed == 0;}
if (pad.Buttons & PSP_CTRL_TRIANGLE) {Botones[6].IsDown = 0;  Botones[6].WasPressed = 0; Botones[6].IsPressed == 0;}
if (pad.Buttons & PSP_CTRL_SQUARE) {Botones[7].IsDown = 0;  Botones[7].WasPressed = 0; Botones[7].IsPressed == 0;}

if (pad.Buttons & PSP_CTRL_LTRIGGER) {Botones[8].IsDown = 0;  Botones[8].WasPressed = 0; Botones[8].IsPressed == 0;}
if (pad.Buttons & PSP_CTRL_RTRIGGER) {Botones[9].IsDown = 0;  Botones[9].WasPressed = 0; Botones[9].IsPressed == 0;}

if (pad.Buttons & PSP_CTRL_START) {Botones[10].IsDown = 0;  Botones[10].WasPressed = 0; Botones[10].IsPressed == 0;}
if (pad.Buttons & PSP_CTRL_SELECT) {Botones[11].IsDown = 0;  Botones[11].WasPressed = 0; Botones[11].IsPressed == 0;}

if (pad.Buttons & PSP_CTRL_NOTE) {Botones[12].IsDown = 0;  Botones[12].WasPressed = 0; Botones[12].IsPressed == 0;}




if(Botones[0].WasPressed == 0 && Botones[0].IsDown == 1){
    Botones[0].IsPressed = 1;
    Botones[0].WasPressed = 1;
} 
if(Botones[1].WasPressed == 0 && Botones[1].IsDown == 1){
    Botones[1].IsPressed = 1;
    Botones[1].WasPressed = 1;
} 
if(Botones[2].WasPressed == 0 && Botones[2].IsDown == 1){
    Botones[2].IsPressed = 1;
    Botones[2].WasPressed = 1;
} 
if(Botones[3].WasPressed == 0 && Botones[3].IsDown == 1){
    Botones[3].IsPressed = 1;
    Botones[3].WasPressed = 1;
} 
if(Botones[4].WasPressed == 0 && Botones[4].IsDown == 1){
    Botones[4].IsPressed = 1;
    Botones[4].WasPressed = 1;
} 
if(Botones[5].WasPressed == 0 && Botones[5].IsDown == 1){
    Botones[5].IsPressed = 1;
    Botones[5].WasPressed = 1;
} 
if(Botones[6].WasPressed == 0 && Botones[6].IsDown == 1){
    Botones[6].IsPressed = 1;
    Botones[6].WasPressed = 1;
} 
if(Botones[7].WasPressed == 0 && Botones[7].IsDown == 1){
    Botones[7].IsPressed = 1;
    Botones[7].WasPressed = 1;
} 
if(Botones[8].WasPressed == 0 && Botones[8].IsDown == 1){
    Botones[8].IsPressed = 1;
    Botones[8].WasPressed = 1;
} 
if(Botones[9].WasPressed == 0 && Botones[9].IsDown == 1){
    Botones[9].IsPressed = 1;
    Botones[9].WasPressed = 1;
} 
if(Botones[10].WasPressed == 0 && Botones[10].IsDown == 1){
    Botones[10].IsPressed = 1;
    Botones[10].WasPressed = 1;
} 
if(Botones[11].WasPressed == 0 && Botones[11].IsDown == 1){
    Botones[11].IsPressed = 1;
    Botones[11].WasPressed = 1;
} 
if(Botones[12].WasPressed == 0 && Botones[12].IsDown == 1){
    Botones[12].IsPressed = 1;
    Botones[12].WasPressed = 1;
} 

    }

}

void IsKeyPressed(){
    
}