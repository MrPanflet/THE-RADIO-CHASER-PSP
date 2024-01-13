#ifndef MAPMANAGER_H_INCLUDED
#define MAPMANAGER_H_INCLUDED

#define MapWidth 475
#define MapHeight 75

int CurrentLevel;
int MaxLevels;

/* Loads the map */
int LoadMap(int LevelToLoad);

/* Turns map1 into map2 */
int Map2ToMap1();

/* Clears map 2 */
int ClearMap();

/* This map is for gameplay*/
int Map [MapHeight] [MapWidth];

/* This map is for loading*/
int Map2 [MapHeight] [MapWidth];


/* This funcion overrides the selected level*/
int SaveMap2(int Lv);

/* This funcion creates a new level*/
int SaveMap();





#endif