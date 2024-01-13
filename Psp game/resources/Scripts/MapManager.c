#include <stdio.h>
#include <pspkernel.h>
#include <stdlib.h>  
#include <string.h>
#include <stdbool.h>
#include <resources\Scripts\MapManager.h>




int CurrentLevel = 0;
int MaxLevels = 0;
int Amount;
char MapName[11]="Lvl000.txt";
int r;
int i;

int Map [MapHeight] [MapWidth] = {
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     };
int Map2[MapHeight] [MapWidth] = {
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     {'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'},
     };
//Little joke, the game calls map2 Layer 1, and map1 Layer2

//Checks if a file exists
//--------------------------------------------------------------------------------------
bool File_Exists(const char *fileName){
FILE *fp = fopen(fileName, "r");
bool Exist = false;
if(fp != NULL){
Exist = true;
}
fclose(fp);
    return Exist;
}
//--------------------------------------------------------------------------------------


//In game map management
//--------------------------------------------------------------------------------------
int Map2ToMap1(){
}
int AddSlopeSigns(){
    int CurentI = 0;
        for (int i = 0; i < MapHeight; i++)
        {
            CurentI = i;
         for (int e = 0; e < MapWidth; e++)
         {

            if(    Map2[e][CurentI] == '!' || Map2[e][CurentI] == '1'){
               Map2[e][CurentI] = '.';         
          }
         }
  //  putw(0, fp);

        }
        CurentI = 0;
        for (int i = 0; i < MapHeight; i++)
        {
                CurentI = i;
         for (int e = 0; e < MapWidth; e++)
         {
            if(Map2[CurentI][e] >= 119 && Map2[CurentI][e] < 125){
              Map2[CurentI-1][e] = '!';         

            }

             if(Map2[CurentI][e] >= 107 && Map2[CurentI][e] < 113){
              Map2[CurentI-1][e] = '1';         

            }

            /* code */
        }
        }
        
}


int ClearMap(){
        int CurentI = 0;
        for (int i = 0; i < MapHeight; i++)
        {
            CurentI = i;
        for (int e = 0; e < MapWidth; e++)
        {
               Map2[CurentI][e] = '.';             
               Map[CurentI][e] = '.';             
      //         Map2[e][CurentI] = '.';             
        }
        }
        
}

//--------------------------------------------------------------------------------------



//Map data Management
//--------------------------------------------------------------------------------------
int UpdateString(int Lvl){

    if(Lvl > 99){
    MapName[3] = ReturnDigit(Lvl,0) + '0';//Lvl345
    MapName[4] = ReturnDigit(Lvl,1) + '0';
    MapName[5] = ReturnDigit(Lvl,2) + '0';  
    }
    else if(Lvl > 9 ){
    MapName[3] = 0 + '0';//Lvl345
    MapName[4] = ReturnDigit(Lvl,0) + '0';
    MapName[5] = ReturnDigit(Lvl,1) + '0';
    }
    else{
    MapName[3] = 0 + '0';
    MapName[4] = 0 + '0';
    MapName[5] = ReturnDigit(Lvl,0) + '0'; 
    }
 //   strcat(MapName,".txt");
 if(Lvl == 0){
   MapName[3] = 0 + '0';
   MapName[4] = 0 + '0';
   MapName[5] = 0 + '0';
 }
//printf("%s\n",MapName);
}
int ReturnDigit(int Num, int Digit)
{
int * FirstArr;
FirstArr = malloc(sizeof(int)*20);
i = 0;
while(Num > 0)
{
    *(FirstArr+i) = Num % 10;
    i++;
    //printf("%d\n", Num % 10);
    Num /= 10;
}
int * SecondArray;
SecondArray = malloc(sizeof(int)*20);
r = 0;
for (int j = i-1; j > - 1; j --){
    
   *(SecondArray+r) = *(FirstArr+j);
r++; 
}

for(int i = 0; i < 20; i++){
    //  printf("%d R ",*(SecondArray+i));
    // printf(" %d \n" , i);
     if(i == Digit){
         r = *(SecondArray+i);
     }
}
//printf(" End \n" , i);
FirstArr = NULL;
SecondArray = NULL;

return r;
}
int CheckNumberOfFiles()
{
UpdateString(Amount);
char *file;
file = malloc(25*sizeof(char));
strcpy(file,"resources/Levels/");
//fileHelp = "resources/Levels/";
strcat(file,MapName);
//printf("%s\n",file);
//free(file);

for (int i = 0; i < 100; i++)
{
Amount++;
UpdateString(Amount);
file = malloc(25*sizeof(char));
strcpy(file,"resources/Levels/");
//fileHelp = "resources/Levels/";
strcat(file,MapName);
if(File_Exists(file) == false){
i = 1000;
}
//printf("%s\n",file);
  //free(file);
}
file = NULL;
//free(file);
return Amount;

}
int LoadMap(int LevelToLoad){
 if(LevelToLoad < 0){
    LevelToLoad = 0;
 }
 UpdateString(LevelToLoad);
char *fileHelp;
fileHelp = malloc(25*sizeof(char));
strcpy(fileHelp,"resources/Levels/");
strcat(fileHelp,MapName);
    FILE *fp = fopen(fileHelp,"r");
   
        int CurentI = 0;
        int e = 0;
    int num; 
      while ( (num = getw(fp)) != 64 && num  != EOF ) {
    //printf("%d\n", num);


        
            if(num != 0){
            Map2[CurentI][e] = num;
               e++;
            }
            else{
               CurentI ++;
               e = 0;
            }

}

e = 0;
CurentI = 0;
      while ( (num = getw(fp)) != EOF ) {
            if(num != 0){
            Map[CurentI][e] = num;
               e++;
            }
            else{
               CurentI ++;
               e = 0;
            }

}


 
    fclose(fp);
}


