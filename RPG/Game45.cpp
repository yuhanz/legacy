#include "Game45.h"

//////////////////////////////////////////////
////		Defination of 'GameMap45'
//////////////////////////////////////////////
GameMap45::~GameMap45()
{
	delete[] floorTile;
	delete[] blockTile;

	objectList.rewind();
	while(!objectList.pointerEnd())
		delete ((ScreenObject*)objectList.getData());

}

void GameMap45::setTiles(int fTypes, Tile* f, 
						 int bTypes, Tile* b)
{	
	floorTypes = fTypes;
	blockTypes = bTypes;

	floorTile = new Tile[floorTypes];
	blockTile = new Tile[blockTypes];

	for(int i=0;i<floorTypes;i++)
		floorTile[i]=f[i];

	for(int j=0;j<blockTypes;j++)
		blockTile[j]=b[j];
}



//+++ draw the map to a direct draw surface.
void GameMap45::drawMap(IDirectDrawSurface* theBoard)
{

	POINT mapSize = getSize();
	
	// lay the floor images first
	for(int j=0;j<mapSize.y;j++)
		for(int i=0;i<mapSize.x;i++)
			drawFloor(floorTile[getFloor(i,j)], theBoard,i,j);

	objectList.sort();
	objectList.rewind();

	if(mapSize.x >= mapSize.y)	// the case when width or equal to height
	{
		for(int i=0;i<mapSize.x;i++)
		{
			while(!objectList.pointerEnd() && objectList.peekWeight()<i)
				((ScreenObject*)objectList.getData())->drawOn(theBoard);
			
			POINT gp={i,0};
			do
			{
				drawCell(theBoard,gp);
				gp = leftCellOf(gp);
			}while(gp.x>=0 && gp.y< mapSize.y);

			
		}


		for(i=0;i<mapSize.y;i++)
		{
			while(!objectList.pointerEnd() && objectList.peekWeight()<i+mapSize.x-1)
				((ScreenObject*)objectList.getData())->drawOn(theBoard);

			POINT gp={mapSize.x-1,i};
			do
			{
				drawCell(theBoard,gp);
				gp = leftCellOf(gp);
			}while(gp.y< mapSize.y);
		}

	}
	
	else							// the case when height is greater
	{

		for(int i=0;i<mapSize.y;i++)
		{

			while(!objectList.pointerEnd() && objectList.peekWeight()<i)
				((ScreenObject*)objectList.getData())->drawOn(theBoard);
			
			POINT gp={0,i};
			do
			{
				drawCell(theBoard,gp);
				gp = rightCellOf(gp);
			}while(gp.y>=0 && gp.x< mapSize.x);
		}

		for(i=0;i<mapSize.x;i++)
		{
			while(!objectList.pointerEnd() && objectList.peekWeight()<i+mapSize.y-1)
				((ScreenObject*)objectList.getData())->drawOn(theBoard);
			
			POINT gp={i, mapSize.y-1};
			do
			{
				drawCell(theBoard,gp);
				gp = rightCellOf(gp);
			}while(gp.x< mapSize.x);

		}
	}


	while(!objectList.pointerEnd())
		((ScreenObject*)objectList.getData())->drawOn(theBoard);



}


//+++ the grid index of the cell on the right of the given cell index.
POINT GameMap45::rightCellOf(POINT p)
{
	POINT rp = {p.x+1, p.y-1};
	return rp;
}


//+++ the grid index of the cell on the left of the given cell index.
POINT GameMap45::leftCellOf(POINT p)
{
	POINT lp = {p.x-1, p.y+1};
	return lp;
}


//+++ Input:	The grid index of the cell, which is ready to be draw.
//+++ Output:	The cell will be drawn at this order:
//+++				vFence, hFence, block.
//+++				(if the cell is at the max index for any side on
//+++			the grid map, draw vFence or/and vFence	for the next cell.)
void GameMap45::drawCell(IDirectDrawSurface* theBoard, POINT p)
{
	int val;
	
	if((val=getBlock(p.x,p.y))>=0)
		drawObject(blockTile[val], theBoard, p.x, p.y);
}



void GameMap45::addScreenObject(ScreenObject* pSo, float weight)
{objectList.add(ArrayNode(weight,pSo));}

void GameMap45::renewScreenObjectList(int size)
{
	objectList.rewind();
	while(!objectList.pointerEnd())
		delete ((ScreenObject*)objectList.getData());
	
	objectList.create(size);
}


//////////////////////////////////////////////////////////////////
GameMap45* GameUnit::theMap=0;

GameUnit::~GameUnit()
{ theMap->setBlock(position.x+direction.x, position.y+direction.y, EMPTY);}

