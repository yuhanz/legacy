#include <windows.h>
#include <stdio.h>
#include "commonwin.h"
#include "commonDX.h"


#include "common.h"
#include "tile.h"
#include "TSprite.h"
#include "MusicDevice.h"

#include "Map.h"
#include "Game45.h"
#include "Manager.h"
#include "Dialog.h"
#include "Control.h"

#include "MyLogo.h"

//--- globles ---
const int OVER_BAR_X=(640-328)/2;		// screen position to display gameover bar
const int OVER_BAR_Y=100;
const int MAX_UNIT=16;

CApplication	capp;
CWindow			cwin;
DDrawPackage	ddraw;	
DInputPackage	dinput;
DMusicPackage	dmusic;	


MusicDevice musicDevice(&dmusic);

GameMap45 theMap;

EffectManager eManager(100);
UnitManager uManager(40);
RPG_DialogBox dBox;
ScriptProcessor scriptProcessor(32);
ScreenController screenControl;
PlayerControl playerControl(&dinput);
CombatStatusBar theStatusBar;

Tile gameoverBar;

DelayTimer delayTimer(45);

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
		    break;

		case WM_SETCURSOR:	 //ÉèÖÃ¹â±ê
			SetCursor(NULL);
			return true;
		break;

		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}



void Render()
{
//screenControl.pointCameraAtCell(0,0);
//screenControl.startShaking();
//screenControl.stopShaking();
//screenControl.rollToCell(0,0);

	static gameoverFlag=false;
	static DelayTimer gameoverDelay(3000);

	while(!delayTimer.hasEnoughDelay())
		;

	if(gameoverFlag==false && playerControl.getPlayer()==0)
	{
		gameoverFlag=true;
		gameoverDelay.start();
	}

	eManager.updateList();			// Update the effects to next state, and remove dead effects.
	uManager.updateTargets();		// Update the targets of the units
	uManager.processMovement();		// Process movement of each unit
	playerControl.process();


	if(playerControl.getPlayer() && !playerControl.getPlayer()->isAlive())
	{
		playerControl.setPlayer(0);
		scriptProcessor.clearList();
		//screenControl.centerCombatUnit();

	}

	uManager.updateLists();			// remove dead units.

		screenControl.placeCamera();
	//screenControl.centerCombatUnit();

	HDC boardDC;
	ddraw.getTheBoard()->GetDC(&boardDC);
	RECT rect = {0,0,640,480};
	FillRect(boardDC, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
	ddraw.getTheBoard()->ReleaseDC(boardDC);

	theMap.renewScreenObjectList(eManager.getListPointer()->getSize()+uManager.getTotal());
	eManager.insertScreenObject();
	uManager.insertScreenObject();
	theMap.drawMap(ddraw.getTheBoard());

	if(gameoverFlag)
	{
		gameoverBar.draw(ddraw.getTheBoard(),OVER_BAR_X,OVER_BAR_Y);

		if(gameoverDelay.hasEnoughDelay())
		{
			 gameoverDelay.stop();
			// Listen to user's input. ESC to quit. Enter to restart the game.
			if(dinput.updateKey())
			{
				if( dinput.checkKey(DIK_RETURN) ||
					dinput.checkKey(DIK_SPACE)  ||
					dinput.checkKey(DIK_NUMPADENTER) )
				{
					scriptProcessor.restartTheGame();
					gameoverFlag=false;
				}
			}
		}

	}
	else
		theStatusBar.drawOn(&ddraw,playerControl.getPlayer());

	scriptProcessor.updateList();

	ddraw.flip();  

//screenControl.cameraOff();
}




int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPreInstance,
				   LPSTR cmdLine, int cmdShow)
{

	char* appName = "RPG";
	const int width  = 200;
	const int height = 200;
	
	capp.Set(hThisInstance, cmdLine, cmdShow);
	if(!cwin.Prepare(appName, width, height, hThisInstance))
		return 0;


	CoInitialize(NULL);
	ddraw.init(cwin.hwnd, 640, 480, 16);
	dinput.init(hThisInstance, cwin.hwnd);
	dmusic.init(cwin.hwnd);
	dmusic.setSearchDirectory(NULL);


	cwin.Show(cmdShow);

	//--- My Logo----
/*
	AIsLogo logo;
	logo.displayOn(&ddraw,&dinput,1,240);
*/
	//----surfaces----
	IDirectDrawSurface* pdsFloors;
	IDirectDrawSurface* pdsObjects;
	IDirectDrawSurface* pdsWalls;
	IDirectDrawSurface* pdsChar[MAX_UNIT];
	IDirectDrawSurface* pdsExplode;
	IDirectDrawSurface* pdsEffect;
	IDirectDrawSurface* pdsTree[3];
	IDirectDrawSurface* pdsStatus;
	
	pdsFloors = ddraw.loadBitmap("floors.bmp");
	ddraw.setColorkey(pdsFloors,0,0);

	pdsObjects = ddraw.loadBitmap("objects.bmp");
	ddraw.setColorkey(pdsObjects,0,0);

	pdsWalls = ddraw.loadBitmap("walls.bmp");
	ddraw.setColorkey(pdsWalls,0,0);
	
	pdsTree[0]=ddraw.loadBitmap("tree0.bmp");
	pdsTree[1]=ddraw.loadBitmap("tree1.bmp");
	pdsTree[2]=ddraw.loadBitmap("tree2.bmp");
	ddraw.setColorkey(pdsTree[0],0,0);
	ddraw.setColorkey(pdsTree[1],0,0);
	ddraw.setColorkey(pdsTree[2],0,0);


	int i;
	char fileName[]="Npc00.bmp";
	for(i=0;i<MAX_UNIT;i++)
	{
		pdsChar[i] = ddraw.loadBitmap(fileName);
		ddraw.setColorkey(pdsChar[i],0,0);
		
		
		if(++fileName[4]>'9')
		{
			++fileName[3];
			fileName[4]='0';
		}

	}

	



	pdsExplode = ddraw.loadBitmap("explosion.bmp");
	ddraw.setColorkey(pdsExplode,0,0);

	pdsEffect = ddraw.loadBitmap("effect.bmp");
	ddraw.setColorkey(pdsEffect,0,0);

	pdsStatus = ddraw.loadBitmap("bars.bmp");
	ddraw.setColorkey(pdsStatus,0,0);


	//--- CombatStatusBar ---
	theStatusBar = CombatStatusBar(pdsStatus);

	//--- Game over bar
	RECT rc={0,80,328,160};
	gameoverBar = Tile(pdsStatus,rc);

	//----Tiles----
	const int n=5;

	//Tile t;
	TileSet floorTS(pdsFloors,64,32);
	Tile floorTile[n];
	for(i=0;i<n;i++)
		floorTile[i]=floorTS.getTile(i,0);
	
	TileSet objectTS(pdsObjects,32,32);
	Tile objectTile[10];
	for(i=0;i<4;i++)
		objectTile[i]=objectTS.getTile(i,0);
	
	TileSet wallTS(pdsWalls,64,128);
	Tile wallTile[4];
	for(i=0;i<4;i++)
		wallTile[i]=wallTS.getTile(i,0);

	Tile treeTile[3];
	RECT rect[] = { {0,0,172,177},
					{0,0,100,133},
					{0,0,96,173}
					};
	treeTile[0]= Tile(pdsTree[0],rect[0]);
	treeTile[1]= Tile(pdsTree[1],rect[1]);
	treeTile[2]= Tile(pdsTree[2],rect[2]);

	objectTile[3] = wallTile[0];
	objectTile[4] = wallTile[1];
	objectTile[5] = wallTile[2];
	objectTile[6] = wallTile[3];
	objectTile[7] = treeTile[0];
	objectTile[8] = treeTile[1];
	objectTile[9] = treeTile[2];

	Tile effectTile[10];
	TileSet effectTS(pdsExplode,54,50);
	for(i=0;i<10;i++)
		effectTile[i] = effectTS.getTile(i,0);

	//Tile bloodTile[5];
	TileSet bloodTS(pdsEffect,32,32);
	Tile bloodTile[5];		// for test only
	for(i=0;i<5;i++)
		bloodTile[i]=bloodTS.getTile(i,3);

	//Tile bladeTile[5];
	Tile bladeTile[5];
	for(i=0;i<5;i++)
		bladeTile[i]=bloodTS.getTile(i,0);

	
	//---GameMap45: Map45, GridMap---
	theMap.setTiles(n,floorTile,10,objectTile);
	theMap.setFloorSize(64,32);
	
	//theMap.loadMap("test.map");
	theMap.setInitialPosition(320, 100);


////////////////////////////////////
/// Set the map to the devices ///
GameUnit::theMap = &theMap;
Effect::theMap = &theMap;
uManager.setMap(&theMap);
eManager.setMap(&theMap);
WalkAI::theMap = &theMap;
screenControl = ScreenController(&theMap);

///////////////////////////////////////////////////////
/////		Test Effect		/////
//ProcessableEffect* pPE =  new DiminishingEffect(3.000001f,3.000001f,0,new Series_TSprite(bloodTile,5));
//eManager.setMap(&theMap);
//eManager.addEffect(pPE);
///////////////////////////////////////////////////////
/////		Set up resources for combatUnit		//////

CombatUnit::effectTile[0]=bloodTile;
CombatUnit::numberOfFrames[0]=5;
CombatUnit::effectTile[1]=bladeTile;
CombatUnit::numberOfFrames[1]=5;

CombatUnit::eManager = &eManager;

////////////////////////////////////////
///		Set up ScriptProcessoer	////
scriptProcessor.setDDraw(&ddraw);
scriptProcessor.setDInput(&dinput);
scriptProcessor.setMap(&theMap);
scriptProcessor.setUnitManager(&uManager);
scriptProcessor.setEffectManager(&eManager);
scriptProcessor.setCharacterSurface(pdsChar);
scriptProcessor.setScreenControl(&screenControl);
scriptProcessor.setMusicDevice(&musicDevice);
scriptProcessor.setPlayerControl(&playerControl);



/*
playerControl.setPlayer(scriptProcessor.createLeader("player", "unit.cst", 0, ScriptProcessor::PLAYER_LIST, 3,3,5,2,5));
RectTrap::thePlayer=playerControl.getPlayer();
screenControl.setCamera(playerControl.getPlayer());
scriptProcessor.runScript("Start.txt");
*/

scriptProcessor.restartTheGame();

delayTimer.start();
//screenControl.rollToCell(0,0);

	MSG temp = cwin.Run();


	pdsFloors->Release();
	pdsObjects->Release();
	pdsWalls->Release();
	for(i=0;i<MAX_UNIT;i++)
		pdsChar[i]->Release();
	for(i=0;i<3;i++)
		pdsTree[i]->Release();
	pdsExplode->Release();
	pdsEffect->Release();
	pdsStatus->Release();

	eManager.clearList();
	
	uManager.clearPlayerList();
	uManager.clearEnemyList();
	theMap.releaseMemory();
	

	dmusic.release();		// release music loader and performance
	dinput.release();		// release input devices
	CoUninitialize();

	return temp.wParam;
	

}
