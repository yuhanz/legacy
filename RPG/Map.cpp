#include "map.h"

///////////////////////////////////////////////////////////
///			implementation for 'Map45'
///////////////////////////////////////////////////////////

//--- width and height are size of the grid map. ---
Map45::Map45(int iniX, int iniY)
{
	initialPos.x = iniX;
	initialPos.y = iniY;

	floorSize.x = 64;		// default floor size.
	floorSize.y = 32;
}





POINT Map45::getFloorPosition(int gx, int gy)
{ 
	POINT screenPos;

	screenPos.y = initialPos.y + floorSize.y * (gx+gy)/2;
	screenPos.x = initialPos.x + floorSize.x * (gx-gy)/2;
	
	return screenPos;
}


POINT Map45::getFloorFloatPosition(float gx, float gy)
{ 
	POINT screenPos;

	screenPos.y = initialPos.y +(int)( floorSize.y * (gx+gy)/2);
	screenPos.x = initialPos.x +(int)( floorSize.x * (gx-gy)/2);
	
	return screenPos;
}
		



void Map45::setFloorSize(int w, int h)
{
	floorSize.x=w;
	floorSize.y=h;
}

POINT Map45::getFloorSize()
{ return floorSize;}


//+++ Input:	gx and gy is the grid position of the object,
//+++			the size of the object is given. (in pixel value)
//+++ Output:	The drawing position for the given object.
POINT Map45::getObjectPosition(int gx, int gy, int width, int height)
{
	POINT fp = getFloorPosition(gx, gy);
	fp.x += (floorSize.x - width)/2;
    fp.y -= height - floorSize.y;
	return fp;
}


POINT Map45::getObjectFloatPosition(float gx, float gy, int width, int height)
{
	POINT fp = getFloorFloatPosition(gx, gy);
	fp.x += (floorSize.x - width)/2;
    fp.y -= height - floorSize.y;
	return fp;
}


//+++ Input:	gx and gy is the grid position of the object,
//+++			the size of the object is given. (in pixel value)
//+++ Output:	The drawing position for the given object, but adjusted
//+++			the buttom to the center of the floor tile, instead
//+++			of the buttom of the floor tile.
POINT Map45::getAdjustedPosition(int gx, int gy, int width, int height, int zShift)
{
	POINT fp = getFloorPosition(gx, gy);
	fp.x += (floorSize.x - width)/2;
	fp.y -= height-floorSize.y + zShift;
	return fp;
}

POINT Map45::getAdjustedFloatPosition(float gx, float gy, int width, int height, int zShift)
{
	POINT fp = getFloorFloatPosition(gx, gy);
	fp.x += (floorSize.x - width)/2;
	fp.y -= height-floorSize.y + zShift;
	return fp;
}



//+++ Input:	The drawing position of the floor image in the
//+++			first cell
//+++ Output:	The position will be recorded in 'initialPos'.
void Map45::setInitialPosition(int sx, int sy)
{
	initialPos.x = sx;
	initialPos.y = sy;
}


POINT Map45::getInitialPosition()
{ return initialPos;}


//+++ draw a tile of floor image to the given surface at specified position.
void Map45::drawFloor(Tile t,IDirectDrawSurface* board, int gx,int gy)
{
	
	POINT fp = getFloorPosition(gx,gy);
	t.draw(board, fp.x, fp.y);
}

//+++ draw a tile of floor image to the given surface at specified position.
void Map45::drawObject(Tile t,IDirectDrawSurface* board, int gx,int gy)
{
	POINT p = getObjectPosition(gx, gy, t.getSize().x, t.getSize().y);
	t.draw(board, p.x, p.y);
}



///////////////////////////////////////////////////////////
///			implementation for 'GridMap'
///////////////////////////////////////////////////////////


int GridMap::point2Index(int x,int y)
{return y*size.x+x;}

//+++ release the allocated memory.
//+++ (Don't use it if no dynamically allcated memory is involved.)
void GridMap::releaseMemory()
{
	if(size.x==0 || size.y==0)
    return;                  // don't release if no map exists.

	
	if(floor)
		delete floor;
	if(block)
		delete block;

    size.x=0;               // change to "no map" state.
    size.y=0;

}


GridMap::GridMap(): floor(0),block(0)
{ size.x=0;
  size.y=0;
}

//--- accessors
POINT GridMap::getSize() { return size; }

int GridMap::getFloor(int x,int y)
{ return floor[point2Index(x,y)]; }

int GridMap::getBlock(int x,int y)
{ return block[point2Index(x,y)]; }