POINT GameUnit::getScreenPosition()
{
	POINT newPos;				// destination grid position
	newPos.x = position.x + direction.x;
	newPos.y = position.y + direction.y;

	POINT size = appearance.getSize();

	POINT oldScreenPos = theMap->getAdjustedPosition(position.x, position.y, size.x, size.y,theMap->getFloorSize().y/3);
	POINT newScreenPos = theMap->getAdjustedPosition(newPos.x,  newPos.y,  size.x, size.y,theMap->getFloorSize().y/3);
	
	// get intermediate position.
	POINT val = {(newScreenPos.x-oldScreenPos.x)*currentStep/stepsAcross+oldScreenPos.x,
				 (newScreenPos.y-oldScreenPos.y)*currentStep/stepsAcross+oldScreenPos.y};

	return val;
}


void GameUnit::updateMovement()
{
	if(currentStep==0)
	{
		POINT newPos;				// destination grid position
		newPos.x = position.x + direction.x;
		newPos.y = position.y + direction.y;


		if(theMap->checkAccess(position,direction)==false)
		{
			direction=newDirection;
			newDirection.x=0;
			newDirection.y=0;

			return;
		}
		else
		{
			theMap->setBlock(position.x, position.y,EMPTY);
			theMap->setBlock(position.x+direction.x, position.y+direction.y, BLOCKED);
		}
		
			
	}


	MapUnit::updateMovement();

}
	

void GameUnit::draw(IDirectDrawSurface* board)
{
	POINT dp = getScreenPosition();
	appearance.draw(board, dp.x, dp.y);
}


void GameUnit::update()
{
	
	POINT d=getDirection();
	updateMovement();
	if(!(d.x==0 &&d.y==0))
	{
		appearance.changeDirection(d);
		appearance.update();
	}
}


ScreenObject GameUnit::getScreenObject()
{
	POINT up = getScreenPosition();
	ScreenObject so(&appearance, up.x,up.y);
	return so;
}

float GameUnit::getDistance()
{return getFloatX()+getFloatY();}

Effect* GameUnit::createEffect(Tile* pTile,int frameNum)
{ return new Effect(getFloatX(), getFloatY(), theMap->getFloorSize().y/3, new Series_TSprite(pTile,frameNum)); }

void GameUnit::setPosition(int x,int y)
{
	theMap->setBlock(position.x+direction.x, position.y+direction.y, EMPTY);
	MapUnit::setPosition(x,y);
	theMap->setBlock(position.x, position.y, BLOCKED);
	currentStep=0;
}

//////////////////////////////////////////////////////////////////
ScreenObject::ScreenObject() : obj(0)
{position.x=0;position.y=0;}

ScreenObject::ScreenObject(Tile* s,int x,int y)
{
	obj=s;
	position.x=x;
	position.y=y;
}


void ScreenObject::drawOn(IDirectDrawSurface* target)
{
	if(obj) obj->draw(target,position.x,position.y);
}


//////////////////////////////////////////////////////////////////
GameMap45* Effect::theMap=0;

Effect::Effect(float gx, float gy, int z, Series_TSprite* ts)
: sprite(ts), x(gx), y(gy), zShift(z), sx(0), sy(0)
{
	sprite->move();
}

Effect::~Effect()
{delete sprite;}

void Effect::setSpeed(float _sx,float _sy)
{
	sx=_sx;
	sy=_sy;
}

ScreenObject Effect::getScreenObject()
{
	
	POINT size = sprite->getSize();

	POINT screenPos = theMap->getAdjustedFloatPosition(x,y, size.x, size.y, zShift);

	return ScreenObject(sprite,screenPos.x, screenPos.y);		
}

void Effect::updateMovement()
{ x+=sx; y+=sy;}

float Effect::getDistance()
{return x+y;}

void Effect::putInBackground()
{
	x-=0.01f;
	y-=0.01f;
}

void Effect::putInForeground()
{
	x+=0.01f;
	y+=0.01f;
}



//////////////////////////////////////////////////////////////////
ProcessableEffect::ProcessableEffect(float gx,float gy,int z,Series_TSprite* ts)
:Effect(gx,gy,z,ts)
{}

ProcessableEffect::~ProcessableEffect()
{}
//////////////////////////////////////////////////////////////////

DiminishingEffect::DiminishingEffect(float gx,float gy,int z,Series_TSprite* ts)
: ProcessableEffect(gx,gy,z,ts)
{}

DiminishingEffect::~DiminishingEffect()
{}

bool DiminishingEffect::process()
{
	updateMovement();
	sprite->update();
	return sprite->isStopped();
}



//////////////////////////////////////////////////////////////////class DistanceSensitiveEffect : public ProcessableEffect
		
DistanceSensitiveEffect::DistanceSensitiveEffect(float gx,float gy,int z,Series_TSprite* ts,int max)
:ProcessableEffect(gx,gy,z,ts), maxDistance(max*max)
{
	oldX=x;
	oldY=y;
}

DistanceSensitiveEffect::~DistanceSensitiveEffect()
{}

bool DistanceSensitiveEffect::process()
{
	updateMovement();
	sprite->update();
	float tx=x-oldX;
	float ty=y-oldY;
	tx*=tx;
	ty*=ty;
	return tx+ty >= maxDistance;
}
