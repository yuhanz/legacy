#ifndef _GAME_45_
#define _GAME_45_

#include "Map.h"
#include "TSprite.h"
#include "common.h"
#include "Map.h"
#include "Utility.h"

class GameMap45;
class GameUnit;
class ScreenObject;
class Effect;

///////////////////////////////////////////////////////////////////////////

class GameUnit : public MapUnit
{
	public:
		Walk_TSprite2 appearance;
		static GameMap45* theMap;

		virtual ~GameUnit();
		POINT getScreenPosition();
		void draw(IDirectDrawSurface*);
		void update();
		void updateMovement();

		ScreenObject getScreenObject();
		float getDistance();
		Effect* createEffect(Tile*,int frameNum);

		void setPosition(int x,int y);
};





/*-------------------------------------------------
-- class GameMap45
--
--		This class is derived from 'Map45' and 'GridMap'.
-- It keeps a direct draw surface as a source and a series
-- of tile. According to the information on the grid map,
-- (the map that's read from the text file) this map will 
-- lay itself on the screen tile by tile. The tile that
-- is 'closer' to the screen will overlying the ones behind.
---------------------------------------------------
*/

class GameMap45 : public Map45, public GridMap
		{
		private:
			int floorTypes;
			int blockTypes;
			Tile* floorTile;
			Tile* blockTile;
			ArrayList objectList;

			POINT rightCellOf(POINT);
			POINT leftCellOf(POINT);
			void drawCell(IDirectDrawSurface*,POINT);

		public:
			
			GameMap45() : Map45(0,0) {}
			virtual ~GameMap45();
			void drawMap(IDirectDrawSurface*);
			void setTiles(int,Tile*,int,Tile*);
			
			void addScreenObject(ScreenObject*, float);
			void renewScreenObjectList(int);
			//bool checkAccess(POINT position,POINT direction);
		};



/*------------------------------------------------------------
--	ScreenObject:
--	
--	  ScreenObject has a pointer to a tile and a screen position
--	to draw the tile. It won't store the tile through the pointer.
------------------------------------------------------------*/
class ScreenObject
{
	public:
		Tile* obj;		   // which tile to use.
		POINT position;    // relative screen position to draw the given tile (upper-left corner)

		ScreenObject();
		ScreenObject(Tile*,int,int);		
		void drawOn(IDirectDrawSurface*);
};

/*-----------------------------------------------------
--	Effect:
--
--	* Effect has a grid position, indicating its position
--	  on the map.
--	* The speed value is used to move it around on the map.
--	* It also keeps a dynamically allocated sprite, and uses it
--	  as a source of drawing.
--	* The map is known in order to draw on the screen properly.
--	* zShift value will be used to draw the Effect "in the air",
--	  instead of sticking on the floor of the map.
--
-----------------------------------------------------*/
class Effect
{
	public:
					// grid position, x and y
		float x;	
		float y;
		int zShift;	// Height to the ground ( in pixel height )

		float sx;	// speed along x
		float sy;	// speed along y

		Series_TSprite* sprite;
		static GameMap45* theMap;
		
		Effect(float,float,int,Series_TSprite*);
		Effect() {}
		virtual ~Effect();
		ScreenObject getScreenObject();
		void updateMovement();
		float getDistance();
		void setSpeed(float _sx,float _sy);

		void putInBackground();
		void putInForeground();
};


/*-----------------------------------------------------
--	ProcessableEffect: (abstract class)
--
--		Basically an effect, but having a "process()" fucntion
--	to centralize the update of the movement and return whether
--	the Effect expires.
--
--		"process()" will return a value to indicate expirancy.
--		true means expired.
--		flase means not expired.
-----------------------------------------------------*/
class ProcessableEffect : public Effect, public Processable
{
	public:
		ProcessableEffect(float,float,int,Series_TSprite*);
		virtual ~ProcessableEffect();
};



/*-----------------------------------------------------
--	DiminishingEffect
--			(A child of 'ProcessableEffect')
--
--		'DiminishingEffect' expires when the sprite
--	stops.
-----------------------------------------------------*/
class DiminishingEffect : public ProcessableEffect
{
	public:
		DiminishingEffect(float,float,int,Series_TSprite*);
		virtual ~DiminishingEffect();
		bool process();
};


/*-----------------------------------------------------
--	DistanceSensitiveEffect
--			(A child of 'ProcessableEffect')
--
--		'DistanceSensitiveEffect' expires when the effect
--  moves exceeding a certain distance (in grid unit).
-----------------------------------------------------*/
class DistanceSensitiveEffect : public ProcessableEffect
{
	private:
		int maxDistance;
		float oldX;
		float oldY;
		
	public:
		
		DistanceSensitiveEffect(float,float,int,Series_TSprite*,int);
		virtual ~DistanceSensitiveEffect();
		bool process();
};

#endif