//--- mutators
void GridMap::setMapSize(int x,int y)
{

	int area = x*y;           // number of cells
     int* f = new int[area];
     int* b = new int[area];
	 for(int i=0;i<area;i++)
	 {
		 f[i]=0;
		 b[i]=-1;
	 }

	 int ix,iy;
	 for(iy=0;iy<size.y && iy<y;iy++)
		 for(ix=0;ix<size.x &&ix<x;ix++)
		 {
			 f[iy*x+ix]=getFloor(ix,iy);
			 b[iy*x+ix]=getBlock(ix,iy);
		 }
	
	if(floor)
		delete[] floor;
	if(block)
		delete[] block;
	floor=f;
	block=b;
	size.x=x;
	size.y=y;
}

void GridMap::setFloorMap(int* f)
{ floor=f;}

void GridMap::setBlockMap(int* b)
{ block=b;}

void GridMap::setFloor(int x,int y,int floorType)
{
	if(withinBound(x,y))
		floor[point2Index(x,y)]=floorType;
}

void GridMap::setBlock(int x,int y,int blockType)
{
	if(withinBound(x,y))
		block[point2Index(x,y)]=blockType;
}

void GridMap::loadMap(char* fileName)
{
     if(floor)
         delete[] floor;
     if(block)
         delete[] block;

     ifstream is(fileName,ios::in | ios::binary);
     is.read((char*)&size, sizeof(size));
     int area = size.x*size.y;           // number of cells
     floor = new int[area];
     block = new int[area];
     is.read((char*)floor,sizeof(int)*area);
     is.read((char*)block,sizeof(int)*area);
     is.close();
}

void GridMap::saveMap(char* fileName)
{
     ofstream os(fileName,ios::out | ios::binary);
     os.write((char*)&size, sizeof(size));
     os.write((char*)floor, sizeof(int)*size.x*size.y);
     os.write((char*)block, sizeof(int)*size.x*size.y);
     os.close();

}


//+++ check whether a unit can move flowing a direction from a position.
//+++ input: unit position, the direction that is trying to moving to.
bool GridMap::checkAccess(POINT position,POINT direction)
{
	POINT newPos;				// destination grid position
	newPos.x = position.x + direction.x;
	newPos.y = position.y + direction.y;

	POINT size=getSize();

	// check boundary
	if(newPos.x<0 || newPos.y<0 || newPos.x>=size.x || newPos.y>=size.y)
			return false;

	// check obstacles
	if(getBlock(newPos.x,newPos.y)==-1)
	{
		if(direction.x!=0 && direction.y!=0)
			return getBlock(newPos.x,position.y)==-1 && getBlock(position.x,newPos.y)==-1;
		else 
			return true;
	}
	
	return false;

}


bool GridMap::withinBound(int x, int y)
{ return x>=0 && x <size.x && y>=0 && y<size.y;}

//------------------------------------------------------------------
MapUnit::MapUnit(): stepsAcross(REGULAR_STEPS), currentStep(0)
{
 position.x=0;
 position.y=0;
 direction.x=0;
 direction.y=0;
 newDirection.x=0;
 newDirection.y=0;
}


MapUnit::MapUnit(POINT pos, int steps) : position(pos), stepsAcross(steps), currentStep(0)
{
 direction.x=0;
 direction.y=0;
 newDirection.x=0;
 newDirection.y=0;
}

POINT MapUnit::nextCell()
{
  POINT t;
  t.x = position.x+direction.x;
  t.y = position.y+direction.y;
  
  return t;
}


float MapUnit::getFloatX()
{ return position.x + (float)direction.x*currentStep/stepsAcross; }

float MapUnit::getFloatY()
{ return position.y + (float)direction.y*currentStep/stepsAcross; }         

int MapUnit::getCurrentStep() {return currentStep;}

void MapUnit::setDirection(POINT d)
{
  if(direction.x==0 && direction.y==0)
      direction=d;
  else
      newDirection=d;
}

bool MapUnit::anyNewDirection()
{ return newDirection.x!=0 || newDirection.y!=0;}

void MapUnit::setStepsAcross(int steps)
{ stepsAcross=steps;}

void MapUnit::updateMovement()
{
  if(direction.x==0 && direction.y==0)	// if no move, don't move
     return;
  
  if(++currentStep==stepsAcross)	// move
  {
      position.x+=direction.x;
      position.y+=direction.y;
      currentStep=0;
      direction=newDirection;
      newDirection.x=0;
      newDirection.y=0;
  }

}


void MapUnit::setPosition(int x,int y)
{
  position.x=x;
  position.y=y;
}

POINT MapUnit::getDirection()
{ return direction;}

POINT MapUnit::getPosition()
{ return position;}

//+++ Diagnally travelling is allowed.
//+++ Returning value is the minimum number of cells to across.
int MapUnit::distanceFrom(int x, int y)
{
	int d1= position.x-x;
    int d2= position.y-y;
	d1 = d1>0? d1 : -d1;
	d2 = d2>0? d2 : -d2;
	
	return d1>d2? d1 : d2;
}

int MapUnit::distanceFrom(MapUnit& other)
{ return distanceFrom((int)other.getFloatX(),(int)other.getFloatY());}
