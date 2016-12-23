#ifndef MAP_H
#define MAP_H

#include <windows.h>
#include <fstream.h>
#include <ddraw.h>

#include "Tile.h"

/*--------------------------------------------------------
--	GridMap
--		A grid map that includes information about floor
--  type and block type
--------------------------------------------------------*/
class GridMap
{
	
               POINT size;
			   int* floor;
               int* block;


      public:
               GridMap();
			   void releaseMemory();

			   int point2Index(int,int);
               POINT getSize();
			   int getFloor(int,int);		// Retrieve floor info at (x,y).
			   int getBlock(int,int);		// Retrieve blcok info at (x,y).
			   void setMapSize(int,int);
			   void setFloorMap(int*);		// Attach a floor map.
			   void setBlockMap(int*);		// Attach a block map.
											// from the floor and blcok arrays.
			   void setFloor(int,int,int);
			   void setBlock(int,int,int);
			   bool checkAccess(POINT position,POINT direction);
			   bool withinBound(int x, int y);	// if a point is in the bound.

               void loadMap(char* fileName);
               void saveMap(char* fileName);

};




/*-------------------------------------------------
-- class Map45
--		This class records all the necessary information
-- and functions to draw a grid map from 45 degree visual
-- point.
---------------------------------------------------
*/


class Map45
{
	private:
		POINT floorSize;	// x is the width of the dimension of a floor image.
							// y is the height of the dimension of a floor image.
		
							// y is the height of the dimension of the grid map
		POINT initialPos;	// screen position of the upper-left corner for the first cell (0,0) in the grid map.

	
	public:

	
		Map45(int iniX, int iniY);
		void setInitialPosition(int, int);	// set the drawing position for the cell (0,0)
		POINT getInitialPosition();			// get the drawing position for the cell (0,0)
		void setFloorSize(int, int);		// set the pixel size for a floor image.
		POINT getFloorSize();

		POINT getFloorPosition(int, int);	// get the screen position of a floor according to its grid position.
		POINT getObjectPosition(int, int, int, int);	// get the screen position of a object according to its grid position.
		POINT getAdjustedPosition(int,int,int, int,int);	// get adjusted object position.
		void drawFloor(Tile,IDirectDrawSurface*, int,int);
		void drawObject(Tile,IDirectDrawSurface*, int,int);

		POINT getFloorFloatPosition(float,float);
		POINT getObjectFloatPosition(float, float, int, int);	// get the screen position of a object according to its grid position.
		POINT getAdjustedFloatPosition(float,float,int, int, int);	// get adjusted object position.

};



/*------------------------------------------------------------------------
 MapUnit:
         A unit that will move on the grid map.

         A movement on the map will be set into steps.
         In each movement, the current step will be incremented by 1.
         When the current step reaches the steps required to across a cell,
         the unit will be move to the next cell, according to the direction
         it's moving into. Current step will be set to 0.

         Intermedia grid position can be reckoned from the current position,
         the direction, steps that are required to across and current step.
         It's useful to display the transitional animation on the screen.
------------------------------------------------------------------------*/
class MapUnit
{
 protected:
         POINT position;        // grid position on the map
         int stepsAcross;       // steps that are required to move the unit to the adjacent map cell
         int currentStep;       // which step the unit is in. Being 0 means it's in the original position. The larger the value, the further away from this cell.
         POINT direction;       // which direction is this unit is going for.
         POINT newDirection;    // the next direction that will be traced after current movement is done.
                                // ('(newDirection.x == 0 && newDirection.y==0)==true" means no input. 'newDirection' will replace 'direction' when 'currentStep'=='stepsAcross'.
 public:

		 enum {EMPTY=-1,BLOCKED=-2,REGULAR_STEPS=8};

		 MapUnit();
		 MapUnit(POINT pos, int steps);
         float getFloatX();     // grid position in float form
         float getFloatY();     // grid position in float form
         POINT nextCell();      // which cell is it aheading to?
         bool anyNewDirection();// if there is any new input.
		 POINT getDirection();
		 POINT getPosition();
		 int getCurrentStep();

         virtual void setDirection(POINT);
		 void setStepsAcross(int);
		 void setPosition(int,int);
		 int distanceFrom(MapUnit&);	// return cell distance between this unit and the input unit.
		 int distanceFrom(int x, int y);// return cell distance from a point.
         virtual void updateMovement(); // update 'currentStep'. Change 'direction' and set 'currentStep' 0 if a cell is reached.
};




#endif