int SaveMap(){
    AddSlopeSigns();
   int Lv;
   Lv = CheckNumberOfFiles() - 1;
MaxLevels = Lv;
UpdateString(Lv);
char *fileHelp;
fileHelp = malloc(25*sizeof(char));
strcpy(fileHelp,"resources/Levels/");
strcat(fileHelp,MapName);


   
    int i = 0;
    FILE *fp = fopen(fileHelp,"w");
 
    // Return if could not open file
    if (fp == NULL)
      return 0;//free(fileHelp);
 

        // Input string into the file
        // single character at a time
       //   fputc(string[i], fp);
        int CurentI = 0;
        for (int i = 0; i < MapHeight; i++)
        {
            CurentI = i;
         for (int e = 0; e < MapWidth; e++)
         {

            putw(Map2[CurentI][e], fp);
         //   fputc(Map2[CurentI][e], fp);
    //    fwrite(Map2 , 1 , sizeof(int) , fp);

         }
         
       // fputc('\n', fp);
    putw(0, fp);

        }


            putw(64, fp);
        
         CurentI = 0;
        for (int i = 0; i < MapHeight; i++)
        {
            CurentI = i;
         for (int e = 0; e < MapWidth; e++)
         {

            putw(Map[CurentI][e], fp);
         //   fputc(Map2[CurentI][e], fp);
    //    fwrite(Map2 , 1 , sizeof(int) , fp);

         }
    putw(0, fp);

        }
    fclose(fp);
    fileHelp = NULL;
    LoadMap(Lv);
    CurrentLevel = Lv+1;
    Map2ToMap1();
    ResetPlayerAndFlagPos();
}



int SaveMap2(int Lv){
    AddSlopeSigns();

UpdateString(Lv);
char *fileHelp;
fileHelp = malloc(25*sizeof(char));
strcpy(fileHelp,"resources/Levels/");
//fileHelp = "resources/Levels/";
strcat(fileHelp,MapName);
//printf("%s\n",fileHelp);

   
    int i = 0;
    FILE *fp = fopen(fileHelp,"w");
 
    // Return if could not open file
    if (fp == NULL)
      return 0;//free(fileHelp);
 

        // Input string into the file
        // single character at a time
       //   fputc(string[i], fp);
        int CurentI = 0;
        for (int i = 0; i < MapHeight; i++)
        {
            CurentI = i;
         for (int e = 0; e < MapWidth; e++)
         {

            putw(Map2[CurentI][e], fp);
         }
    putw(0, fp);

        }

            putw(64, fp);

         CurentI = 0;
        for (int i = 0; i < MapHeight; i++)
        {
            CurentI = i;
         for (int e = 0; e < MapWidth; e++)
         {

            putw(Map[CurentI][e], fp);
         //   fputc(Map2[CurentI][e], fp);
    //    fwrite(Map2 , 1 , sizeof(int) , fp);

         }
    putw(0, fp);

        }   
   
    fclose(fp);
    fileHelp = NULL;
}

//--------------------------------------------------------------------------------------
