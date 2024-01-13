#include <stdio.h>
#include <stdlib.h>
#include <pspkernel.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <pspgum.h>
#include "glib2d.h"
#include <psprtc.h>
#include <pspaudiolib.h>
//#include <time.h>
//#include <psprtc.h>

#include "resources\Scripts\callbacks.h"
#include "resources\Scripts\MapManager.h"
#include "resources\Scripts\ControlManager.h"
#include "resources\Scripts\AudioManagement.h"


#define MapRenderXDev 300     // 130
#define MapRenderYDev 200     // 130
#define MapRenderXGame 300    // 130
#define MapRenderYGame 200    // 130
#define PlayerSpeedRun 135    //135
#define PlayerSpeedSlope 140  //140
#define PlayerSpeedWalk 190   //170

#define aceleration   0.002    //0.0015
#define aceleration2  0.0025     //0.002
#define aceleration3  0.005     //0.001
#define MaxSpeedWalk  2.3   //3.7
#define MaxSpeedRun  3.4   //4.1
#define Deceleration  0.915     //0.9
#define StopValue 0.1     //0.1

#define MaxCoyote 6


#define DemoAllowed 0
#define WaitTillDemoMode 2500


#define CoinsUntilNewLife 50

#define Language 1
#define Style 0


//0 = Spanish
//1 = English
#define DefautJumpForce 100

PSP_MODULE_INFO("Plataformero",0,2,0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);



//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------

double Gravity = 4.4;
double JumpForce = 100; 
int PlayerAnimInterval = 6;
float MaxPlayerSpeed = PlayerSpeedWalk;

float CurrentMaxSpeed = MaxSpeedWalk;


int TimeUntillDemo = WaitTillDemoMode;



int lives = 3;


//Map loading variable (DO NOT TOUCH)
//----------------------------------------------------------------------------------
//int CurrentLevel = 0;
//int MaxLevels = 0;
//int Amount;
//char MapName[11]="Lvl000.txt";
//int r;
int i;
//----------------------------------------------------------------------------------

//Structs definition
//----------------------------------------------------------------------------------

typedef struct PlayerStruct{
    float X;
    float Y;
    float SpeedX;
    float SpeedY;
};
typedef struct MouseStruct{
    int x;
    int y;
    int Tile;
    int TileX;
    int TileY;
     int TypeOfTileBehavior;
};


typedef struct Rectangle{
    float x;
    float y;
    float width;
    float height;
}Rectangle;

typedef struct Vector2
{
    int x;
    int y;
}Vector2;


typedef struct ButtonStruct{
    const int x;
    const int y;
    int Width;
    int Heigth;
    int Function;
    bool Active;
    g2dColor color;
    char Name[14];
};


typedef struct OverWorldBlocks{
    int State;
    //# = Normal wall
    //. = Road
    //S = UnlockedLevel
    //A = BlockLeftMovement
    //B = BlockRightMovement
    //C = BlockUpMovement
    //D = BlockDownMovement

    int LevelToLoad;
    //0 and up = Level
    //-1 = No level to load
};


typedef struct ProTextureStruct  {
int SourceX;
int SourceY;
int SourceWidth;
int SourceHeigt;

int DestWidth;
int DestHeigt;

};

struct ButtonStruct MainMenuButtons [3] = {
        {20,10,20,20,4,true,BLACK,"Comenzar"},
        {20,40,20,20,5,true,BLACK,"Continuar"},
        {20,70,20,20,7,true,BLACK,"Editor"},
};
//4 = Continuar
//5 = Comenzar
    struct ButtonStruct PauseButtons [3] = {
        {20,50,20,20,4,true,BLACK,"Resume"},
        {20,80,20,20,6,true,BLACK,"Save"},
        {20,110,20,20,3,true,BLACK,"MainMenu"},
    };



  int MENUDATATiles[5][10] = {
   {65,71,68,74,219,188,224,216,36,0},
   {66,72,69,75,222,185,226,229,35,1},
   {167,173,170,179,214,191,208,232,207,2},
   {107,109,108,111,235,236,209,215,206,3},
   {119,121,120,123,196,198,187,193,210,4}
  };

  int MENUDATATypes[5][10] = {
   {4,4,4,4,3,2,0,0,8,6},
   {3,3,3,3,3,1,0,0,0,6},
   {0,0,0,0,0,2,0,0,0,6},
   {5,5,5,5,0,0,0,0,0,6},
   {5,5,5,5,8,7,0,0,1,6}
  };



typedef struct  PlayerCollider
{
    float XOffset;
    float YOffset;
    float sizeX;
    float sizeY;
    /* data */
};
typedef struct Enemy
{
    int X;
    float Y;
    int OriginalY;
    int Type;
    int Anim;
    int SpeedX;
    float SpeedY;
}Enemy;


typedef struct Trampoline
{
    int X;
    int Y;
    int Anim;
    int AnimFrame;
}Trampoline;





//0 = GameMode
//1 = TransitionWait
//2 = Transition
//3 = Pause 
//4 = Menu

//----------------------------------------------------------------------------------
struct Vector2 SpawnPos = (Vector2){-20,-20};

int EnemiesAnim;
int EnemiesAnimCount;
  
    //MapRelatedVariables
    //--------------------------------------------------------------------------------------
    struct PlayerStruct MapView;
    struct ProTextureStruct HorizontalAnim;
    struct ProTextureStruct PlayerTextureControl = {0,0,23,41,23,41};
    struct MouseStruct MousePos;


int CamX;
float CamY;

    
    struct PlayerStruct Player;
    struct PlayerCollider PlayerCol = {5,5,12,36};
    struct PlayerStruct Flag;
    struct PlayerStruct Gradient;
    //Rectangle PlayerColison();

 struct OverWorldBlocks OverWorldSquares[29][16];



 struct Enemy Enemies1[40];
 struct Vector2 EnemyBlockers[85];

 struct Trampoline Trampolines[50];
 
 //struct Enemy* Enemies2[40];
 
int Enemies = 0;
int EnemiesBlockers = 0;

int AmountOfTrampolines = 0;



g2dColor BackGroundColors[4] = {GU_RGBA(73,201,255,255),GU_RGBA(160,179,221,255),GU_RGBA(255,255,201,255),GU_RGBA(0,60,83,255)};



int BackGroundsID[32] = {0,0,0,0,0,0,0,1,1,0,1,1,1,1,1,1,1,2,2,2,2,2,2,0,2,2,3,3,3,3,3,3,3,3};

int NextPlayableLevel = 0;
    
    
    int SelectedButton = 0;
    int GradientINT;
    int GameState = 4;
    //0 = Game
    //4 = Menu
    //5 = Death
    //6 = OverWorld
    //--------------------------------------------------------------------------------------
    



//Checks collison
//--------------------------------------------------------------------------------------
bool SimpleCollisonRecsDetection(Rectangle REC1,Rectangle REC2){

    bool collision = false;

    if ((REC1.x < (REC2.x + REC2.width) && (REC1.x + REC1.width) > REC2.x) &&
        (REC1.y < (REC2.y + REC2.height) && (REC1.y + REC1.height) > REC2.y)) collision = true;

    return collision;
}
//--------------------------------------------------------------------------------------

Rectangle GetCollisionRec(Rectangle rec1, Rectangle rec2)
{
    Rectangle rec = { 0, 0, 0, 0 };

    if (SimpleCollisonRecsDetection(rec1, rec2))
    {
        float dxx = fabsf(rec1.x - rec2.x);
        float dyy = fabsf(rec1.y - rec2.y);

        if (rec1.x <= rec2.x)
        {
            if (rec1.y <= rec2.y)
            {
                rec.x = rec2.x;
                rec.y = rec2.y;
                rec.width = rec1.width - dxx;
                rec.height = rec1.height - dyy;
            }
            else
            {
                rec.x = rec2.x;
                rec.y = rec1.y;
                rec.width = rec1.width - dxx;
                rec.height = rec2.height - dyy;
            }
        }
        else
        {
            if (rec1.y <= rec2.y)
            {
                rec.x = rec1.x;
                rec.y = rec2.y;
                rec.width = rec2.width - dxx;
                rec.height = rec1.height - dyy;
            }
            else
            {
                rec.x = rec1.x;
                rec.y = rec1.y;
                rec.width = rec2.width - dxx;
                rec.height = rec2.height - dyy;
            }
        }

        if (rec1.width > rec2.width)
        {
            if (rec.width >= rec2.width) rec.width = rec2.width;
        }
        else
        {
            if (rec.width >= rec1.width) rec.width = rec1.width;
        }

        if (rec1.height > rec2.height)
        {
            if (rec.height >= rec2.height) rec.height = rec2.height;
        }
        else
        {
           if (rec.height >= rec1.height) rec.height = rec1.height;
        }
    }

    return rec;
}



//Variable definition
//--------------------------------------------------------------------------------------
    bool DeveloperMode = false;
    bool LeftMousePressed = false;
    bool RightMousePressed = false;

    //Screen related
    //--------------------------------------------------------------------------------------
    const int screenWidth = 480;
    const int screenHeight = 272;
    int CurrentScreenWidth = screenWidth;
    int CurrrentScreenHeight = screenHeight;
    //--------------------------------------------------------------------------------------

    //Animation related
    //--------------------------------------------------------------------------------------
    int CurrentPlayerAnimStateH = 0;
    int CurrentPlayerAnimStateV = 0;
    int CurrentPlayerDirection = 0;
    //--------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------


//0 = abajo
//1 = arriba
//2 = izquierda
//3 = derecha

int PlayerAnimCount;


int CoyoteTime = 0;


    int XMin = 0;
     int XMax = MapWidth;
     int YMax;
     int YMin;
     int PLAYERX ;
     int PLAYERY ;

double deltaTime = 0.1; /*IT JUST WORKS, IT JUST WORKS LITTLE LIES STUNNING SHOWS PEOPLE BUY MONEY FLOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO  */

//pspTime time;


// u16 MicroSeconds1;
// u16 MicroSeconds2;
// u16 MicroSeconds3;





