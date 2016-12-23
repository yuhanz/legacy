#ifndef _MAP_FUNCTIONS_
#define _MAP_FUNCTIONS_

#include <windows.h>
#include "Map.h"
#include <list>

using namespace std;

typedef list<POINT> PointList;

bool operator==(POINT,POINT);		// Dummy functions.
bool operator>(POINT,POINT);		// They are defined to let
bool operator<(POINT,POINT);		// POINT fit for a list template.
bool operator!=(POINT,POINT);

list<POINT>* FindShortestPath(GridMap& map, POINT iniPos, POINT destPos);


#endif