int main() 
{
    
      //--------------------------------------------------------------------------------------
setupCallbacks();
StartTheAduioSystem();
        //--------------------------------------------------------------------------------------
ClearMap();
SpawnPos = (Vector2){-20,-20};
LoadMap(0);
CurrentLevel = 1;
Map2ToMap1();
ResetPlayerAndFlagPos();

LoadOverWorld();
    // Initialization
    //--------------------------------------------------------------------------------------
MaxLevels = CheckNumberOfFiles();
//CurrentLevel++;

  SceCtrlData AnalougePad;
  sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

  UpdateControls();
//Initialize window and audio device
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
int EditedLayer = 2;

// TODO: Load resources / Initialize variables at this point
// Load Images and Sounds
//--------------------------------------------------------------------------------------
g2dTexture* TileSheet = NULL;
g2dTexture* PlayerTexture = NULL;
if(Style == 0){
PlayerTexture = g2dTexLoad("resources/Images/PlayerSheet2.png",G2D_SWIZZLE);
 TileSheet = g2dTexLoad("resources/Images/TileSheet.png",G2D_SWIZZLE);
}
else{
 PlayerTexture = g2dTexLoad("resources/Images/PlayerSheet3.png",G2D_SWIZZLE);
 TileSheet = g2dTexLoad("resources/Images/TileSheet2.png",G2D_SWIZZLE);
}

g2dTexture* GradientTexture = g2dTexLoad("resources/Images/Gradient.png",G2D_SWIZZLE);

g2dTexture* Menus = NULL;
if(Language == 0){
    Menus = g2dTexLoad("resources/Images/Menus1.png",G2D_SWIZZLE);
}
else{
    Menus = g2dTexLoad("resources/Images/Menus2.png",G2D_SWIZZLE);
}
g2dTexture* CurrentBackGround = NULL;//g2dTexLoad("resources/Images/BackGround.png",G2D_SWIZZLE);



// Sound ClickEffect = LoadSound("resources/Sounds/click.wav");
// Sound FlagEffect = LoadSound("resources/Sounds/Flag2.wav");
// Sound StepEffect = LoadSound("resources/Sounds/Step.wav");
//--------------------------------------------------------------------------------------

//InitalizeTheCamera
//--------------------------------------------------------------------------------------

float gamezoom = 1;
float zoomModifier;
//95
bool Grounded = false;
int Direction = 2;
int DetectedCharacter;
int LastDetectedCharacter;
int ApplyGravity = 1;
int dy;
int DeathCount = 0;

int WaitingToStart = 1;
int WaitUntilAutomaticPlay = 300;

int running = 1;

int coins = 0;
int coinsForLives = 0;
 int FromDemoToGameMode = 0;

int JumpDelay;

float TIME;
Automaticplay = 1;
 
int PlayerXOnOverWorld = 1;
int PlayerYOnOverWorld = 2;


int PlayerOverWorldxCrop = 92;
int PlayerOverWorldyCrop = 108;


int OnMapSelector = 0;
//int OverWorldDirection = 0;
int OverWorldPlayerAnimStateH = 0;
int OverWorldPlayerAnimStateV = 0;

int OverWorldPlayerDirection = 3;
int OverWorldPlayerAnimFrame;
int OverWorldPlayerAnimState;
bool OverWorldMoving = false;

int OverWorldDestinationX;
int OverWorldDestinationY;

int PlayerOverWorldxRender = (PlayerXOnOverWorld * 16) -1;
int PlayerOverWorldyRender = (PlayerYOnOverWorld * 16) -23;

int AlreadyLoadedOverWordlsPalyerPos = 0;

g2dSetGlobalScale(1);
Automaticplay = 0;
Vector2 BlockedBlock = {0,0};
Vector2 BlockedBlock2 = {0,0};
CleanEnemyArray();
//--------------------------------------------------------------------------------------
       // PlaySound(ClickEffect);
MousePos.Tile = 'A';
MousePos.TypeOfTileBehavior = 4;
//GameState = 10;
int SequenceCounter = 0;
int PlayedSequence1 = 0;

//New variables
int RepeatCountVertical;
int RepeatCountHorizontal;

bool OnSquareSelector = false;

//
int SquareSelectorX = 0;
int SquareSelectorY = 0;
//
int Message;
int MessageCounter = 0;
int MessageCounterMax = 120;
int RepeatMax = 8;//Convert to defined variable
int LastXPOS = MousePos.x;
int LastYPOS = MousePos.y;
bool filltobottom = false;

int XPressed;
DeveloperMode = false;
CurrentLevel = 1;

int LeaveToEditor;

int MenuSequenceCounter;
int MenuMultiplier;

int GotToEndSequence = 0;
int PlayedEndSequence = 0;

    // Main game loop
    while (running == 1)    
    {
//GetTime
        //Check is level editor is avtive
        //--------------------------------------------------------------------------------------
  clock_t t; 
    t = clock();
//Time
//sceKernelSysClock2USec
//MicroSeconds1 = time.microseconds ;

PLAYERX = (int)(CamX + 240) / 20;
XMax = (int)(PLAYERX) + 14;
XMin = (int)(PLAYERX) - 14;

if(XMin < 0 ) {
XMin = 0;
}else if(XMax > MapWidth ) {
XMax = MapWidth;
}

PLAYERY = (int)(CamY + 136) / 20;
YMax = (int)(PLAYERY) + 10;
YMin = (int)(PLAYERY) - 10;

if(YMin < 0 ) {
YMin = 0;
}
else if(YMax > MapHeight ) {
YMax = MapHeight;
}



UpdateControls();
UpdateAudioSystem();
if(DeveloperMode == false){
    
//Level editor is unactive
if(Automaticplay == 1 && GameState != 5){
   TimeUntillDemo = WaitTillDemoMode;
    sceCtrlPeekBufferPositive(&pad,1);
    if (pad.Buttons & PSP_CTRL_START) {
         lives = 3;
         coins = 0;
         coinsForLives = 0;
        
        LoadMap(0);
      //  Map2ToMap1();
       FromDemoToGameMode = 1;
        Automaticplay = 0;
        GameState = 5;
        DeathCount = 175;
        WaitingToStart = 0;
        TimeUntillDemo = WaitTillDemoMode;
    }
}
//Detect the enter key
//----------------------------------------------------------------------------------
  
  
    if(Botones[START].IsPressed == 1 && GameState == 3){
        GameState = 0;
       

    }
    else if(Botones[START].IsPressed == 1 && GameState == 0){
        GameState = 3;
     Player.SpeedX = 0;
      Player.SpeedY = 0;
   
    }



// if(Botones[TRIANGLE].IsPressed == 1){
//     LoadOverWorldFromOriginal();
// }
//----------------------------------------------------------------------------------

if(GameState == 0 && OnMapSelector == 0)
{ 


if(Botones[CROSS].IsPressed == 1){

   JumpDelay = 4;
}

if(JumpDelay != 0){
  JumpDelay --;
    if(Grounded == true || CoyoteTime > 0){
   //JumpForce = DefautJumpForce;
JumpDelay = 0;
    CoyoteTime = 0;
Player.SpeedY = -(JumpForce*deltaTime);
Grounded = false;
PlayJumpSound();

    }
}

// if(Grounded == false && Botones[CROSS].IsDown != 1 && Player.SpeedY < 0){
//   Player.SpeedY = Player.SpeedY + 4*deltaTime;
// }

if(ApplyGravity == 1){
Player.SpeedY = Player.SpeedY + Gravity*deltaTime;

}

if(Botones[SQUARE].IsDown == 1 || Botones[LTRIGGER].IsDown == 1){
    MaxPlayerSpeed = PlayerSpeedRun;
    CurrentMaxSpeed = MaxSpeedRun;
    PlayerAnimInterval = 5;
}
else{

if(Grounded == true){
    
    if(Player.SpeedX < 0){
        if(Player.SpeedX > -MaxSpeedWalk){

        }
        else{
Player.SpeedX = Player.SpeedX * Deceleration;
        }

    }
    else if(Player.SpeedX > 0){
                if(Player.SpeedX < MaxSpeedWalk){
        }
        else{
Player.SpeedX = Player.SpeedX * Deceleration;
        }

    }
}

    PlayerAnimInterval = 6;
    MaxPlayerSpeed = PlayerSpeedWalk;
    CurrentMaxSpeed = MaxSpeedWalk;
}
// <pspdisplay.h>
if(Botones[LEFT].IsDown == 1 || ((pad.Lx - 128) / 24) < 0){
if(Grounded == true){
if(Player.SpeedX > -CurrentMaxSpeed){
    if(Player.SpeedX > 0){
Player.SpeedX = Player.SpeedX + ((MaxPlayerSpeed * -1) ) * aceleration2;

    }
    else{
Player.SpeedX = Player.SpeedX + ((MaxPlayerSpeed * -1) ) * aceleration;

    }

}
}
else
{
    if(Player.SpeedX > -CurrentMaxSpeed){
    Player.SpeedX = Player.SpeedX + ((MaxPlayerSpeed * -1) * deltaTime) * aceleration3;

    }
   // Player.SpeedX = ((MaxPlayerSpeed * -1) * deltaTime);
}


Direction = 3;
}
else if(Botones[RIGHT].IsDown == 1 || ((pad.Lx - 128) / 24) > 0){
    if(Grounded == true){

    if(Player.SpeedX < CurrentMaxSpeed){
        if(Player.SpeedX < 0 ){
Player.SpeedX = Player.SpeedX + (MaxPlayerSpeed) * aceleration2;
        }
          else{
Player.SpeedX = Player.SpeedX + (MaxPlayerSpeed) * aceleration;
     
          }


    }
    }
else{
           if(Player.SpeedX < CurrentMaxSpeed){
        Player.SpeedX = Player.SpeedX + (MaxPlayerSpeed*deltaTime) * aceleration3;
           }
    //    Player.SpeedX = (MaxPlayerSpeed*deltaTime) ;
    }
Direction = 2;
}
else{
    if(Player.SpeedX < 0){
        if(Player.SpeedX > -StopValue){
Player.SpeedX = 0;

        }
        else{
Player.SpeedX = Player.SpeedX * Deceleration;
        }

    }
    else if(Player.SpeedX > 0){
                if(Player.SpeedX < StopValue){
Player.SpeedX = 0;

        }
        else{
Player.SpeedX = Player.SpeedX * Deceleration;
        }

    }
}


//----------------------------------------------------------------------------------
 
//Collision detection
//----------------------------------------------------------------------------------
int CurentI = 0; 

Player.X = Player.X + PlayerCol.XOffset;
Player.Y = Player.Y + PlayerCol.YOffset;
if(true){
Grounded = false;
CoyoteTime --;

for (int i = XMin; i < XMax; i++)
        {

            CurentI = i;
        for (int e = YMin; e < YMax; e++)
        {
        Rectangle rec2 = {CurentI*20,e*20,20,20};

            if((e != BlockedBlock.y || CurentI != BlockedBlock.x) && (e != BlockedBlock2.y || CurentI != BlockedBlock2.x)){//&& AllowCollison == 1
         if(((Map2[e][CurentI] >= 82 && Map2[e][CurentI] < 107)  || (Map2[e][CurentI] >= 218 && Map2[e][CurentI] < 224)) ) 
         {
            //Player.X+PlayerCol.XOffset,Player.Y+PlayerCol.YOffset,PlayerCol.sizeX,PlayerCol.sizeY
            Rectangle rec1 = {Player.X,Player.Y+ Player.SpeedY,PlayerCol.sizeX,PlayerCol.sizeY};
         //   Rectangle rec2 = {CurentI*20,e*20,20,20};
            if(SimpleCollisonRecsDetection(rec1,rec2) == true)
             {
            //  Player.SpeedY = 0;
            if(Player.SpeedY > 0){
            Player.SpeedY = 0;
            Player.Y = GetCollisionRec(rec1,rec2).y-PlayerCol.sizeY;
            Grounded = true;
              CoyoteTime = MaxCoyote;
                }
            else if(Player.SpeedY < 0)
                {
            Player.SpeedY = 0;
            Player.Y = GetCollisionRec(rec1,rec2).y + GetCollisionRec(rec1,rec2).height;
                }

             }
            Rectangle rec3 = {Player.X+ Player.SpeedX,Player.Y,PlayerCol.sizeX,PlayerCol.sizeY};
            if(SimpleCollisonRecsDetection(rec2,rec3) == true)
             {
                //<stdio.h>
                if(Player.SpeedX > 0){
            Player.SpeedX = 0;
            Player.X = GetCollisionRec(rec3,rec2).x - PlayerCol.sizeX;
                }
                else if(Player.SpeedX < 0)
                {
            Player.SpeedX = 0;
            Player.X = GetCollisionRec(rec3,rec2).x + GetCollisionRec(rec3,rec2).width;
                }

             }
         
         }  
        if(((Map2[e][CurentI] >= 65 && Map2[e][CurentI] < 82)) && rec2.y - Player.Y > 27 && Player.SpeedY >= 0 )
         {
            if(Player.SpeedY >= 0){
            //Player.X+PlayerCol.XOffset,Player.Y+PlayerCol.YOffset,PlayerCol.sizeX,PlayerCol.sizeY
            Rectangle rec1 = {Player.X,Player.Y+ Player.SpeedY,PlayerCol.sizeX,PlayerCol.sizeY};
         //   Rectangle rec2 = {CurentI*20,e*20,20,20};
            if(SimpleCollisonRecsDetection(rec1,rec2) == true)
             {
            //  Player.SpeedY = 0;
            if(Player.SpeedY > 0){
            Player.SpeedY = 0;
            Player.Y = GetCollisionRec(rec1,rec2).y-PlayerCol.sizeY;
            Grounded = true;
           CoyoteTime = MaxCoyote;
                }
                else if(Player.SpeedY < 0)
                {
            Player.SpeedY = 0;
            Player.Y = GetCollisionRec(rec1,rec2).y + GetCollisionRec(rec1,rec2).height;
                }

             }
             Rectangle rec3 = {Player.X+ Player.SpeedX,Player.Y,PlayerCol.sizeX,PlayerCol.sizeY};
             if(SimpleCollisonRecsDetection(rec2,rec3) == true)
             {
                //<stdio.h>
                if(Player.SpeedX > 0){
            Player.SpeedX = 0;
            Player.X = GetCollisionRec(rec3,rec2).x - PlayerCol.sizeX;
                }
                else if(Player.SpeedX < 0)
                {
            Player.SpeedX = 0;
            Player.X = GetCollisionRec(rec3,rec2).x + GetCollisionRec(rec3,rec2).width;
                }

             }
            }
         }
        
                 if(((Map[e][CurentI] >= 82 && Map[e][CurentI] < 107)  || (Map[e][CurentI] >= 218 && Map[e][CurentI] < 224)) ) 
         {
            //Player.X+PlayerCol.XOffset,Player.Y+PlayerCol.YOffset,PlayerCol.sizeX,PlayerCol.sizeY
            Rectangle rec1 = {Player.X,Player.Y+ Player.SpeedY,PlayerCol.sizeX,PlayerCol.sizeY};
         //   Rectangle rec2 = {CurentI*20,e*20,20,20};
            if(SimpleCollisonRecsDetection(rec1,rec2) == true)
             {
            //  Player.SpeedY = 0;
            if(Player.SpeedY > 0){
            Player.SpeedY = 0;
            Player.Y = GetCollisionRec(rec1,rec2).y-PlayerCol.sizeY;
            Grounded = true;
              CoyoteTime = MaxCoyote;
                }
            else if(Player.SpeedY < 0)
                {
            Player.SpeedY = 0;
            Player.Y = GetCollisionRec(rec1,rec2).y + GetCollisionRec(rec1,rec2).height;
                }

             }
            Rectangle rec3 = {Player.X+ Player.SpeedX,Player.Y,PlayerCol.sizeX,PlayerCol.sizeY};
            if(SimpleCollisonRecsDetection(rec2,rec3) == true)
             {
                //<stdio.h>
                if(Player.SpeedX > 0){
            Player.SpeedX = 0;
            Player.X = GetCollisionRec(rec3,rec2).x - PlayerCol.sizeX;
                }
                else if(Player.SpeedX < 0)
                {
            Player.SpeedX = 0;
            Player.X = GetCollisionRec(rec3,rec2).x + GetCollisionRec(rec3,rec2).width;
                }

             }
         
         }  
        if(((Map[e][CurentI] >= 65 && Map[e][CurentI] < 82)) && rec2.y - Player.Y > 27 && Player.SpeedY >= 0 )
         {
            if(Player.SpeedY >= 0){
            //Player.X+PlayerCol.XOffset,Player.Y+PlayerCol.YOffset,PlayerCol.sizeX,PlayerCol.sizeY
            Rectangle rec1 = {Player.X,Player.Y+ Player.SpeedY,PlayerCol.sizeX,PlayerCol.sizeY};
         //   Rectangle rec2 = {CurentI*20,e*20,20,20};
            if(SimpleCollisonRecsDetection(rec1,rec2) == true)
             {
            //  Player.SpeedY = 0;
            if(Player.SpeedY > 0){
            Player.SpeedY = 0;
            Player.Y = GetCollisionRec(rec1,rec2).y-PlayerCol.sizeY;
            Grounded = true;
           CoyoteTime = MaxCoyote;
                }
                else if(Player.SpeedY < 0)
                {
            Player.SpeedY = 0;
            Player.Y = GetCollisionRec(rec1,rec2).y + GetCollisionRec(rec1,rec2).height;
                }

             }
             Rectangle rec3 = {Player.X+ Player.SpeedX,Player.Y,PlayerCol.sizeX,PlayerCol.sizeY};
             if(SimpleCollisonRecsDetection(rec2,rec3) == true)
             {
                //<stdio.h>
                if(Player.SpeedX > 0){
            Player.SpeedX = 0;
            Player.X = GetCollisionRec(rec3,rec2).x - PlayerCol.sizeX;
                }
                else if(Player.SpeedX < 0)
                {
            Player.SpeedX = 0;
            Player.X = GetCollisionRec(rec3,rec2).x + GetCollisionRec(rec3,rec2).width;
                }

             }
            }
         }
        
        
        }
       
       
        if(Map2[e][CurentI] == 207 || Map[e][CurentI] == 207){
                Rectangle rec1 = {Player.X,Player.Y,PlayerCol.sizeX,PlayerCol.sizeY};
           // Rectangle rec2 = {CurentI*20,e*20,20,20};
              if(SimpleCollisonRecsDetection(rec1,rec2) == true){
              GameState = 5;
              CurrentPlayerAnimStateH = 2;
              CurrentPlayerAnimStateV = 0;
               lives -- ;

              }
         }
        else if(Map2[e][CurentI] == 208 || Map[e][CurentI] == 208){
              Rectangle rec1 = {Player.X,Player.Y,PlayerCol.sizeX,PlayerCol.sizeY};
              Rectangle rec2 = {CurentI*20,e*20+17,20,3};
              if(SimpleCollisonRecsDetection(rec1,rec2) == true){
              //ResetPlayerAndFlagPos();
              GameState = 5;
              CurrentPlayerAnimStateH = 2;
              CurrentPlayerAnimStateV = 0;
               lives -- ;
              }
         }
        else if(Map2[e][CurentI] == 209 || Map[e][CurentI] == 209){
              Rectangle rec1 = {Player.X,Player.Y,PlayerCol.sizeX,PlayerCol.sizeY};
              Rectangle rec2 = {CurentI*20,(e*20),20,2};
              if(SimpleCollisonRecsDetection(rec1,rec2) == true){
              //ResetPlayerAndFlagPos();
              GameState = 5;
              lives -- ;
              CurrentPlayerAnimStateH = 2;
              CurrentPlayerAnimStateV = 0;
              }
         }
        if( Map2[e][CurentI] == 224 || Map[e][CurentI] == 224){
        Rectangle rec1 = {Player.X,Player.Y+ Player.SpeedY,PlayerCol.sizeX,PlayerCol.sizeY};
         //   Rectangle rec2 = {CurentI*20,e*20,20,20};
            if(SimpleCollisonRecsDetection(rec1,rec2) == true)
             {
        SpawnPos = (Vector2){rec2.x, rec2.y};
             }
      }
       

        if(Map2[e][CurentI] == 226 || Map[e][CurentI] == 226) 
         {
            //Player.X+PlayerCol.XOffset,Player.Y+PlayerCol.YOffset,PlayerCol.sizeX,PlayerCol.sizeY
            Rectangle rec1 = {CurentI*20,e*20,20,10};
         //   Rectangle rec2 = {CurentI*20,e*20,20,20};

            Rectangle rec3 = {Player.X+ Player.SpeedX,Player.Y,PlayerCol.sizeX,PlayerCol.sizeY};
            if(SimpleCollisonRecsDetection(rec1,rec3) == true)
             {
                //<stdio.h>
                if(Player.SpeedX > 0){
            Player.SpeedX = 0;
            Player.X = GetCollisionRec(rec3,rec2).x - PlayerCol.sizeX;
                }
                else if(Player.SpeedX < 0)
                {
            Player.SpeedX = 0;
            Player.X = GetCollisionRec(rec3,rec2).x + GetCollisionRec(rec3,rec2).width;
                }

             }
         
         }  
      
        }
        }
     }

Player.X = Player.X - PlayerCol.XOffset;
Player.Y = Player.Y - PlayerCol.YOffset;



//"Raycast" manager
//----------------------------------------------------------------------------------
int X;
int Y;

for (int i = XMin; i < XMax; i++)
        {
            CurentI = i;
        for (int e = YMin; e < YMax; e++)
        {//Map2[e][CurentI] >= 107 && Map2[e][CurentI] < 131
            //if(IsRamp(Map2[e][CurentI]) == 1){

          
Rectangle rec2 = {CurentI*20,e*20,20,20};



Rectangle rec1 = {(Player.X+12)+Player.SpeedX,Player.Y+PlayerTextureControl.DestHeigt-2,1,1};

//Rectangle rec2 = {CurentI*20,e*20,20,20};
if(SimpleCollisonRecsDetection(rec1,rec2)){

DetectedCharacter = Map2[e][CurentI];
if(Player.SpeedY >= 0){


if(DetectedCharacter >= 119 && DetectedCharacter < 125)
{

X = rec1.x - rec2.x;
Y = rec1.y - rec2.y;

X++;
Y++;
//<time.h>

if(X + Y > 16 || LastDetectedCharacter >= 65 && LastDetectedCharacter < 107){
Grounded = true;
Player.SpeedY = 0;
Player.Y = Player.Y - ((X-1) - (20 - Y));
BlockedBlock.x = (rec2.x / 20)+1;
BlockedBlock.y = (rec2.y / 20);
}
     
if(Player.SpeedX < 0){
Grounded = true;
}

}
else{
BlockedBlock.x = 0; BlockedBlock.y = 0;

}

if(DetectedCharacter >= 125 && DetectedCharacter < 131)
{


int X2 = rec1.x - rec2.x;
int Y2 = rec1.y - rec2.y;

X2++;
Y2++;
//<time.h>


if(Player.SpeedY >  0){
Grounded = true;
Player.SpeedY = 0;
Player.Y = Player.Y - (((X2) - (20 - Y2))+20);



}
      
        }




if(DetectedCharacter == '!')
{
if(Player.SpeedX < 0 || LastDetectedCharacter >= 65 && LastDetectedCharacter < 107){

    rec2.y = (e+1)*20;
   //   if(rec1.y - rec2.y < 0 ){

//Grounded = true;
 if(Player.SpeedY <= 3){

X = rec1.x - rec2.x;
Y = rec1.y - rec2.y;

X++;
Y++;
Player.SpeedY = 0;
Player.Y = (Player.Y - ((X-1) - (19 - Y)));

    }

   
   
  //   }
//
     
}

      
        }




if(DetectedCharacter >= 107 && DetectedCharacter < 113)
{

X = rec1.x - rec2.x;
Y = rec1.y - rec2.y;

X++;
Y++;
//<time.h>
X = 20 - X;

if(X + Y > 14 || LastDetectedCharacter >= 65 && LastDetectedCharacter < 107){
Grounded = true;
Player.SpeedY = 0;
Player.Y = Player.Y - ((X-1) - (20 - Y)) - 1;
BlockedBlock2.x = (rec2.x / 20)-1;
BlockedBlock2.y = (rec2.y / 20);
}
     
if(Player.SpeedX < 0){
Grounded = true;
}

}
else{
BlockedBlock2.x = 0; BlockedBlock2.y = 0;

}

if(DetectedCharacter >= 113 && DetectedCharacter < 119)
{


int X2 = rec1.x - rec2.x;
int Y2 = rec1.y - rec2.y;

X2++;
Y2++;
//<time.h>

X2 = 20 - X2;
if(Player.SpeedY >  0){
Grounded = true;
Player.SpeedY = 0;
Player.Y = Player.Y - (((X2) - (20 - Y2))+20);



}
      
        }



if(DetectedCharacter == '1')
{
if(Player.SpeedX > 0 && Grounded == false){

   rec2.y = (e+1)*20;
   if(rec1.y - rec2.y < 4 || Player.SpeedX > 0){
Grounded = true;
 if( rec1.x - rec2.x < 2 || Player.SpeedX > 2){

if(Player.SpeedY <= 3){

X = rec1.x - rec2.x;
Y = rec1.y - rec2.y;

X++;
Y++;

X = 20 - X;



Player.Y = Player.Y - ((X-1) - (20 - Y)) + 20;
}

    }

   }
   
     
}

}





LastDetectedCharacter = Map[e][CurentI];
}

}





    if(Map2[e][CurentI] == 206){
    Rectangle rec1 = {Player.X+2,Player.Y+2,19,39};
        //    Rectangle rec2 = {CurentI*20,e*20,20,20};
            if(SimpleCollisonRecsDetection(rec1,rec2) == true){
            coins ++;
            coinsForLives ++;
            Map2[e][CurentI] = '.';
            if(coinsForLives >= CoinsUntilNewLife){
              coinsForLives = 0;
           //   PlayClickSound();
              lives++;
            }
else{
   // PlayCoinSound();
}

          
              }
         } 

    if(Map[e][CurentI] == 206){
    Rectangle rec1 = {Player.X+2,Player.Y+2,19,39};
        //    Rectangle rec2 = {CurentI*20,e*20,20,20};
            if(SimpleCollisonRecsDetection(rec1,rec2) == true){
            coins ++;
            coinsForLives ++;
            Map[e][CurentI] = '.';
            if(coinsForLives >= CoinsUntilNewLife){
              coinsForLives = 0;
           //   PlayClickSound();
              lives++;
            }
else{
   // PlayCoinSound();
}

          
              }
         } 



        //   }
        }
        }
//----------------------------------------------------------------------------------

for (int i = 0; i < Enemies; i++)
{

int currentI = i;
   if(Enemies1[i].X != -20 && Enemies1[i].X != 0){

   
if(Enemies1[i].Type != 2){
if((Player.SpeedY <= 0 ) && Enemies1[i].SpeedX != 0 &&  Enemies1[i].Anim == 0 ){
   Rectangle rec2;
    if(Enemies1[i].Type == 0){
rec2 = (Rectangle){Enemies1[i].X+4,Enemies1[i].Y+4,12,1}; 
    }
    else{
        rec2 = (Rectangle){Enemies1[i].X,Enemies1[i].Y,12,1}; 
    }
    //  Rectangle rec2 = {Enemies1[i].X+4,Enemies1[i].Y+4,12,16}; 
      Rectangle rec1 = {Player.X+PlayerCol.XOffset,Player.Y+PlayerCol.YOffset,PlayerCol.sizeX,PlayerCol.sizeY};
      if(SimpleCollisonRecsDetection(rec1,rec2) == true &&  Enemies1[i].Anim == 0){
      GameState = 5;
      lives --;
      CurrentPlayerAnimStateH = 2;
      CurrentPlayerAnimStateV = 0;
      }
}
else {
    Rectangle rec2 = {Enemies1[i].X-11,Enemies1[i].Y,42,9}; 
    Rectangle rec1 = {Player.X,Player.Y,23,41};
    if(SimpleCollisonRecsDetection(rec1,rec2) == true &&  Enemies1[i].Anim == 0){
              //ResetPlayerAndFlagPos();
              //<stdio.h>
              
                 Enemies1[i].SpeedX = 0;
                 Enemies1[i].SpeedY = 1;
                 Enemies1[i].Anim = 1;
                Player.SpeedY = -(75*deltaTime);
                Grounded = false;
                  i = Enemies;
              }


}
 


if(Enemies1[i].SpeedY != 0){
    Enemies1[i].SpeedY ++;
    if(Enemies1[i].SpeedY >= 25){
Enemies1[i].X = -20;
Enemies1[i].Y = -20;
    }
}
    for (int c = 0; c < EnemiesBlockers; c++)
 {
    if(SimpleCollisonRecsDetection((Rectangle){Enemies1[i].X,Enemies1[i].Y,20,20},(Rectangle){EnemyBlockers[c].x,EnemyBlockers[c].y,20,20}) == true){
         Enemies1[currentI].SpeedX = Enemies1[currentI].SpeedX * -1;
    }
            
             


 }


Enemies1[i].X =  Enemies1[i].X + Enemies1[i].SpeedX;
 
}
else{
    //Anim = 0 (Grounded)
    //SpeedX = TimeBetween frames
if(Enemies1[i].Anim == 0 || Enemies1[i].Anim == 2){
        Enemies1[i].SpeedX ++;
        if(Enemies1[i].SpeedX == 100){
            Enemies1[i].Anim = 2;
        }
        if(Enemies1[i].SpeedX == 120){
            Enemies1[i].SpeedX = 0;
            Enemies1[i].SpeedY = -(75*deltaTime);
            Enemies1[i].Anim = 1;
            Enemies1[i].Y --;
        }
    }

if(Enemies1[i].Anim == 1){
 Enemies1[i].SpeedY = Enemies1[i].SpeedY + 2.2*deltaTime;
 if((Enemies1[i].Y) >= Enemies1[i].OriginalY){
    Enemies1[i].Y = Enemies1[i].OriginalY;
    Enemies1[i].Anim = 0;
    Enemies1[i].SpeedY = 0;
 }
}

Enemies1[i].Y =  Enemies1[i].Y + Enemies1[i].SpeedY;


Rectangle rec2 = (Rectangle){Enemies1[i].X+4,Enemies1[i].Y+4,12,16}; 
      Rectangle rec1 = {Player.X,Player.Y,23,41};
      if(SimpleCollisonRecsDetection(rec1,rec2) == true){
      GameState = 5;
      lives --;
      CurrentPlayerAnimStateH = 2;
      CurrentPlayerAnimStateV = 0;
      }

}
}
}


for(int i = 0; i < AmountOfTrampolines; i++){

Rectangle rec1 = {Player.X+Player.SpeedX,Player.Y+Player.SpeedY,23,41};
Rectangle rec2 = {Trampolines[i].X,Trampolines[i].Y,20,20};

  if(SimpleCollisonRecsDetection(rec1,rec2) == true && Player.SpeedY > 0){
    Trampolines[i].Anim = 1;
                Player.SpeedY = -(110*deltaTime);
                Grounded = false;
  }
  if(Trampolines[i].Anim != 0)
  {
    Trampolines[i].AnimFrame ++;

    if(Trampolines[i].AnimFrame == 2 && Trampolines[i].Anim == 1){
        Trampolines[i].AnimFrame = 0;
        Trampolines[i].Anim = 2;
    }
    else if (Trampolines[i].AnimFrame == 12){
Trampolines[i].AnimFrame = 0;
Trampolines[i].Anim = 0;
    }

  }
}


if(EnemiesAnimCount < 10){
    EnemiesAnimCount++;
}
else{
    EnemiesAnimCount = 0;
if(EnemiesAnim == 0){
    EnemiesAnim = 1;

}else{
    EnemiesAnim = 0;
}

}





}






//Flag manager
//----------------------------------------------------------------------------------
Rectangle rec1 = {Player.X,Player.Y,23,40};
Rectangle rec2 = {Flag.X,Flag.Y,20,20};
if(SimpleCollisonRecsDetection(rec1,rec2)){

Flag.X = Flag.X + 500;
Flag.Y = Flag.Y + 500;

GameState = 1;
Player.SpeedX = 0;
Player.SpeedY = 0;
Gradient.X = 365;//Player.X+120;
Gradient.Y = Player.Y-50;
  SaveData();
    SaveCoinsAndLives(coins,lives,PlayedSequence1,PlayedEndSequence);
if(CurrentLevel == 31 && PlayedEndSequence == 0){
GotToEndSequence = 1;
}
 // SaveData();
  //  SaveCoinsAndLives(coins,lives,PlayedSequence1,PlayedEndSequence);
 //   GameState = 0;
 //   SelectedButton = 0;

}
//----------------------------------------------------------------------------------



//Transition Management
//----------------------------------------------------------------------------------
if(GameState == 1){
 GradientINT ++;
 Gradient.X = Gradient.X - 5;
    if(GradientINT > 130) {
    GameState = 2;
GradientINT = 0;
Gradient.Y = Player.Y-50;
    }
}
if(GameState == 2){
    Grounded = true;
    
    Gradient.X = Gradient.X - 5;
  if(GradientINT == 0){
//<stdio.h>
CurrentPlayerAnimStateH = 0;
CurrentPlayerAnimStateV = 2;
    if(CurrentLevel != MaxLevels){
        if(LeaveToEditor == 0){
            if(GotToEndSequence == 0){

         SpawnPos = (Vector2){-20,-20};
      //  LoadMap(CurrentLevel);
      //  Map2ToMap1();
      //  ResetPlayerAndFlagPos();
     //   CurrentLevel++;
     OnMapSelector = 1;
     
OverWorldDestinationY = PlayerYOnOverWorld  ;
OverWorldDestinationX = PlayerXOnOverWorld  ;
            }
     else
    {
        GameState = 11;
        SequenceCounter = 0;
        CurrentBackGround = NULL;
    }
        }
else{
    LeaveToEditor = 0;
    DeveloperMode = true;
    GameState = 9;
    CurrentBackGround = NULL;

    GradientINT = 0;
    Gradient.Y = Player.Y-200;
}
    }
    else{
      //   SpawnPos = (Vector2){-20,-20};
      //  CurrentLevel = 0;
      //  LoadMap(CurrentLevel);
      //  CurrentLevel++;
      //          Map2ToMap1();
      //  ResetPlayerAndFlagPos();
     OnMapSelector = 1;

    }
 Gradient.Y = Player.Y-50;
  }
 GradientINT ++;

if(GradientINT > 122) {
  GameState = 0;
  OnMapSelector = 1;
  
OverWorldDestinationY = PlayerYOnOverWorld  ;
OverWorldDestinationX = PlayerXOnOverWorld  ;
  if(NextPlayableLevel == CurrentLevel)
  {
OverWorldSquares[PlayerXOnOverWorld][PlayerYOnOverWorld].State = 'S';
NextPlayableLevel ++;
  }
  GradientINT = 0;
    }
}

if(GameState == 5){
DeathCount++;
// <time.h>
if(DeathCount < 20){
    Player.Y = Player.Y - 5;
}
else if (DeathCount < 80){
    Player.Y = Player.Y + 5;
}
else if(DeathCount == 200){
    CurrentPlayerAnimStateH = 0;
    CurrentPlayerAnimStateV = 2;

                GameState = 0;


    if(Automaticplay == 1){
        SpawnPos = (Vector2){-20,-20};
        LoadMap(0);
        Map2ToMap1();
        Player.SpeedX = 0;
        Player.SpeedY = 0;
    //  Automaticplay = 0;
        CurrentDuration = 0;
        Frame = 0;
        coins = 0;
        coinsForLives = 0;
        ResetPlayerAndFlagPos();
        Player.Y = Player.Y - 40;
        UpdateControls();
    }
    else{
     if(FromDemoToGameMode == 0){
        if(lives <= 0){
        SpawnPos = (Vector2){-20,-20};
        CurrentLevel = 0;
        Player.SpeedX = 0;
        Player.SpeedY = 0;
        OnMapSelector = 1;
     
OverWorldDestinationY = PlayerYOnOverWorld  ;
OverWorldDestinationX = PlayerXOnOverWorld  ;
        GameState = 0;
        lives = 3;
        // PlayerXOnOverWorld = 1;
        // PlayerYOnOverWorld = 2;
        // OverWorldDestinationX = 0;
        // OverWorldDestinationY = 0;
        // OverWorldMoving = 0;
        // PlayerOverWorldxRender = (PlayerXOnOverWorld * 16) -1;
        // PlayerOverWorldyRender = (PlayerYOnOverWorld * 16) -23;
        //PROBLEM WITH 
        WaitingToStart = 0; 
        CurrentDuration = 0;
        Frame = 0;
        ResetPlayerAndFlagPos();
        Player.Y = Player.Y - 40;
        UpdateControls();
        }
        
         //  lives = lives - 1;
   
   
     }
    }
    Player.SpeedX = 0;
    Player.SpeedY = 0;
    ResetPlayerAndFlagPos();

    DeathCount = 0;
    if(FromDemoToGameMode == 1){
        FromDemoToGameMode = 0;
    }
}
}
//----------------------------------------------------------------------------------


//Speed and camera update
if(GameState != 5){

Player.X = Player.X + Player.SpeedX;
Player.Y = Player.Y + Player.SpeedY;
CamX = (int)((Player.X)-240);

CamY = ((Player.Y)-116);
//cam.target = (Vector2){Player.X + (23 / 2),Player.Y + (41/2)};
}
//----------------------------------------------------------------------------------

//Animation
//----------------------------------------------------------------------------------//<time.h>
if(GameState != 5){

if(Player.SpeedX > 0 && Grounded == true){

    if(CurrentPlayerAnimStateV != 2){
    CurrentPlayerAnimStateV = 2;
PlayerAnimCount = 0;
    }
    if(PlayerAnimCount < PlayerAnimInterval){
        PlayerAnimCount++;
    }
    else
    {
    if(CurrentPlayerAnimStateH == 0 || CurrentPlayerAnimStateH == 3){
   
}
        if(CurrentPlayerAnimStateH + 1 != 4){
            CurrentPlayerAnimStateH++;
        }
        else{
            CurrentPlayerAnimStateH = 0;
        }
        PlayerAnimCount = 0;
    }
    
}
else if(Player.SpeedX < 0 && Grounded == true){
   
    if(CurrentPlayerAnimStateV != 3){
    CurrentPlayerAnimStateV = 3;
PlayerAnimCount = 0;
    }
    if(PlayerAnimCount < PlayerAnimInterval){
        PlayerAnimCount++;
    }
    else
    {
               if(CurrentPlayerAnimStateH == 0 || CurrentPlayerAnimStateH == 3){
    
}
        if(CurrentPlayerAnimStateH + 1 != 4){
            CurrentPlayerAnimStateH++;
        }
        else{
            CurrentPlayerAnimStateH = 0;
        }
        PlayerAnimCount = 0;
    }
    
}
else{
    CurrentPlayerAnimStateH = 0;
    CurrentPlayerAnimStateV = Direction;

}

if(Player.SpeedX < 0 && Direction == 2){
    CurrentPlayerAnimStateH = 0;
    CurrentPlayerAnimStateV = 1;
}
else if(Player.SpeedX > 0 && Direction == 3){
      CurrentPlayerAnimStateH = 1;
    CurrentPlayerAnimStateV = 1;
}

if(Grounded == false){
     CurrentPlayerAnimStateV = 0;
     if(Direction == 3){
     CurrentPlayerAnimStateH = 1;

     }
     else{
     CurrentPlayerAnimStateH = 0;

     }
}

}
//----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
     

    //       0x49c9FFFF   Dessert
if(CurrentLevel <= 31){
         g2dClear(BackGroundColors[BackGroundsID[CurrentLevel]]);
}
else{
         g2dClear(BackGroundColors[0]);

}





if(Player.X  - (480 / 2) < 0){
CamX =  0;
}



if(GameState != 4 && GameState != 6){
       
        //Draw the flag 
        //--------------------------------------------------------------------------------------
        //DrawTexturePro(TileSheet, (Rectangle){380,100,20,20},(Rectangle){Flag.X,Flag.Y,20,20},(Vector2){0,0},0,WHITE);
        //--------------------------------------------------------------------------------------

        //Draw the squares
        //--------------------------------------------------------------------------------------
        int CurrentI = 0;
        for (int i = XMin; i < XMax; i++)
        {

            CurrentI = i;
        for (int e = YMin; e < YMax; e++)
        {
//dfsad

            if(Map2[e][CurrentI] > 64  && Map2[e][CurrentI] != 207 && Map2[e][CurrentI] != 215 && Map2[e][CurrentI] != 216 && Map2[e][CurrentI] != 226 && Map2[e][CurrentI] != 229 && Map2[e][CurrentI] != 232 )
            {
    int Y;
    int X = Map2[e][CurrentI]-65;
    
    Y = X / 25;


    X = (X) - (Y * 25);

    g2dBeginRects(TileSheet);
    g2dResetCrop();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(20,20);
    g2dSetCoordXY((int)((CurrentI * 20)-CamX),(int)((e*20)-CamY));
    g2dSetCropWH(20,20);
    g2dSetCropXY(X*20,Y*20);
    g2dAdd();
    g2dEnd();
}
  
  
  
            if(Map[e][CurrentI] > 64 && Map[e][CurrentI] != 207 && Map[e][CurrentI] != 215 && Map[e][CurrentI] != 216 && Map[e][CurrentI] != 226 && Map[e][CurrentI] != 229 && Map[e][CurrentI] != 232)
            {
            int Y;
            int X = Map[e][CurrentI]-65;
            Y = X / 25;
            X = (X) - (Y * 25);

            g2dBeginRects(TileSheet);
            g2dResetCrop();
            g2dSetTexLinear(false);
            g2dSetCoordMode(G2D_UP_LEFT);
            g2dSetAlpha(255);
            g2dSetScaleWH(20,20);
            g2dSetCoordXY((int)((CurrentI * 20)-CamX),(int)((e*20)-CamY));
            g2dSetCropWH(20,20);
            g2dSetCropXY(X*20,Y*20);
            g2dAdd();
            g2dEnd();
            }


if(Map2[e][CurrentI] == '$' && Map[e][CurrentI] == '.'){
            g2dBeginRects(TileSheet);
            g2dResetCrop();
            g2dSetTexLinear(false);
            g2dSetCoordMode(G2D_UP_LEFT);
            g2dSetAlpha(255);
            g2dSetScaleWH(20,20);
            g2dSetCoordXY((int)((CurrentI * 20)-CamX),(int)((e*20)-CamY));
            g2dSetCropWH(20,20);
            g2dSetCropXY(380,140);
            g2dAdd();
            g2dEnd();
  }
        }
        }


for(int i = 0; i < AmountOfTrampolines; i++){
            g2dBeginRects(TileSheet);
            g2dResetCrop();
            g2dSetTexLinear(false);
            g2dSetCoordMode(G2D_UP_LEFT);
            g2dSetAlpha(255);
            g2dSetScaleWH(20,20);
            g2dSetCoordXY((int)((Trampolines[i].X)-CamX),(int)((Trampolines[i].Y)-CamY));
            g2dSetCropWH(20,20);
            g2dSetCropXY(220+20*Trampolines[i].Anim,120);
            g2dAdd();
            g2dEnd();
}


            // g2dBeginRects(NULL);
            // g2dResetCrop();
            // g2dSetTexLinear(false);
            // g2dSetCoordMode(G2D_UP_LEFT);
            // g2dSetAlpha(255);
            // g2dSetScaleWH(20,20);
            // g2dSetCoordXY((int)((BlockedBlock.x * 20)-CamX),(int)((BlockedBlock.y * 20)-CamY));
            // g2dSetColor(BLACK);
            // g2dAdd();
            // g2dEnd();
            //             g2dBeginRects(NULL);
            // g2dResetCrop();
            // g2dSetTexLinear(false);
            // g2dSetCoordMode(G2D_UP_LEFT);
            // g2dSetAlpha(255);
            // g2dSetScaleWH(20,20);
            // g2dSetCoordXY((int)((BlockedBlock2.x * 20)-CamX),(int)((BlockedBlock2.y * 20)-CamY));
            // g2dSetColor(BLACK);
            // g2dAdd();
            // g2dEnd();

if(SpawnPos.x != -20){
    g2dBeginRects(TileSheet);
    g2dResetCrop();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(20,20);
    g2dSetCoordXY((int)(SpawnPos.x-CamX),(int)((SpawnPos.y)-CamY));
    
    g2dSetCropWH(20,20);
    
    g2dSetCropXY(200,120);

    g2dAdd();
    g2dEnd();
}


        //--------------------------------------------------------------------------------------
        

for (int i = 0; i < Enemies; i++)
{
    if(Enemies1[i].X != - 20 && Enemies1[i].X != 0){

    g2dBeginRects(TileSheet);
    g2dResetCrop();
    //g2dSetTexRepeat(true);
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(20,20);
    g2dSetCoordXY((int)(Enemies1[i].X-CamX),(int)(Enemies1[i].Y-CamY));
    g2dSetCropWH(20,20);
       
    if(Enemies1[i].Type == 0){
        if(Enemies1[i].Anim == 0){ 
    g2dSetCropXY(20+20*EnemiesAnim,120);
     }
        else{
             g2dSetCropXY(360,140);
        }
    }
    else if(Enemies1[i].Type == 1){
        if(Enemies1[i].Anim == 0){
        g2dSetCropXY(340+20*EnemiesAnim,120);

        }
        else{
             g2dSetCropXY(380,120);
        }
    }
    else if(Enemies1[i].Type == 2){
        g2dSetCropXY(280+20*Enemies1[i].Anim,120);
    }
    g2dAdd();
    g2dEnd();
    }

  //  DrawTexturePro(TileSheet, (Rectangle){20+20*EnemiesAnim,120,20,20},(Rectangle){Enemies1[i].X,Enemies1[i].Y,20,20 },(Vector2){0,0},0,CLITERAL(Color){ 255, 255, 255, 255 });
}


    //Draw the player   
        //--------------------------------------------------------------------------------------
    g2dBeginRects(PlayerTexture);
    g2dResetCrop();
    g2dReset();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(23,41);
    g2dSetCoordXY((int)Player.X-CamX ,(int)(Player.Y-CamY));
    g2dSetCropWH(23,41);
    g2dSetCropXY(CurrentPlayerAnimStateH*23,CurrentPlayerAnimStateV*41);
    g2dAdd();
    g2dEnd();
    // g2dBeginRects(NULL);
    // g2dResetCrop();
    // g2dReset();
    // g2dSetColor(RED);
    // g2dSetTexLinear(false);
    // g2dSetCoordMode(G2D_UP_LEFT);
    // g2dSetAlpha(255);
    // g2dSetScaleWH(PlayerCol.sizeX,PlayerCol.sizeY);
    // g2dSetCoordXY(((int)Player.X + PlayerCol.XOffset)-CamX ,(int)((Player.Y+ PlayerCol.YOffset)-CamY));
    // g2dSetCropWH(23,41);
    // g2dSetCropXY(CurrentPlayerAnimStateH*23,CurrentPlayerAnimStateV*41);
    // g2dAdd();
    // g2dEnd();

//To do: Draw score
//Done: Draw coins and lives

    g2dBeginRects(PlayerTexture);
    g2dResetCrop();
    g2dReset();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(33,22);
    g2dSetCoordXY(5,5);
    g2dSetCropWH(33,22);
    g2dSetCropXY(92,75);
    g2dAdd();
    g2dEnd();
int lives2;
if(GameState == 5){
 lives2 = lives+1;
}
else{
    
 lives2 = lives;// + 1;
}
    for (int i = 0; i < 4; i++)
    {
    g2dBeginRects(PlayerTexture);
    g2dResetCrop();
    g2dReset();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(8,11);
    g2dSetCoordXY(70-9*i,13);
    g2dSetCropWH(8,11);
    if(lives > 0){
    g2dSetCropXY((8 * (lives2  % 10)),164);
    }
    else{
        g2dSetCropXY(0,164);
    }
    g2dAdd();
    g2dEnd();
    lives2 = lives2 / 10;
  //Power = Power * 10;
    }


//Coins
    g2dBeginRects(PlayerTexture);
    g2dResetCrop();
    g2dReset();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(29,11);
    g2dSetCoordXY(8,34);
    g2dSetCropWH(29,11);
    g2dSetCropXY(95,98);
    g2dAdd();
    g2dEnd();
int coins2 = coins;
    for (int i = 0; i < 4; i++)
    {
    g2dBeginRects(PlayerTexture);
    g2dResetCrop();
    g2dReset();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(8,11);
    g2dSetCoordXY(70-9*i,34);
    g2dSetCropWH(8,11);
    g2dSetCropXY((8 * (coins2 % 10)),164);
    g2dAdd();
    g2dEnd();
    coins2 = coins2 / 10;
  //Power = Power * 10;
    }
    

//



        //--------------------------------------------------------------------------------------
if(WaitingToStart == 1){

    g2dBeginRects(Menus);
    g2dResetCrop();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(128,14);
    g2dSetCropXY(0,14);
    g2dSetCoordXY( 176 , 200);
    g2dSetColor(BLACK); //272        
    g2dSetCropWH(128,14);//128 64
    g2dAdd();
    g2dEnd();

}






if(DeathCount > 90 && GameState == 5){
    g2dBeginRects(NULL);
    g2dResetCrop();
    g2dReset();
    g2dSetColor(BLACK);
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(480,272);
    g2dSetCoordXY(0,0);
    g2dAdd();
    g2dEnd();


    if(Automaticplay == 0 && WaitingToStart == 0){
   
   if(FromDemoToGameMode == 0){
    if(lives <= 0){
    coins = 0;
    coinsForLives = 0;
    g2dBeginRects(Menus);
    g2dResetCrop();
    g2dReset();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetColor(WHITE);
    g2dSetScaleWH(104,14);
    g2dSetCoordXY(188,144);
    g2dSetCropWH(104,14);
    g2dSetCropXY(0,28 );
    g2dAdd();
    g2dEnd();
    }
    else{
    g2dBeginRects(PlayerTexture);
    g2dResetCrop();
    g2dReset();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(33,21);
    g2dSetCoordXY(220,136);
    g2dSetCropWH(33,21);
    g2dSetCropXY(92,75);
    g2dAdd();
    g2dEnd();
    
int lives2;

 lives2 = lives;// + 1;

    for (int i = 0; i < 4; i++)
    {
    g2dBeginRects(PlayerTexture);
    g2dResetCrop();
    g2dReset();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(8,11);
    g2dSetCoordXY(285-9*i,144);
    g2dSetCropWH(8,11);
    g2dSetCropXY((8 * (lives2  % 10)),164);
    g2dAdd();
    g2dEnd();
    lives2 = lives2 / 10;
  //Power = Power * 10;
    }

    }
   }
   else{
//FromDemoToGameMode = 0;
   }
   
    }
    else if(WaitingToStart == 1){

    g2dBeginRects(Menus);
    g2dResetCrop();
    g2dReset();
    //g2dSetTexRepeat(true);
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetColor(WHITE);
    g2dSetScaleWH(104,14);
    g2dSetCoordXY(188,144);
    g2dSetCropWH(104,14);
    g2dSetCropXY(0,28);
    g2dAdd();
    g2dEnd();


    }





}





}

if(OnMapSelector == 1){


//0 = Up
//1 = Down
//2 = Left 
//3 = Right

//<stdio.h>
if(Botones[LEFT].IsDown == 1 && GameState != 3 && GameState != 2 && GameState != 1 && OverWorldMoving == false){
OverWorldMoving = false;
if(OverWorldSquares[PlayerXOnOverWorld - 1][PlayerYOnOverWorld].State != '#' && OverWorldSquares[PlayerXOnOverWorld ][PlayerYOnOverWorld].State != 'A'){


PlayerXOnOverWorld --; 
OverWorldMoving = true;

}
OverWorldPlayerAnimState = 0;
OverWorldPlayerDirection = 2;
OverWorldPlayerAnimFrame = 1;


OverWorldDestinationX =PlayerXOnOverWorld;
OverWorldDestinationY = PlayerYOnOverWorld;
while (OverWorldSquares[OverWorldDestinationX][OverWorldDestinationY].State == '.' )
{
    OverWorldDestinationX--;
}//<pspdisplay.h>



if(OverWorldSquares[OverWorldDestinationX][OverWorldDestinationY].State == '#'){
         OverWorldDestinationX ++;
}

PlayerXOnOverWorld = OverWorldDestinationX;
PlayerYOnOverWorld = OverWorldDestinationY;


}
if(Botones[RIGHT].IsDown == 1 && GameState != 3 && GameState != 2 && GameState != 1 && OverWorldMoving == false){
OverWorldMoving = false;
if(OverWorldSquares[PlayerXOnOverWorld + 1][PlayerYOnOverWorld].State != '#' && OverWorldSquares[PlayerXOnOverWorld ][PlayerYOnOverWorld].State != 'B' && OverWorldDestinationX < 28){

if(PlayerXOnOverWorld != 30){

PlayerXOnOverWorld ++;
OverWorldMoving = true;
}
}
OverWorldPlayerAnimState = 0;
OverWorldPlayerDirection = 3;

OverWorldDestinationX = PlayerXOnOverWorld;
OverWorldDestinationY = PlayerYOnOverWorld;

while (OverWorldSquares[OverWorldDestinationX][OverWorldDestinationY].State == '.' && OverWorldDestinationX < 28)
{
    OverWorldDestinationX++;
}
if(OverWorldSquares[OverWorldDestinationX][OverWorldDestinationY].State == '#'){
         OverWorldDestinationX --;
}

 PlayerXOnOverWorld = OverWorldDestinationX;
PlayerYOnOverWorld = OverWorldDestinationY;

}



if(Botones[UP].IsDown == 1 && GameState != 3 && GameState != 2 && GameState != 1 && OverWorldMoving == false){
OverWorldMoving = false;
if(OverWorldSquares[PlayerXOnOverWorld][PlayerYOnOverWorld - 1].State != '#' && OverWorldSquares[PlayerXOnOverWorld ][PlayerYOnOverWorld].State != 'C'){
PlayerYOnOverWorld  --;
OverWorldMoving = true;
}
OverWorldPlayerDirection = 0;
OverWorldPlayerAnimState = 0;
OverWorldPlayerAnimFrame = 1;


OverWorldDestinationX =PlayerXOnOverWorld;
OverWorldDestinationY = PlayerYOnOverWorld;

while (OverWorldSquares[OverWorldDestinationX][OverWorldDestinationY].State == '.')
{
    OverWorldDestinationY--;
}
if(OverWorldSquares[OverWorldDestinationX][OverWorldDestinationY].State == '#'){
         OverWorldDestinationY ++;
}

PlayerXOnOverWorld = OverWorldDestinationX;
PlayerYOnOverWorld = OverWorldDestinationY;


}
if(Botones[DOWN].IsDown == 1 && GameState != 3 && GameState != 2 && GameState != 1 && OverWorldMoving == false){
OverWorldMoving = false;
if(OverWorldSquares[PlayerXOnOverWorld][PlayerYOnOverWorld + 1].State != '#'&& OverWorldSquares[PlayerXOnOverWorld ][PlayerYOnOverWorld].State != 'D'){
PlayerYOnOverWorld  ++;
OverWorldMoving = true;
}
OverWorldPlayerDirection = 1;
OverWorldPlayerAnimFrame = 1;
OverWorldPlayerAnimState = 0;



OverWorldDestinationX =PlayerXOnOverWorld;
OverWorldDestinationY = PlayerYOnOverWorld;

while (OverWorldSquares[OverWorldDestinationX][OverWorldDestinationY].State == '.')
{
    OverWorldDestinationY++;
}
if(OverWorldSquares[OverWorldDestinationX][OverWorldDestinationY].State == '#'){
         OverWorldDestinationY--;
}

PlayerXOnOverWorld = OverWorldDestinationX;
PlayerYOnOverWorld = OverWorldDestinationY;


}


if(Botones[CROSS].IsPressed == 1  && GameState != 3 && GameState != 2 && GameState != 1 && OverWorldMoving == false){
if(OverWorldSquares[PlayerXOnOverWorld][PlayerYOnOverWorld].LevelToLoad != '.'){

CurrentLevel = OverWorldSquares[PlayerXOnOverWorld][PlayerYOnOverWorld].LevelToLoad - 65;
LoadMap(CurrentLevel);
//GameState = 0;
        SpawnPos.x = -20;
        SpawnPos.y = -20;
        FromDemoToGameMode = 1;
        Automaticplay = 0;
        GameState = 5;

        OnMapSelector = 0;
        DeathCount = 175;
        WaitingToStart = 0;
        TimeUntillDemo = WaitTillDemoMode;
}
}



if(OverWorldPlayerDirection == 0){
    PlayerOverWorldxCrop = 54+ 19*OverWorldPlayerAnimState;
    PlayerOverWorldyCrop = 42;
if(OverWorldMoving == true){
OverWorldPlayerAnimFrame++;

if(OverWorldPlayerAnimFrame == 9){
OverWorldPlayerAnimFrame = 1;
if(OverWorldPlayerAnimState != 2){
OverWorldPlayerAnimState ++;
}
else
{
    OverWorldPlayerAnimState = 0;
}
    }

PlayerOverWorldyRender --;
if(PlayerOverWorldyRender == (OverWorldDestinationY * 16) - 23){
    OverWorldMoving = false;
   
}

}
else{
    OverWorldPlayerAnimState = 0;
}


}
else if(OverWorldPlayerDirection == 1){
    PlayerOverWorldxCrop = 69 + 19*OverWorldPlayerAnimState;
    PlayerOverWorldyCrop = 0;


if(OverWorldMoving == true){
    OverWorldPlayerAnimFrame++;
if(OverWorldPlayerAnimFrame == 9){
OverWorldPlayerAnimFrame = 1;
if(OverWorldPlayerAnimState != 2){
OverWorldPlayerAnimState ++;
}
else
{
    OverWorldPlayerAnimState = 0;
}
        
}
PlayerOverWorldyRender ++;


if(PlayerOverWorldyRender == (OverWorldDestinationY * 16) - 23){
    OverWorldMoving = false;
   
}
   }
else{
    OverWorldPlayerAnimState = 0;
}
}
else if(OverWorldPlayerDirection == 2){
       PlayerOverWorldxCrop = 92 + 19*OverWorldPlayerAnimState;
    PlayerOverWorldyCrop = 108;
     if(OverWorldMoving == true){
      OverWorldPlayerAnimFrame++;
      if(OverWorldPlayerAnimFrame == 9){
        OverWorldPlayerAnimFrame = 1;
        if(OverWorldPlayerAnimState != 1){
OverWorldPlayerAnimState ++;
        }
        else
        {
            OverWorldPlayerAnimState = 0;
        }
        
      }

PlayerOverWorldxRender --;


if(PlayerOverWorldxRender == OverWorldDestinationX * 16){
    OverWorldMoving = false;
   
}

   }
else{
    OverWorldPlayerAnimState = 0;
}
}
else{
PlayerOverWorldxCrop = 92+ 19*OverWorldPlayerAnimState;
PlayerOverWorldyCrop = 141;
  if(OverWorldMoving == true){
OverWorldPlayerAnimFrame++;
if(OverWorldPlayerAnimFrame == 9){
OverWorldPlayerAnimFrame = 1;
if(OverWorldPlayerAnimState != 1){
OverWorldPlayerAnimState ++;
}
else
{
    OverWorldPlayerAnimState = 0;
}
        
      }
PlayerOverWorldxRender ++;


if(PlayerOverWorldxRender == OverWorldDestinationX * 16){
    OverWorldMoving = false;
   
}
  
  
  
  
  }
else{
    OverWorldPlayerAnimState = 0;
}
}



    if(CurrentBackGround == NULL){
      if(Style == 0){
        CurrentBackGround = g2dTexLoad("resources/Images/BackGround.png",G2D_SWIZZLE);
      }
      else{
        CurrentBackGround = g2dTexLoad("resources/Images/BackGround2.png",G2D_SWIZZLE);

      }
    }

    g2dBeginRects(CurrentBackGround);
    g2dResetCrop();
    g2dReset();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(480,272);
    g2dSetCoordXY(0,0);
    g2dSetCropWH(480,272);
    g2dSetCropXY(0,0);
    g2dAdd();
    g2dEnd();


int CurrentI = 0;
for (int i = -1; i < 16; i++)
{
  CurrentI = i;
  
for (int e = -1; e < 30; e++)
{
if((OverWorldSquares[e][CurrentI].LevelToLoad - 65) == NextPlayableLevel)
{   
    g2dBeginRects(PlayerTexture);
    g2dResetCrop();
    g2dReset();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(16,16);
    g2dSetCoordXY(e*16,CurrentI*16);
    g2dSetCropWH(16,16);
    g2dSetCropXY(114,54);
    g2dAdd();
    g2dEnd();
}
if((OverWorldSquares[e][CurrentI].State) == 'S')
{   
    g2dBeginRects(PlayerTexture);
    g2dResetCrop();
    g2dReset();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(16,16);
    g2dSetCoordXY(e*16,CurrentI*16);
    g2dSetCropWH(16,16);
    g2dSetCropXY(114,42);
    g2dAdd();
    g2dEnd();
}

}

}
                    //[x][y]


    g2dBeginRects(PlayerTexture);
    g2dResetCrop();
    g2dReset();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(19,33);
    g2dSetCoordXY((PlayerOverWorldxRender) - 1,(PlayerOverWorldyRender));
    g2dSetCropWH(19,33);
    g2dSetCropXY(PlayerOverWorldxCrop,PlayerOverWorldyCrop);
    g2dAdd();
    g2dEnd();



    // g2dBeginRects(NULL);
    // g2dResetCrop();
    // g2dReset();
    // g2dSetColor(RED);
    // g2dSetTexLinear(false);
    // g2dSetCoordMode(G2D_UP_LEFT);
    // g2dSetAlpha(255);
    // g2dSetScaleWH(16,16);
    // g2dSetCoordXY(PlayerXOnOverWorld*16,PlayerYOnOverWorld*16);
    // g2dAdd();
    // g2dEnd();


    // g2dBeginRects(NULL);
    // g2dResetCrop();
    // g2dReset();
    // g2dSetColor(GREEN);
    // g2dSetTexLinear(false);
    // g2dSetCoordMode(G2D_UP_LEFT);
    // g2dSetAlpha(255);
    // g2dSetScaleWH(16,16);
    // g2dSetCoordXY(OverWorldDestinationX*16,OverWorldDestinationY*16);
    // g2dAdd();
    // g2dEnd();
}     




if(GameState == 10)
{
   // CurrentBackGround == NULL;
  g2dBeginRects(CurrentBackGround);
    g2dResetCrop();
    g2dReset();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(480,272);
    g2dSetCoordXY(0,0);
    g2dSetCropWH(480,272);
    g2dSetCropXY(0,0);
    g2dAdd();
    g2dEnd();
SequenceCounter++;
if(SequenceCounter < 240)//<pspctrl.h>
{
    if(CurrentBackGround == NULL){
    CurrentBackGround = g2dTexLoad("resources/Film1/1.png",G2D_SWIZZLE);
    }
}
   if(SequenceCounter == 241){
    CurrentBackGround = NULL;
   }
 
 if(SequenceCounter < 481){
     if(CurrentBackGround == NULL){
    CurrentBackGround = g2dTexLoad("resources/Film1/2.png",G2D_SWIZZLE);
    }
 }
 if(SequenceCounter == 482){
    CurrentBackGround = NULL;

   }
 
  if(SequenceCounter < 722){
     if(CurrentBackGround == NULL){
    CurrentBackGround = g2dTexLoad("resources/Film1/3.png",G2D_SWIZZLE);
    }
 }
 if(SequenceCounter == 723){
    CurrentBackGround = NULL;
    GameState = 0;
OnMapSelector = 1;

SequenceCounter = 0;
   }

}



if(GameState == 11)
{
    PlayedEndSequence = 1;
   // CurrentBackGround == NULL;
  g2dBeginRects(CurrentBackGround);
    g2dResetCrop();
    g2dReset();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(480,272);
    g2dSetCoordXY(0,0);
    g2dSetCropWH(480,272);
    g2dSetCropXY(0,0);
    g2dAdd();
    g2dEnd();

    if(CurrentBackGround == NULL && SequenceCounter == 0){
    CurrentBackGround = g2dTexLoad("resources/Film1/4.png",G2D_SWIZZLE);
    }
SequenceCounter++;


   if(SequenceCounter == 481){
    CurrentBackGround = NULL;
  //      GameState = 0;
//OnMapSelector = 1;
  CurrentBackGround = g2dTexLoad("resources/Film1/5.png",G2D_SWIZZLE);

   }
 
if(SequenceCounter == 721){
GameState = 4;
SequenceCounter = 0;
SaveData();
SaveCoinsAndLives(coins,lives,PlayedSequence1,PlayedEndSequence);
CurrentBackGround = NULL;
}

}


if(GameState == 4){


if(CurrentBackGround == NULL){
      CurrentBackGround = g2dTexLoad("resources/Images/MainMenu.png",G2D_SWIZZLE);
}

MenuSequenceCounter ++;

if(MenuSequenceCounter == 13){
    MenuSequenceCounter = 0;
    if(MenuMultiplier == 0){
        MenuMultiplier = 1;
    }
    else{
           MenuMultiplier = 0;
    }
}


    //Main menu is active 
    //----------------------------------------------------------------------------------

    g2dBeginRects(CurrentBackGround);
    g2dResetCrop();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetColor(WHITE);
    g2dSetAlpha(255);
    g2dSetScaleWH(280,272);
    g2dSetCropWH(280,272);
    g2dSetCropXY(200,0);
    g2dSetCoordXY(200,0);
    g2dAdd();
    g2dEnd();


    g2dBeginRects(NULL);
    g2dResetCrop();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetColor(GRAY);
    g2dSetAlpha(255);
    g2dSetScaleWH(200,400);
    g2dSetCoordXY(0,0);
    g2dAdd();
    g2dEnd();


    g2dBeginRects(CurrentBackGround);
    g2dResetCrop();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetColor(WHITE);
    g2dSetAlpha(255);
    g2dSetScaleWH(145,52);
    g2dSetCropWH(145,52);
    g2dSetCropXY(0,52*MenuMultiplier);
    g2dSetCoordXY(241,148);
    g2dAdd();
    g2dEnd();






    g2dBeginRects(PlayerTexture);
    g2dResetCrop();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(7,11);
    g2dSetCoordXY( 22, MainMenuButtons[SelectedButton].y + 1);
    g2dSetColor(BLACK);      
    g2dSetCropWH(7,11);
    g2dSetCropXY(85,164);

    g2dAdd();
    g2dEnd();

for ( i = 0; i < 3; i++)
{
  
    g2dBeginRects(Menus);
    g2dResetCrop();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(128,14);
    g2dSetCoordXY(40, MainMenuButtons[i].y);
    g2dSetColor(BLACK);      
    g2dSetCropWH(128,14);
    g2dSetCropXY(0,(MainMenuButtons[i].Function * 14)  );

    g2dAdd();
    g2dEnd();


    

    
}




if(Botones[DOWN].IsPressed == 1){
    SelectedButton++;
    if(SelectedButton == 3){
        SelectedButton = 0;
    }
}

if(Botones[UP].IsPressed == 1){
    SelectedButton--;
    if(SelectedButton == -1){
        SelectedButton = 2;
    }
}


if(Botones[CROSS].IsPressed == 1){
    CurrentBackGround = NULL;
    if(MainMenuButtons[SelectedButton].Function == 5){
        PlayerXOnOverWorld = 1;
        PlayerYOnOverWorld = 2;
        OverWorldDestinationX = 0;
        OverWorldDestinationY = 0;
        OverWorldMoving = 0;
        SpawnPos.x = -20;
        SpawnPos.y = -20;
        PlayerOverWorldxRender = (PlayerXOnOverWorld * 16) -1;
        PlayerOverWorldyRender = (PlayerYOnOverWorld * 16) -23;
   PlayedEndSequence = 0;
   PlayedSequence1 = 0;
    LoadOverWorldFromOriginal();
lives = 3;
coins = 0;
  //  OnMapSelector = 1;
    GameState = 10;
    PlayedSequence1 = 1;
    CurrentBackGround = NULL;
    }
    if(MainMenuButtons[SelectedButton].Function == 4){
    LoadOverWorld();
lives = LoadLives();
coins = LoadCoins();
PlayedSequence1 = LoadSequence();
PlayedEndSequence = LoadEndSequence();
coinsForLives = coins - (coins / CoinsUntilNewLife) * 50;

if(AlreadyLoadedOverWordlsPalyerPos == 0){
    AlreadyLoadedOverWordlsPalyerPos = 1;
if(NextPlayableLevel < 32 && NextPlayableLevel != 0){
        int CurrentI = 0;
for (int i = -1; i < 16; i++)
{
  CurrentI = i;
  
for (int e = -1; e < 30; e++)
{
if((OverWorldSquares[e][CurrentI].LevelToLoad - 65) == NextPlayableLevel)
{   
    PlayerXOnOverWorld = e;
    PlayerYOnOverWorld = CurrentI;
}
}

}
    }
       OverWorldDestinationX = 0;
       OverWorldDestinationY = 0;
       OverWorldMoving = 0;
       PlayerOverWorldxRender = (PlayerXOnOverWorld * 16) -1;
       PlayerOverWorldyRender = (PlayerYOnOverWorld * 16) -23;

}
   //    fdas<pspkernel.h>

if(lives <= 0){
    lives = 3;
}
if (coins < 0)
{
coins = 0;
    /* code */
}



    SpawnPos.x = -20;
    SpawnPos.y = -20;
    
    if(PlayedSequence1 == 0){
    GameState = 10;
    CurrentBackGround = NULL;
    PlayedSequence1 = 1;
    }
    else{
        OnMapSelector = 1;
        GameState = 0;
    }

   // OverWorldMoving = false;
    }
  
    if(MainMenuButtons[SelectedButton].Function == 7){

DeveloperMode = true;
CurrentBackGround = NULL;
   // OverWorldMoving = false;
    }
  
  
    SelectedButton = 0;
    Botones[CROSS].IsPressed = 2;
    Botones[CROSS].WasPressed = 1;
}




    //----------------------------------------------------------------------------------

}


//Game is paused
//--------------------------------------------------------------------------------------
if(GameState == 3){

    g2dBeginRects(Menus);
    g2dResetCrop();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(84,14);
    g2dSetCoordXY( 198 , 136);
    g2dSetColor(BLACK);      
    g2dSetCropWH(84,14);
    g2dSetCropXY(0,0);

    g2dAdd();
    g2dEnd();

    for (int i = 0; i < 3; i++)
    {
    g2dBeginRects(Menus);
    g2dResetCrop();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(128,14);
    g2dSetCoordXY( 198, 136 + PauseButtons[i].y);
    g2dSetColor(BLACK);      
    g2dSetCropWH(128,14);
    g2dSetCropXY(0,(PauseButtons[i].Function * 14) );

    g2dAdd();
    g2dEnd();
    }
    

    g2dBeginRects(PlayerTexture);
    g2dResetCrop();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(7,11);
    g2dSetCoordXY( 186, 138 + PauseButtons[SelectedButton].y);
    g2dSetColor(BLACK);      
    g2dSetCropWH(7,11);
    g2dSetCropXY(85,164);

    g2dAdd();
    g2dEnd();

if(Botones[DOWN].IsPressed == 1){
    SelectedButton++;
    if(SelectedButton == 3){
        SelectedButton = 0;
    }
}

if(Botones[UP].IsPressed == 1){
    SelectedButton--;
    if(SelectedButton == -1){
        SelectedButton = 2;
    }
}


if(Botones[CROSS].IsPressed == 1){
    if(PauseButtons[SelectedButton].Function == 4){
        GameState = 0;
        SelectedButton = 0;
    }
    else if(PauseButtons[SelectedButton].Function == 6){
    SaveData();
    SaveCoinsAndLives(coins,lives,PlayedSequence1,PlayedEndSequence);
    GameState = 0;
    SelectedButton = 0;
    }
    else if(PauseButtons[SelectedButton].Function == 3){
    GameState = 4;
    CurrentBackGround = NULL;
    OnMapSelector = 0;
    SelectedButton = 0;
    }
    Botones[CROSS].IsPressed == 2;
}

}
//--------------------------------------------------------------------------------------





//Draw the gradient
//--------------------------------------------------------------------------------------
if(GameState == 1 || GameState == 2){

// DrawTexturePro(GradientTexture,(Rectangle){0,0,73,140},(Rectangle){Gradient.X+Player.X+90,Gradient.Y-70,73*2,140*2},(Vector2){0,0},0,WHITE);
// DrawTexturePro(GradientTexture,(Rectangle){73,0,73,140},(Rectangle){Gradient.X+586+Player.X+90,Gradient.Y-70,73*2,140*2},(Vector2){0,0},0,WHITE);
// DrawRectangle(Gradient.X+(GradientTexture.width)+Player.X+90,Gradient.Y-70,440,140*2,BLACK);

        g2dBeginRects(NULL);
    g2dResetCrop();
    g2dReset();
    g2dSetColor(BLACK);
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(515,280);
    g2dSetCoordXY(Gradient.X+146+90,0);
    g2dAdd();
    g2dEnd();

    g2dBeginRects(GradientTexture);
    g2dResetCrop();
    g2dReset();
    g2dSetColor(BLACK);
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(146,280);
    g2dSetCoordXY(Gradient.X+90,0);
g2dSetCropXY(0,0);
g2dSetCropWH(73,140);
    g2dAdd();
    g2dEnd();

        g2dBeginRects(GradientTexture);
    g2dResetCrop();
    g2dReset();
    g2dSetColor(BLACK);
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(146,280);
    g2dSetCoordXY(Gradient.X+(73)+90+586,0);
    g2dSetCropXY(73,0);
    g2dSetCropWH(73,140);
    g2dAdd();
    g2dEnd();
    



}
//--------------------------------------------------------------------------------------


        g2dFlip(G2D_VSYNC);



      //  EndDrawing();
        //----------------------------------------------------------------------------------

}
else{

//Level editor is active


//Update camera positon
//--------------------------------------------------------------------------------------
MapView.Y = MapView.Y + ((pad.Ly - 128) / 24);
MapView.X = MapView.X + ((pad.Lx - 128) / 24);

CamX = (int)((MapView.X)-240);

CamY = ((MapView.Y)-116);
//--------------------------------------------------------------------------------------
if(CurrentBackGround == NULL){
CurrentBackGround = g2dTexLoad("resources/Images/NewEditor.png",G2D_SWIZZLE);
}
//Key controls
//--------------------------------------------------------------------------------------


if(Botones[UP].IsPressed == 1){
if(OnSquareSelector == false){
if(MousePos.y > 0){
    LastYPOS = MousePos.y;
MousePos.y --;
LastXPOS = MousePos.x;

RepeatCountVertical = 1;
RepeatCountHorizontal = 1;
}
}
else{
     if(SquareSelectorY - 1 >= 0){
    SquareSelectorY--;

    }
    else{
        SquareSelectorY = 4;
    }
}
}
else if(Botones[UP].IsDown == 1 && OnSquareSelector == false){
if(MousePos.y > 0){
if(RepeatCountVertical == 0){
    LastYPOS = MousePos.y;
LastXPOS = MousePos.x;

MousePos.y --;
RepeatCountVertical = 1;
}
else{
    RepeatCountVertical ++;
    if(RepeatCountVertical == RepeatMax){
        RepeatCountVertical = 0;
    }
}

}
}

if(Botones[DOWN].IsPressed == 1 ){
    if(OnSquareSelector == false){
if(MousePos.y < MapHeight -1 ){
    LastYPOS = MousePos.y;
LastXPOS = MousePos.x;
MousePos.y ++;
RepeatCountVertical = 1;
RepeatCountHorizontal = 1;
}
    }
else{
    if(SquareSelectorY + 1 <= 4){
    SquareSelectorY++;

    }
    else{
        SquareSelectorY = 0;
    }
}
}
else if(Botones[DOWN].IsDown == 1 && OnSquareSelector == false){
if(MousePos.y < MapHeight -1 ){
    if(RepeatCountVertical == 0){
LastYPOS = MousePos.y;

MousePos.y ++;
LastXPOS = MousePos.x;

RepeatCountVertical = 1;
    }
    else{
    RepeatCountVertical ++;
    if(RepeatCountVertical == RepeatMax){
        RepeatCountVertical = 0;
    }
    }
}
}



if(Botones[LEFT].IsPressed  == 1&& Botones[START].IsDown == 0){
if(OnSquareSelector == false){
if(MousePos.x > 0){
LastXPOS = MousePos.x;
LastYPOS = MousePos.y;

MousePos.x --;
RepeatCountVertical = 1;
RepeatCountHorizontal = 1;
}
}
else{
     if(SquareSelectorX - 1 >= 0){
    SquareSelectorX--;

    }
    else{
        SquareSelectorX = 9;
    }
}
}
else if(Botones[LEFT].IsDown == 1 && Botones[START].IsDown == 0 && OnSquareSelector == false){
   if(MousePos.x > 0){
    if(RepeatCountHorizontal == 0){
        LastXPOS = MousePos.x;
MousePos.x --;
LastYPOS = MousePos.y;

RepeatCountHorizontal = 1;
    }
    else{
         RepeatCountHorizontal ++;
    if(RepeatCountHorizontal == RepeatMax){
        RepeatCountHorizontal = 0;
    }
    }
   }
}


if(Botones[RIGHT].IsPressed == 1 && Botones[START].IsDown == 0){
if(OnSquareSelector == false){
if(MousePos.x < MapWidth-1){//<time.h>
LastXPOS = MousePos.x;
LastYPOS = MousePos.y;
MousePos.x ++;
RepeatCountHorizontal = 1;
RepeatCountVertical = 1;
}
}
else{//<time.h>
    if(SquareSelectorX + 1 < 10){
    SquareSelectorX++;

    }
    else{
        SquareSelectorX = 0;
    }
}
}
else if(Botones[RIGHT].IsDown == 1&& Botones[START].IsDown == 0 && OnSquareSelector == false){
   if(MousePos.x < MapWidth-1){
    if(RepeatCountHorizontal == 0){
        LastXPOS = MousePos.x;
LastYPOS = MousePos.y;

MousePos.x ++;
RepeatCountHorizontal = 1;
    }
    else{
         RepeatCountHorizontal ++;
    if(RepeatCountHorizontal == RepeatMax){
        RepeatCountHorizontal = 0;
    }
    }
   }
}



if(Botones[LTRIGGER].IsPressed == 1 && Botones[RTRIGGER].IsDown == 0){
    filltobottom = false;
    if(OnSquareSelector == true){
    OnSquareSelector = false;
    MousePos.Tile = MENUDATATiles[SquareSelectorY][ SquareSelectorX];
    MousePos.TypeOfTileBehavior = MENUDATATypes[SquareSelectorY][ SquareSelectorX];
    if( MousePos.TypeOfTileBehavior == 6){
       
       
if(MousePos.Tile == 0){
    int EnemiesONMap = 0;
    int EnemyBlocker = 0;

   int CurentI = 0;
        for (int i = 0; i < MapWidth; i++)
        {
            CurentI = i;
         for (int e = 0; e < MapHeight; e++)
         {
           if(Map2[e][CurentI] == 216)
           {
EnemiesONMap++;
           
           }
           if(Map2[e][CurentI] == 232){
EnemiesONMap++;
            
           }
            if(Map2[e][CurentI] == 229){
EnemiesONMap++;
   
           }
           if(Map2[e][CurentI] == 215)
           {
//EnemiesONMap++;
EnemyBlocker++;
           }
         }
        }

if(EnemyBlocker < 80 && EnemiesONMap < 40){
  SaveMap();
  MaxLevels = MaxLevels +1;
    Message = 1;
    MessageCounter = MessageCounterMax;
}
else if(EnemyBlocker < 80){
    Message = 6;
    MessageCounter = MessageCounterMax;
}
else if(EnemiesONMap < 40){
    Message = 8;
    MessageCounter = MessageCounterMax;
}
  
}
else if(MousePos.Tile == 1){
ClearMap();
    Message = 3;
    MessageCounter = MessageCounterMax;
}
else if(MousePos.Tile == 2){
    DeveloperMode = false;
    LeaveToEditor = true;
    Automaticplay = 0;
    WaitingToStart = 0;
    CurrentBackGround = NULL;
    GameState = 0;
    OnMapSelector = 0;
    ResetPlayerAndFlagPos();
}
else if(MousePos.Tile == 3){
    LoadMap(0);
    CurrentLevel = 1;
       Message = 5;
    MessageCounter = MessageCounterMax;
}
else if(MousePos.Tile == 4){
  
        int EnemiesONMap = 0;
    int EnemyBlocker = 0;

   int CurentI = 0;
        for (int i = 0; i < MapWidth; i++)
        {
            CurentI = i;
         for (int e = 0; e < MapHeight; e++)
         {
           if(Map2[e][CurentI] == 216)
           {
EnemiesONMap++;
           
           }
           if(Map2[e][CurentI] == 232){
EnemiesONMap++;
            
           }
            if(Map2[e][CurentI] == 229){
EnemiesONMap++;
   
           }
           if(Map2[e][CurentI] == 215)
           {
//EnemiesONMap++;
EnemyBlocker++;
           }
         }
        }

if(EnemyBlocker < 80 && EnemiesONMap < 40){
   SaveMap2(CurrentLevel-1);
        Message = 2;
    MessageCounter = MessageCounterMax;
}
else if(EnemyBlocker < 80){
    Message = 6;
    MessageCounter = MessageCounterMax;
}
else if(EnemiesONMap < 40){
    Message = 8;
    MessageCounter = MessageCounterMax;
}
}
        
       
        MousePos.Tile = 65;
        MousePos.TypeOfTileBehavior = 4;
        SquareSelectorY = 0;
        SquareSelectorX = 0;

    }
  
  

    }
    else{
    OnSquareSelector = true;
    }
}


if(Botones[MUSICNOTE].IsPressed == 1){
      MousePos.x = (MapView.X)/20;
  MousePos.y = (MapView.Y)/20;
  if(MousePos.x < 0){
     MousePos.x = 0;
  }
    if(MousePos.y < 0){
     MousePos.y = 0;
  }
}

if(Botones[LTRIGGER].IsDown == 1 && Botones[RTRIGGER].IsDown == 1){
      MousePos.x = (MapView.X)/20;
  MousePos.y = (MapView.Y)/20;
  if(MousePos.x < 0){
     MousePos.x = 0;
  }
    if(MousePos.y < 0){
     MousePos.y = 0;
  }
}


if(Botones[CROSS].IsDown == 1 && OnSquareSelector == false){

    LeftMousePressed = true;
if (MousePos.TypeOfTileBehavior == 9 && Map2[MousePos.y][MousePos.x] != MousePos.Tile){

Map2[MousePos.y][MousePos.x] = MousePos.Tile - 1;

if(MousePos.y - LastYPOS == 1){
Map2[MousePos.y-1][MousePos.x] = MousePos.Tile + 1;
Map2[MousePos.y][MousePos.x] = MousePos.Tile ;

}
if(MousePos.y - LastYPOS == -1){
Map2[MousePos.y+1][MousePos.x] = MousePos.Tile + 1;
Map2[MousePos.y][MousePos.x] = MousePos.Tile ;

}

}
    
}

if(Botones[CROSS].IsPressed == 1 && OnSquareSelector == false){
    if(MousePos.TypeOfTileBehavior == 3){
Map2[MousePos.y][MousePos.x] = MousePos.Tile;
MousePos.TypeOfTileBehavior = 9;
    }
    else if (MousePos.TypeOfTileBehavior == 9){
Map2[MousePos.y][MousePos.x] = MousePos.Tile - 1;

    }
}

if(XPressed == 1 && Botones[CROSS].IsDown == 0 && OnSquareSelector == false){
    
    if (MousePos.TypeOfTileBehavior == 9 && Map2[MousePos.y][MousePos.x] != MousePos.Tile){
Map2[MousePos.y][MousePos.x] = MousePos.Tile + 1;
MousePos.TypeOfTileBehavior = 3;
    }
}

if(Botones[RTRIGGER].IsPressed == 1 && MousePos.TypeOfTileBehavior == 0){
if(filltobottom == true){
    filltobottom = false;
}
else{
    filltobottom = true;
}
}


XPressed = Botones[CROSS].IsDown;

if(Botones[START].IsDown == 1 && Botones[RIGHT].IsPressed == 1){
 if(CurrentLevel < MaxLevels){
        LoadMap(CurrentLevel);
        Map2ToMap1();
        ResetPlayerAndFlagPos();
        CurrentLevel++;
//<stdio.h>
    }
    else{
        CurrentLevel = 0;
        LoadMap(CurrentLevel);
        CurrentLevel = 1;
                Map2ToMap1();
        ResetPlayerAndFlagPos();
    }
}
if(Botones[START].IsDown == 1 && Botones[LEFT].IsPressed == 1){
 if(CurrentLevel - 1 > 0){
  
        CurrentLevel--;
        LoadMap(CurrentLevel-1);
        Map2ToMap1();
        ResetPlayerAndFlagPos();
    }
    else{
        CurrentLevel = MaxLevels;
        LoadMap(MaxLevels-1);
        
        Map2ToMap1();
        ResetPlayerAndFlagPos();
    }
}

if(Botones[CIRCLE].IsDown == 1){
RightMousePressed = true;
}

if(Botones[SQUARE].IsPressed == 1){
    if(EditedLayer == 2){
EditedLayer = 1;
    }
    else {
        EditedLayer = 2;
    }
}
//--------------------------------------------------------------------------------------



//Mouse control
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------


    g2dClear(WHITE);


       g2dBeginRects(NULL);
    g2dResetCrop();
    g2dSetColor(RED);
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(9500,1500);
    g2dSetCoordXY(-CamX,-CamY);
    g2dAdd();
    g2dEnd();
        //Draw the squares 
        //--------------------------------------------------------------------------------------
            int CurrentI = 0;
        for (int i = XMin; i < XMax; i++)
        {

            CurrentI = i;
        for (int e = YMin; e < YMax; e++)
        {
//dfsad


            if(Map2[e][CurrentI] > 64)
            {
    int Y;
    int X = Map2[e][CurrentI]-65;
    
    Y = X / 25;


    X = (X) - (Y * 25);

    g2dBeginRects(TileSheet);
    g2dResetCrop();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(20,20);
    g2dSetCoordXY((int)((CurrentI * 20)-CamX),(int)((e*20)-CamY));
    g2dSetCropWH(20,20);
    g2dSetCropXY(X*20,Y*20);
    g2dAdd();
    g2dEnd();
}
  
  
       
        }
        }
        //--------------------------------------------------------------------------------------
      
if(EditedLayer == 1){
           g2dBeginRects(NULL);
    g2dResetCrop();
    g2dSetColor(GRAY);
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(125);
    g2dSetScaleWH(9500,1500);
    g2dSetCoordXY(-CamX,-CamY);
    g2dAdd();
    g2dEnd();
}

        CurrentI = 0;
        for (int i = XMin; i < XMax; i++)
        {

            CurrentI = i;
        for (int e = YMin; e < YMax; e++)
        {
            if(Map[e][CurrentI] > 64)
            {
            int Y;
            int X = Map[e][CurrentI]-65;
            Y = X / 25;
            X = (X) - (Y * 25);

            g2dBeginRects(TileSheet);
            g2dResetCrop();
            g2dSetTexLinear(false);
            g2dSetCoordMode(G2D_UP_LEFT);
            g2dSetAlpha(255);
            g2dSetScaleWH(20,20);
            g2dSetCoordXY((int)((CurrentI * 20)-CamX),(int)((e*20)-CamY));
            g2dSetCropWH(20,20);
            g2dSetCropXY(X*20,Y*20);
            g2dAdd();
            g2dEnd();
            }


            if(Map2[e][CurrentI] == '$'){
            g2dBeginRects(TileSheet);
            g2dResetCrop();
            g2dSetTexLinear(false);
            g2dSetCoordMode(G2D_UP_LEFT);
            g2dSetAlpha(255);
            g2dSetScaleWH(20,20);
            g2dSetCoordXY((int)((CurrentI * 20)-CamX),(int)((e*20)-CamY));
            g2dSetCropWH(20,20);
            g2dSetCropXY(380,140);
            g2dAdd();
            g2dEnd();
  }
            if(Map2[e][CurrentI] == '#'){
            g2dBeginRects(TileSheet);
            g2dResetCrop();
            g2dSetTexLinear(false);
            g2dSetCoordMode(G2D_UP_LEFT);
            g2dSetAlpha(255);
            g2dSetScaleWH(20,20);
            g2dSetCoordXY((int)((CurrentI * 20)-CamX),(int)((e*20)-CamY));
            g2dSetCropWH(20,20);
            g2dSetCropXY(420,140);
            g2dAdd();
            g2dEnd();
  }
       
         }
        }


//Mouse behaviour
//---------------------------------------------------------
   if(LeftMousePressed == true){
              
              
                if(EditedLayer == 2){


                    if(MousePos.TypeOfTileBehavior == 0){
                        if(filltobottom == false){
               Map2[MousePos.y][MousePos.x] = MousePos.Tile;
                        }
                        else{ 
    int Y = MousePos.y;
    while (Y < MapHeight-1)
    {
        Map2[Y][MousePos.x] = MousePos.Tile;
        Y++;
    }
                        }
                    }
                    else{
                        if(MousePos.TypeOfTileBehavior == 5){
                        Map2[MousePos.y][MousePos.x] = MousePos.Tile;
                        Map2[MousePos.y+1][MousePos.x] = MousePos.Tile + 6;

                        }
                        
                        else if(MousePos.TypeOfTileBehavior == 4){
                      int Cancel = 0;


                      
                      if(MousePos.Tile == 65){

if( Map2[MousePos.y+1][MousePos.x] >= 125 && Map2[MousePos.y+1][MousePos.x] < 131){
Map2[MousePos.y][MousePos.x] = 85;
Map2[MousePos.y-1][MousePos.x] = '.';
Cancel = 1;

}
if(Map2[MousePos.y+1][MousePos.x] >= 113 && Map2[MousePos.y+1][MousePos.x] < 119){
Map2[MousePos.y][MousePos.x] = 83;
Map2[MousePos.y-1][MousePos.x] = '.';
Cancel = 1;
}

if(Cancel == 0){
            Map2[MousePos.y][MousePos.x] = MousePos.Tile;


                        if(MousePos.y - LastYPOS == -1 && Map2[MousePos.y+1][MousePos.x] != 169 && Map2[MousePos.y+1][MousePos.x] != 86){
                        Map2[MousePos.y+1][MousePos.x] = 169;
                        }
                        if(Map2[MousePos.y+1][MousePos.x] == 169 || Map2[MousePos.y+1][MousePos.x] == 86){
                        
                        Map2[MousePos.y][MousePos.x] = 86;

                        }

                        if(MousePos.x - LastXPOS == 1 && Map2[MousePos.y][LastXPOS] == 86){
                        Map2[MousePos.y][LastXPOS] = 85;
                        }
                        else if (Map2[MousePos.y][LastXPOS] == 84){
                        Map2[MousePos.y][LastXPOS] = 168;

                        }



                        if(MousePos.y - LastYPOS == 1 && (Map2[MousePos.y-1][MousePos.x-1] == 65  || Map2[MousePos.y-1][MousePos.x-1] == 85 ) && Map2[MousePos.y][MousePos.x] != 84){
                        Map2[MousePos.y-1][MousePos.x] = 83;
                        Map2[MousePos.y][MousePos.x] = 84;
                        }
                     
if(Map2[MousePos.y-1][MousePos.x-1] == 168){
    Map2[MousePos.y-1][MousePos.x] = 83;
}

                        if(MousePos.y - LastYPOS == 1){
                        Map2[MousePos.y][MousePos.x] = 84;
                        }
 }
                   
                      }
                    
                       
                      if(MousePos.Tile == 71){

if( Map2[MousePos.y+1][MousePos.x] >= 125 && Map2[MousePos.y+1][MousePos.x] < 131){
Map2[MousePos.y][MousePos.x] = 93;
Map2[MousePos.y-1][MousePos.x] = '.';
Cancel = 1;

}
if(Map2[MousePos.y+1][MousePos.x] >= 113 && Map2[MousePos.y+1][MousePos.x] < 119){
Map2[MousePos.y][MousePos.x] = 91;
Map2[MousePos.y-1][MousePos.x] = '.';
Cancel = 1;
}


if(Cancel == 0){
  
  
            Map2[MousePos.y][MousePos.x] = MousePos.Tile;
  
   if(MousePos.y - LastYPOS == -1 && Map2[MousePos.y+1][MousePos.x] != 175 && Map2[MousePos.y+1][MousePos.x] != 94){
                        Map2[MousePos.y+1][MousePos.x] = 175;
                        }
                        if(Map2[MousePos.y+1][MousePos.x] == 175 || Map2[MousePos.y+1][MousePos.x] == 94){
                        
                        Map2[MousePos.y][MousePos.x] = 94;

                        }

                        if(MousePos.x - LastXPOS == 1 && Map2[MousePos.y][LastXPOS] == 94){
                        Map2[MousePos.y][LastXPOS] = 93;
                        }
                        else if (Map2[MousePos.y][LastXPOS] == 92){
                        Map2[MousePos.y][LastXPOS] = 174;

                        }



                        if(MousePos.y - LastYPOS == 1 && (Map2[MousePos.y-1][MousePos.x-1] == 71  || Map2[MousePos.y-1][MousePos.x-1] == 93 ) && Map2[MousePos.y][MousePos.x] != 92){
                        Map2[MousePos.y-1][MousePos.x] = 91;
                        Map2[MousePos.y][MousePos.x] = 92;
                        }
                     
if(Map2[MousePos.y-1][MousePos.x-1] == 174){
    Map2[MousePos.y-1][MousePos.x] = 91;
}

                        if(MousePos.y - LastYPOS == 1){
                        Map2[MousePos.y][MousePos.x] = 92;
                        }
}
                     


                      }
                     
                      if(MousePos.Tile == 68){
       


if( Map2[MousePos.y+1][MousePos.x] >= 125 && Map2[MousePos.y+1][MousePos.x] < 131){
Map2[MousePos.y][MousePos.x] = 89;
Map2[MousePos.y-1][MousePos.x] = '.';
Cancel = 1;

}
if(Map2[MousePos.y+1][MousePos.x] >= 113 && Map2[MousePos.y+1][MousePos.x] < 119){
Map2[MousePos.y][MousePos.x] = 87;
Map2[MousePos.y-1][MousePos.x] = '.';
Cancel = 1;
}


if(Cancel == 0){
 Map2[MousePos.y][MousePos.x] = MousePos.Tile;

                        if(MousePos.y - LastYPOS == -1 && Map2[MousePos.y+1][MousePos.x] != 172 && Map2[MousePos.y+1][MousePos.x] != 90){
                        Map2[MousePos.y+1][MousePos.x] = 172;
                        }
                        if(Map2[MousePos.y+1][MousePos.x] == 172 || Map2[MousePos.y+1][MousePos.x] == 90){
                        
                        Map2[MousePos.y][MousePos.x] = 90;

                        }

                        if(MousePos.x - LastXPOS == 1 && Map2[MousePos.y][LastXPOS] == 90){
                        Map2[MousePos.y][LastXPOS] = 89;
                        }
                        else if (Map2[MousePos.y][LastXPOS] == 88){
                        Map2[MousePos.y][LastXPOS] = 171;

                        }



                        if(MousePos.y - LastYPOS == 1 && (Map2[MousePos.y-1][MousePos.x-1] == 68  || Map2[MousePos.y-1][MousePos.x-1] == 89 ) && Map2[MousePos.y][MousePos.x] != 88){
                        Map2[MousePos.y-1][MousePos.x] = 87;
                        Map2[MousePos.y][MousePos.x] = 88;
                        }
                     
if(Map2[MousePos.y-1][MousePos.x-1] == 171){
    Map2[MousePos.y-1][MousePos.x] = 87;
}

                        if(MousePos.y - LastYPOS == 1){
                        Map2[MousePos.y][MousePos.x] = 88;
                        }
}
           


                      }
                    
                      if(MousePos.Tile == 74){
        



if( Map2[MousePos.y+1][MousePos.x] >= 125 && Map2[MousePos.y+1][MousePos.x] < 131){
Map2[MousePos.y][MousePos.x] = 97;
Map2[MousePos.y-1][MousePos.x] = '.';
Cancel = 1;

}
if(Map2[MousePos.y+1][MousePos.x] >= 113 && Map2[MousePos.y+1][MousePos.x] < 119){
Map2[MousePos.y][MousePos.x] = 87;
Map2[MousePos.y-1][MousePos.x] = '.';
Cancel = 1;
}


        if(Cancel == 0){
      Map2[MousePos.y][MousePos.x] = MousePos.Tile;

                        if(MousePos.y - LastYPOS == -1 && Map2[MousePos.y+1][MousePos.x] != 181 && Map2[MousePos.y+1][MousePos.x] != 98){
                        Map2[MousePos.y+1][MousePos.x] = 181;
                        }
                        if(Map2[MousePos.y+1][MousePos.x] == 181 || Map2[MousePos.y+1][MousePos.x] == 98){
                        
                        Map2[MousePos.y][MousePos.x] = 98;

                        }

                        if(MousePos.x - LastXPOS == 1 && Map2[MousePos.y][LastXPOS] == 98){
                        Map2[MousePos.y][LastXPOS] = 97;
                        }
          
                        else if (Map2[MousePos.y][LastXPOS] == 96){
                        Map2[MousePos.y][LastXPOS] = 180;

                        }



                        if(MousePos.y - LastYPOS == 1 && (Map2[MousePos.y-1][MousePos.x-1] == 74  || Map2[MousePos.y-1][MousePos.x-1] == 97 ) && Map2[MousePos.y][MousePos.x] != 96){
                        Map2[MousePos.y-1][MousePos.x] = 95;
                        Map2[MousePos.y][MousePos.x] = 96;
                        }
                     
if(Map2[MousePos.y-1][MousePos.x-1] == 180){
    Map2[MousePos.y-1][MousePos.x] = 95;
}

                        if(MousePos.y - LastYPOS == 1){
                        Map2[MousePos.y][MousePos.x] = 96;
                        }

        }
      

                      }
                       

                
                        }
                        else if(MousePos.TypeOfTileBehavior == 2){
                        Map2[MousePos.y][MousePos.x] = MousePos.Tile;
                        Map2[MousePos.y+1][MousePos.x] = MousePos.Tile + 1;

                        }
                        else if(MousePos.TypeOfTileBehavior == 1){
                        Map2[MousePos.y][MousePos.x] = MousePos.Tile;
                        Map2[MousePos.y][MousePos.x+1] = MousePos.Tile + 1;

                        }
                        else if(MousePos.TypeOfTileBehavior == 8){
                            int Y;
                        Map2[MousePos.y][MousePos.x] = MousePos.Tile;
                        Y = MousePos.y;
                            Y++;

                        while (Map2[Y][MousePos.x] == '.')
                        {
                            /* code */
                            if(MousePos.Tile == 36){
                            Map2[Y][MousePos.x] = 195;

                            }
                            else{
                            Map2[Y][MousePos.x] = 197;

                            }
                            Y++;

                        }
                        
                        //Map2[MousePos.y+1][MousePos.x] = MousePos.Tile + 6;

                        }
                        else if(MousePos.TypeOfTileBehavior == 7){

  if(MousePos.x > 0 && MousePos.y > 0){
    int X = MousePos.x;
    int Y = MousePos.y;//nt CloudPos[3] = {197,201,0};
int CurrentTile = 202;//MousePos.Tile;
//<stdlib.h>
for (int i = 0; i < 2; i++)
{
    /* code */
for (int i = 0; i < 2; i++)
{
    Map2[Y][X] = CurrentTile;
    X++;
    CurrentTile ++;
}
Y++;
X = MousePos.x;
}

    
}

                        }
                    }

                }
                else{
             
                                  
                    if(MousePos.TypeOfTileBehavior == 0){
                        if(filltobottom == false){
               Map[MousePos.y][MousePos.x] = MousePos.Tile;
                        }
                        else{ 
    int Y = MousePos.y;
    while (Y < MapHeight-1)
    {
        Map[Y][MousePos.x] = MousePos.Tile;
        Y++;
    }
                        }
                    }
                    else{
                        if(MousePos.TypeOfTileBehavior == 5){
                        Map[MousePos.y][MousePos.x] = MousePos.Tile;
                        Map[MousePos.y+1][MousePos.x] = MousePos.Tile + 6;

                        }
                        
                        else if(MousePos.TypeOfTileBehavior == 4){
                      int Cancel = 0;


                      
                      if(MousePos.Tile == 65){

if( Map[MousePos.y+1][MousePos.x] >= 125 && Map[MousePos.y+1][MousePos.x] < 131){
Map[MousePos.y][MousePos.x] = 85;
Map[MousePos.y-1][MousePos.x] = '.';
Cancel = 1;

}
if(Map[MousePos.y+1][MousePos.x] >= 113 && Map[MousePos.y+1][MousePos.x] < 119){
Map[MousePos.y][MousePos.x] = 83;
Map[MousePos.y-1][MousePos.x] = '.';
Cancel = 1;
}

if(Cancel == 0){
            Map[MousePos.y][MousePos.x] = MousePos.Tile;


                        if(MousePos.y - LastYPOS == -1 && Map[MousePos.y+1][MousePos.x] != 169 && Map[MousePos.y+1][MousePos.x] != 86){
                        Map[MousePos.y+1][MousePos.x] = 169;
                        }
                        if(Map[MousePos.y+1][MousePos.x] == 169 || Map[MousePos.y+1][MousePos.x] == 86){
                        
                        Map[MousePos.y][MousePos.x] = 86;

                        }

                        if(MousePos.x - LastXPOS == 1 && Map[MousePos.y][LastXPOS] == 86){
                        Map[MousePos.y][LastXPOS] = 85;
                        }
                        else if (Map[MousePos.y][LastXPOS] == 84){
                        Map[MousePos.y][LastXPOS] = 168;

                        }



                        if(MousePos.y - LastYPOS == 1 && (Map[MousePos.y-1][MousePos.x-1] == 65  || Map[MousePos.y-1][MousePos.x-1] == 85 ) && Map[MousePos.y][MousePos.x] != 84){
                        Map[MousePos.y-1][MousePos.x] = 83;
                        Map[MousePos.y][MousePos.x] = 84;
                        }
                     
if(Map[MousePos.y-1][MousePos.x-1] == 168){
    Map[MousePos.y-1][MousePos.x] = 83;
}

                        if(MousePos.y - LastYPOS == 1){
                        Map[MousePos.y][MousePos.x] = 84;
                        }
 }
                   
                      }
                    
                       
                      if(MousePos.Tile == 71){

if( Map[MousePos.y+1][MousePos.x] >= 125 && Map[MousePos.y+1][MousePos.x] < 131){
Map[MousePos.y][MousePos.x] = 93;
Map[MousePos.y-1][MousePos.x] = '.';
Cancel = 1;

}
if(Map[MousePos.y+1][MousePos.x] >= 113 && Map[MousePos.y+1][MousePos.x] < 119){
Map[MousePos.y][MousePos.x] = 91;
Map[MousePos.y-1][MousePos.x] = '.';
Cancel = 1;
}


if(Cancel == 0){
  
  
            Map[MousePos.y][MousePos.x] = MousePos.Tile;
  
   if(MousePos.y - LastYPOS == -1 && Map[MousePos.y+1][MousePos.x] != 175 && Map[MousePos.y+1][MousePos.x] != 94){
                        Map[MousePos.y+1][MousePos.x] = 175;
                        }
                        if(Map[MousePos.y+1][MousePos.x] == 175 || Map[MousePos.y+1][MousePos.x] == 94){
                        
                        Map[MousePos.y][MousePos.x] = 94;

                        }

                        if(MousePos.x - LastXPOS == 1 && Map[MousePos.y][LastXPOS] == 94){
                        Map[MousePos.y][LastXPOS] = 93;
                        }
                        else if (Map[MousePos.y][LastXPOS] == 92){
                        Map[MousePos.y][LastXPOS] = 174;

                        }



                        if(MousePos.y - LastYPOS == 1 && (Map[MousePos.y-1][MousePos.x-1] == 71  || Map[MousePos.y-1][MousePos.x-1] == 93 ) && Map[MousePos.y][MousePos.x] != 92){
                        Map[MousePos.y-1][MousePos.x] = 91;
                        Map[MousePos.y][MousePos.x] = 92;
                        }
                     
if(Map[MousePos.y-1][MousePos.x-1] == 174){
    Map[MousePos.y-1][MousePos.x] = 91;
}

                        if(MousePos.y - LastYPOS == 1){
                        Map[MousePos.y][MousePos.x] = 92;
                        }
}
                     


                      }
                     
                      if(MousePos.Tile == 68){
       


if( Map[MousePos.y+1][MousePos.x] >= 125 && Map[MousePos.y+1][MousePos.x] < 131){
Map[MousePos.y][MousePos.x] = 89;
Map[MousePos.y-1][MousePos.x] = '.';
Cancel = 1;

}
if(Map[MousePos.y+1][MousePos.x] >= 113 && Map[MousePos.y+1][MousePos.x] < 119){
Map[MousePos.y][MousePos.x] = 87;
Map[MousePos.y-1][MousePos.x] = '.';
Cancel = 1;
}


if(Cancel == 0){
 Map[MousePos.y][MousePos.x] = MousePos.Tile;

                        if(MousePos.y - LastYPOS == -1 && Map[MousePos.y+1][MousePos.x] != 172 && Map[MousePos.y+1][MousePos.x] != 90){
                        Map[MousePos.y+1][MousePos.x] = 172;
                        }
                        if(Map[MousePos.y+1][MousePos.x] == 172 || Map[MousePos.y+1][MousePos.x] == 90){
                        
                        Map[MousePos.y][MousePos.x] = 90;

                        }

                        if(MousePos.x - LastXPOS == 1 && Map[MousePos.y][LastXPOS] == 90){
                        Map[MousePos.y][LastXPOS] = 89;
                        }
                        else if (Map[MousePos.y][LastXPOS] == 88){
                        Map[MousePos.y][LastXPOS] = 171;

                        }



                        if(MousePos.y - LastYPOS == 1 && (Map[MousePos.y-1][MousePos.x-1] == 68  || Map[MousePos.y-1][MousePos.x-1] == 89 ) && Map[MousePos.y][MousePos.x] != 88){
                        Map[MousePos.y-1][MousePos.x] = 87;
                        Map[MousePos.y][MousePos.x] = 88;
                        }
                     
if(Map[MousePos.y-1][MousePos.x-1] == 171){
    Map[MousePos.y-1][MousePos.x] = 87;
}

                        if(MousePos.y - LastYPOS == 1){
                        Map[MousePos.y][MousePos.x] = 88;
                        }
}
           


                      }
                    
                      if(MousePos.Tile == 74){
        



if( Map[MousePos.y+1][MousePos.x] >= 125 && Map[MousePos.y+1][MousePos.x] < 131){
Map[MousePos.y][MousePos.x] = 97;
Map[MousePos.y-1][MousePos.x] = '.';
Cancel = 1;

}
if(Map[MousePos.y+1][MousePos.x] >= 113 && Map[MousePos.y+1][MousePos.x] < 119){
Map[MousePos.y][MousePos.x] = 87;
Map[MousePos.y-1][MousePos.x] = '.';
Cancel = 1;
}


        if(Cancel == 0){
      Map[MousePos.y][MousePos.x] = MousePos.Tile;

                        if(MousePos.y - LastYPOS == -1 && Map[MousePos.y+1][MousePos.x] != 181 && Map[MousePos.y+1][MousePos.x] != 98){
                        Map[MousePos.y+1][MousePos.x] = 181;
                        }
                        if(Map[MousePos.y+1][MousePos.x] == 181 || Map[MousePos.y+1][MousePos.x] == 98){
                        
                        Map[MousePos.y][MousePos.x] = 98;

                        }

                        if(MousePos.x - LastXPOS == 1 && Map[MousePos.y][LastXPOS] == 98){
                        Map[MousePos.y][LastXPOS] = 97;
                        }
          
                        else if (Map[MousePos.y][LastXPOS] == 96){
                        Map[MousePos.y][LastXPOS] = 180;

                        }



                        if(MousePos.y - LastYPOS == 1 && (Map[MousePos.y-1][MousePos.x-1] == 74  || Map[MousePos.y-1][MousePos.x-1] == 97 ) && Map[MousePos.y][MousePos.x] != 96){
                        Map[MousePos.y-1][MousePos.x] = 95;
                        Map[MousePos.y][MousePos.x] = 96;
                        }
                     
if(Map[MousePos.y-1][MousePos.x-1] == 180){
    Map[MousePos.y-1][MousePos.x] = 95;
}

                        if(MousePos.y - LastYPOS == 1){
                        Map[MousePos.y][MousePos.x] = 96;
                        }

        }
      

                      }
                       

                
                        }
                        else if(MousePos.TypeOfTileBehavior == 2){
                        Map[MousePos.y][MousePos.x] = MousePos.Tile;
                        Map[MousePos.y+1][MousePos.x] = MousePos.Tile + 1;

                        }
                        else if(MousePos.TypeOfTileBehavior == 1){
                        Map[MousePos.y][MousePos.x] = MousePos.Tile;
                        Map[MousePos.y][MousePos.x+1] = MousePos.Tile + 1;

                        }
                        else if(MousePos.TypeOfTileBehavior == 8){
                            int Y;
                        Map[MousePos.y][MousePos.x] = MousePos.Tile;
                        Y = MousePos.y;
                            Y++;

                        while (Map[Y][MousePos.x] == '.')
                        {
                            /* code */
                            if(MousePos.Tile == 36){
                            Map[Y][MousePos.x] = 195;

                            }
                            else{
                            Map[Y][MousePos.x] = 197;

                            }
                            Y++;

                        }
                        
                        //Map[MousePos.y+1][MousePos.x] = MousePos.Tile + 6;

                        }
                        else if(MousePos.TypeOfTileBehavior == 7){

  if(MousePos.x > 0 && MousePos.y > 0){
    int X = MousePos.x;
    int Y = MousePos.y;//nt CloudPos[3] = {197,201,0};
int CurrentTile = 202;//MousePos.Tile;
//<stdlib.h>
for (int i = 0; i < 2; i++)
{
    /* code */
for (int i = 0; i < 2; i++)
{
    Map[Y][X] = CurrentTile;
    X++;
    CurrentTile ++;
}
Y++;
X = MousePos.x;
}

    
}

                        }
                    }  

                }

             }
    else if(RightMousePressed == true){
    if(EditedLayer == 2){
    Map2[MousePos.y][MousePos.x] = '.';
    }
    else{
    Map[MousePos.y][MousePos.x] = '.';
    }

             }






if(MousePos.x >= 0 &&  MousePos.y >= 0){
//Repair
if(MousePos.Tile != 35 && MousePos.Tile != 36){
    int Y;
    int X = MousePos.Tile-65;
    
    Y = X / 25;


    X = (X) - (Y * 25);
    g2dBeginRects(TileSheet);
    g2dResetCrop();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(20,20);
    g2dSetCoordXY((MousePos.x*20)-CamX,(MousePos.y*20)-CamY);
    g2dSetCropWH(20,20);
    g2dSetCropXY(20*X,20*Y);
    g2dAdd();
    g2dEnd();
}
else if(MousePos.Tile == 35){
//DrawTexturePro(TileSheet, (Rectangle){(int)(420),(int)(140),(int)20,(int)20},(Rectangle){MousePos.x*20,MousePos.y*20,20,20},(Vector2){0,0},0,WHITE);
    g2dBeginRects(TileSheet);
    g2dResetCrop();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(20,20);
    g2dSetCoordXY((MousePos.x*20)-CamX,(MousePos.y*20)-CamY);
    g2dSetCropWH(20,20);
    g2dSetCropXY(420,140);
    g2dAdd();
    g2dEnd();
}
else{
//DrawTexturePro(TileSheet, (Rectangle){(int)(380),(int)(140),(int)20,(int)20},(Rectangle){MousePos.x*20,MousePos.y*20,20,20},(Vector2){0,0},0,WHITE);
    g2dBeginRects(TileSheet);
    g2dResetCrop();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(20,20);
    g2dSetCoordXY((MousePos.x*20)-CamX,(MousePos.y*20)-CamY);
    g2dSetCropWH(20,20);
    g2dSetCropXY(380,140);
    g2dAdd();
    g2dEnd();
}


    g2dBeginRects(TileSheet);
    g2dResetCrop();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(20,20);
    g2dSetCoordXY((MousePos.x*20)-CamX,(MousePos.y*20)-CamY);
    g2dSetCropWH(20,20);
    g2dSetCropXY(460,140);
    g2dAdd();
    g2dEnd();

//DrawTexturePro(TileSheet, (Rectangle){460,140,20,20},(Rectangle){MousePos.x*20,MousePos.y*20,20,20},(Vector2){0,0},0,WHITE);


}
LeftMousePressed = false;
RightMousePressed = false;
//---------------------------------------------------------

  //Draw the UI
      //-------------------------------------------------------------------------------------- 


    g2dBeginRects(CurrentBackGround);
    g2dResetCrop();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(233,26);
    g2dSetCoordXY(0,0);
    g2dSetCropWH(233,26);
    g2dSetCropXY(0,0);
    g2dAdd();
    g2dEnd();
       
        g2dBeginRects(CurrentBackGround);
    g2dResetCrop();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(26,26);
    g2dSetCoordXY(239,0);
    g2dSetCropWH(26,26);
    g2dSetCropXY(200,138);
    g2dAdd();
    g2dEnd();

       g2dBeginRects(CurrentBackGround);
    g2dResetCrop();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(20,20);
    g2dSetCoordXY(242,3);
    g2dSetCropWH(20,20);
    g2dSetCropXY(SquareSelectorX*20,118+SquareSelectorY*20);
    g2dAdd();
    g2dEnd();
//DrawTexturePro(CurrentBackground, (Rectangle){0,0,233,26},(Rectangle){(MapView.X),(MapView.Y),233,26},(Vector2){0,0},0,WHITE);
//DrawTexturePro(CurrentBackground, (Rectangle){200,138,26,26},(Rectangle){(MapView.X) +239,(MapView.Y) ,26,26},(Vector2){0,0},0,WHITE);
//DrawTexturePro(CurrentBackground, (Rectangle){SquareSelectorX*20,118+SquareSelectorY*20,20,20},(Rectangle){(MapView.X) +242,(MapView.Y)+3 ,20,20},(Vector2){0,0},0,WHITE);



if(OnSquareSelector == true){
//List
//DrawTexturePro(CurrentBackground, (Rectangle){SquareSelectorX*22,26,22,92},(Rectangle){(MapView.X) + (2+(SquareSelectorX*22) + (1 * SquareSelectorX)),(MapView.Y) + 26,22,92},(Vector2){0,0},0,WHITE);
 g2dBeginRects(CurrentBackGround);
    g2dResetCrop();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(22,92);
    g2dSetCoordXY((2+(SquareSelectorX*22) + (1 * SquareSelectorX)),26);
    g2dSetCropWH(22,92);
    g2dSetCropXY(SquareSelectorX*22,26);
    g2dAdd();
    g2dEnd();
//Selector
//DrawTexturePro(CurrentBackground, (Rectangle){200,118,20,20},(Rectangle){(MapView.X) + (3+(SquareSelectorX*20) + (3 * SquareSelectorX)),(MapView.Y) + (3+(SquareSelectorY*20) + (3 * SquareSelectorY)),20,20},(Vector2){0,0},0,WHITE);

 g2dBeginRects(CurrentBackGround);
    g2dResetCrop();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(20,20);
    g2dSetCoordXY((3+(SquareSelectorX*20) + (3 * SquareSelectorX)),(3+(SquareSelectorY*20) + (3 * SquareSelectorY)));
    g2dSetCropWH(20,20);
    g2dSetCropXY(200,118);
    g2dAdd();
    g2dEnd();



}
else{
    //Selector
//DrawTexturePro(CurrentBackground, (Rectangle){200,118,20,20},(Rectangle){(MapView.X) +242,(MapView.Y) +3,20,20},(Vector2){0,0},0,WHITE);



       g2dBeginRects(CurrentBackGround);
    g2dResetCrop();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(20,20);
    g2dSetCoordXY(242,3);
    g2dSetCropWH(20,20);
    g2dSetCropXY(200,118);
    g2dAdd();
    g2dEnd();
}

if(MessageCounter != 0){
MessageCounter--;
//DrawTexturePro(CurrentBackground, (Rectangle){233,9*Message,192,9},(Rectangle){(MapView.X) +295,(MapView.Y) + 9,192,9},(Vector2){0,0},0,WHITE);


   g2dBeginRects(CurrentBackGround);
    g2dResetCrop();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(192,9);
    g2dSetCoordXY(295,9);
    g2dSetCropWH(192,9);
    g2dSetCropXY(233,9*Message);
    g2dAdd();
    g2dEnd();
}

if(filltobottom == true){
//DrawTexturePro(CurrentBackground, (Rectangle){233,81,192,9},(Rectangle){(MapView.X) +295,(MapView.Y) + 9,192,9},(Vector2){0,0},0,WHITE);
   g2dBeginRects(CurrentBackGround);
    g2dResetCrop();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(192,9);
    g2dSetCoordXY(295,9);
    g2dSetCropWH(192,9);
    g2dSetCropXY(233,81);
    g2dAdd();
    g2dEnd();
}

//DrawTexturePro(CurrentBackground, (Rectangle){233,0,192,9},(Rectangle){(MapView.X) +295,(MapView.Y) ,192,9},(Vector2){0,0},0,WHITE);
    g2dBeginRects(CurrentBackGround);
    g2dResetCrop();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(192,9);
    g2dSetCoordXY(295,0);
    g2dSetCropWH(192,9);
    g2dSetCropXY(233,0);
    g2dAdd();
    g2dEnd();
 
 
  int CurrentLevel2 = CurrentLevel - 1;
  for (int i = 0; i < 4; i++)
    {
//DrawTexturePro(CurrentBackground, (Rectangle){228,26+8 * (CurrentLevel2  % 10),5,8},(Rectangle){(MapView.X) + 410 -9*i,(MapView.Y) ,5,8},(Vector2){0,0},0,WHITE);
g2dBeginRects(CurrentBackGround);
    g2dResetCrop();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(5,8);
    g2dSetCoordXY(410 -9*i,0);
    g2dSetCropWH(5,8);
    g2dSetCropXY(228,26+8 * (CurrentLevel2  % 10));
    g2dAdd();
    g2dEnd();
    CurrentLevel2 = CurrentLevel2 / 10;
  //Power = Power * 10;
    }

if(EditedLayer == 2){
        g2dBeginRects(CurrentBackGround);
    g2dResetCrop();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(26,14);
    g2dSetCoordXY(267,0);
    g2dSetCropWH(26,14);
    g2dSetCropXY(425,0);
    g2dAdd();
    g2dEnd();
}
else{
           g2dBeginRects(CurrentBackGround);
    g2dResetCrop();
    g2dSetTexLinear(false);
    g2dSetCoordMode(G2D_UP_LEFT);
    g2dSetAlpha(255);
    g2dSetScaleWH(26,14);
    g2dSetCoordXY(267,0);
    g2dSetCropWH(26,14);
    g2dSetCropXY(425,14);
    g2dAdd();
    g2dEnd();
}

//DrawTexturePro(CurrentBackground, (Rectangle){200,118,20,20},(Rectangle){(MapView.X) + (3+(SquareSelectorX*20) + (3 * SquareSelectorX)),(MapView.Y) + (3+(SquareSelectorY*20) + (3 * SquareSelectorY)),20,20},(Vector2){0,0},0,WHITE);


  //  EndDrawing();
    g2dFlip(G2D_VSYNC);
    LeftMousePressed = false;
    RightMousePressed = false;

}
      


if(Botones[SELECT].IsPressed == 1 && GameState != 10&& GameState != 11){
    if(DeveloperMode == true){
    DeveloperMode = false;
GameState = 4;
    CurrentBackGround = NULL;

    }
    else{
    DeveloperMode = true;
    }
    CurrentBackGround = NULL;
}


    t = clock() - t; 
  //  deltaTime = ((double)t)/1000;



      //sceKernelDelayThread(15000);
 
  //  deltaTime = ((double)t)/CLOCKS_PER_SEC; // in seconds 

        //--------------------------------------------------------------------------------------




//--------------------------------------------------------------------------------------
   
    }
    // De-Initialization
    //--------------------------------------------------------------------------------------

PlayerTexture = NULL;
TileSheet = NULL;
GradientTexture = NULL;
Menus = NULL;
CurrentBackGround = NULL;
   
  //  UnloadTexture(PlayerTexture);
  //  UnloadTexture(TileSheet);
  //  UnloadTexture(GradientTexture);
   
  //  CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}






//In game map management
//--------------------------------------------------------------------------------------


int ResetPlayerAndFlagPos(){
    Enemies = 0;
    EnemiesBlockers = 0;
    AmountOfTrampolines = 0;
    CurrentPlayerAnimStateH = 0;
    CurrentPlayerAnimStateV = 2;
    
    int CurentI = 0;
        for (int i = 0; i < MapWidth; i++)
        {
            CurentI = i;
         for (int e = 0; e < MapHeight; e++)
         {
        if(Map2[e][CurentI] == '#')
           {
           Player.X = CurentI*20;
           Player.Y = e*20;
           }
        if(Map2[e][CurentI] == '$')
           {
           Flag.X = CurentI*20;
           Flag.Y = e*20;
           }
        if(Map2[e][CurentI] == 216||Map2[e][CurentI] == 232||Map2[e][CurentI] == 229 || Map[e][CurentI] == 216||Map[e][CurentI] == 232||Map[e][CurentI] == 229)
           {
            Enemies++;
           }
        if(Map2[e][CurentI] == 215 || Map[e][CurentI] == 215)
           {
            EnemiesBlockers++;
           }
        if(Map2[e][CurentI] == 226 || Map[e][CurentI] == 226)
           {
            Trampolines[AmountOfTrampolines].X = CurentI*20;
            Trampolines[AmountOfTrampolines].Y = e*20;
            AmountOfTrampolines++;
           // EnemiesBlockers++;
           }
       
         }
        }

if(SpawnPos.x != -20){
    Player.X = SpawnPos.x;
    Player.Y = SpawnPos.y - 30;
}


        EnemyArrayReset();


        if(Enemies != 0){
   //         Enemies++;
        }
          if(EnemiesBlockers != 0){
            EnemiesBlockers++;
        }
 //   EnemyArrayStart(Enemies);
//  AddEnemiesIntoArray();
   // ArrayReset();
    }
//--------------------------------------------------------------------------------------

int EnemyArrayReset(){

    for (int i = 0; i < 41; i++)
    {
        Enemies1[i].X = 0;
        Enemies1[i].Anim = 0;
        Enemies1[i].SpeedX = -1;
        Enemies1[i].SpeedY = 0;
        Enemies1[i].Y = 0;
    }
  for (int i = 0; i < 85; i++)
    {
         EnemyBlockers[i].x = 0;
         EnemyBlockers[i].y = 0;
    }



    
int W = 0;
int F = 0;
    int CurentI = 0;
        for (int i = 0; i < MapWidth; i++)
        {
            CurentI = i;
         for (int e = 0; e < MapHeight; e++)
         {
           if(Map2[e][CurentI] == 216)
           {

               Enemies1[W].X = CurentI * 20;
               Enemies1[W].Y = e * 20;
Enemies1[W].Type = 0;
           // AddEnemyIntoArray(CurentI*20,e*20);
           W++;
           }
           if(Map2[e][CurentI] == 232){
               Enemies1[W].X = CurentI * 20;
               Enemies1[W].Y = e * 20;
Enemies1[W].Type = 1;
           // AddEnemyIntoArray(CurentI*20,e*20);
           W++;
           }
            if(Map2[e][CurentI] == 229){
               Enemies1[W].X = CurentI * 20;
               Enemies1[W].Y = e * 20;
               Enemies1[W].OriginalY = Enemies1[W].Y;
Enemies1[W].Type = 2;
           // AddEnemyIntoArray(CurentI*20,e*20);
           W++;
           }
           if(Map2[e][CurentI] == 215)
           {

            EnemyBlockers[F].x = CurentI * 20;
            EnemyBlockers[F].y = e * 20;
            // AddEnemyIntoArray(CurentI*20,e*20);
            F++;
           }
         }
        }
W = 0;
return 0;

}

int CleanEnemyArray(){
     for (int i = 0; i < 41; i++)
    {
        Enemies1[i].X = -20;
        Enemies1[i].Anim = 0;
        Enemies1[i].SpeedX = -1;
        Enemies1[i].SpeedY = 0;
        Enemies1[i].Y = -20;
    }
  for (int i = 0; i < 85; i++)
    {
         EnemyBlockers[i].x = 0;
         EnemyBlockers[i].y = 0;
    }

}

int LoadOverWorld(){


FILE *fp = fopen("resources/OverWorld/Levels.txt","r");

    int CurrentI = 0;
    int e = 0;
  int c;
       while ( (c = fgetc(fp)) != EOF ) {

            if(c != '\n'){
            OverWorldSquares[e][CurrentI].LevelToLoad = c;
               e++;
            }
            else{
               CurrentI ++;
               e = 0;
            }

}
    fclose(fp);

c = 0;
CurrentI = 0;
e = 0;
fp = fopen("resources/OverWorld/State.txt","r");

     while ( (c = fgetc(fp)) != EOF && c != '@') {

            if(c != '\n'){
            OverWorldSquares[e][CurrentI].State = c;
               e++;
            }
            else{
               CurrentI ++;
               e = 0;
            }

}
 

if(c == '@'){
    c = fgetc(fp);
    NextPlayableLevel = c - 65;
}
 fclose(fp);
    return 0;
}

int LoadOverWorldFromOriginal(){


FILE *fp = fopen("resources/OverWorldOriginal/State.txt","r");

    int CurrentI = 0;
    int e = 0;
    char c;


     while ( (c = fgetc(fp)) != EOF) {

            if(c != '\n'){
            OverWorldSquares[e][CurrentI].State = c;
               e++;
            }
            else{
               CurrentI ++;
               e = 0;
            }

}
fclose(fp);
NextPlayableLevel = 0;
    return 0;
}

int SaveData(){


FILE *fp = fopen("resources/OverWorld/State.txt","w");
int CurrentI = 0;
for (int i = 0; i < 16; i++)
{
  CurrentI = i;
  
for (int e = 0; e < 29; e++)
{

fputc(OverWorldSquares[e][CurrentI].State, fp);
}
fputc('#', fp);
fputc('\n', fp);
}
//fputc(OverWorldSquares[1][1].State, fp);
fputc('@', fp);
fputc(NextPlayableLevel+65, fp);
fclose(fp);



}

int  SaveCoinsAndLives(int coins, int lives, int PlayedSequence1, int PlayedENDSequence){
FILE *fp = fopen("resources/OverWorld/Lives.txt","w");

  putw(coins, fp);
  putw(lives, fp);
  putw(PlayedSequence1, fp);
  putw(PlayedENDSequence, fp);
  fclose(fp);


return 0;
}

int LoadCoins(){
    int coins;
    FILE *fp = fopen("resources/OverWorld/Lives.txt","r");
    coins = getw(fp);
    fclose(fp);

    return coins;
}
int LoadLives(){
       int lives;
    FILE *fp = fopen("resources/OverWorld/Lives.txt","r");
    getw(fp);
    lives = getw(fp);
    fclose(fp);

    return lives;
}
int LoadSequence(){
       int Sequence;
    FILE *fp = fopen("resources/OverWorld/Lives.txt","r");
    getw(fp);
    getw(fp);
    Sequence = getw(fp);
    fclose(fp);

    return Sequence;
}
int LoadEndSequence(){
       int Sequence;
    FILE *fp = fopen("resources/OverWorld/Lives.txt","r");
    getw(fp);
    getw(fp);
    getw(fp);
    Sequence = getw(fp);
    fclose(fp);

    return Sequence;
}

int IsSolid(int Character){
 //<time.h>

 
   if(Character >= 65 && Character < 107){
   return 1;
   }  
}
int IsRamp(int DetectedCharacter){
if(DetectedCharacter >= 107 && DetectedCharacter < 131){
return 1;
}
else{
    return 0;
}
}
int IsLeftRamp(int DetectedCharacter){
if(DetectedCharacter >= 119 && DetectedCharacter < 125){
return 1;
}

}
int IsLeftRampSupport(int DetectedCharacter){
    if(DetectedCharacter >= 125 && DetectedCharacter < 131){
        return 1;
    }
}
int IsRightRamp(int DetectedCharacter){
if(DetectedCharacter >= 107 && DetectedCharacter < 113){
    return 1;
}
}
int IsRightRampSupport(int DetectedCharacter){
    if(DetectedCharacter >= 113 && DetectedCharacter < 119){
        return 1;
    }
}

