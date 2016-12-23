//////////////////////////////////////////////////////////////
//	Programmer:		Yuhan Zhang
//	EMail:			Yuhan_Zhang@netease.com
//	Date:			July 3, 2003
//	project:			Bombing game.
//	Description:		Missiles are dropped from the sky.
//					People are wandering around on the ground.
//					The player will play one of the people
//					to walking around and dodge all the missiles
//					The player is only allowed to walking left
//					or right and no other actions.
//						3 Credits will be given to each play.
//					And there are 3 levels in the game. hard ones
//					proceeding easy ones.
//						The player will win if the human-controlled
//					character survives. (A special music will be
//					played as a greeting.)  "Game over" will be
//					claimed	if the player is killed three times.
//					 (However, after the player wins, "Gave over"
//					will be still claimed anyway.)
//
//////////////////////////////////////////////////////////////


#define DIRECT3D_VERSION         0x0800


#include <windows.h>
#include <time.h>
#include <typeinfo>


#include "common.h"
#include "Utility.h"
#include "commonwin.h"
#include "commonDX.h"
#include "sprite.h"
#include "Units.h"
#include "managers.h"
#include "controls.h"


//--- globles ---
CApplication	capp;
CWindow			cwin;
DDrawPackage	ddraw;
DInputPackage	dinput;
DMusicPackage	dmusic;

IDirectDrawSurface* theScene;

BombManager theAttacker(30);
VictimManager theVictim(10);
Player thePlayer;
StageControl theStage;

DelayTimer screenTimer;		// use to control the speed of drawing



//--- functions ---
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
		    break;
		
		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}




void Render()
{
	
	
	if(!screenTimer.hasEnoughDelay())
		return;

	
	
	RECT rcRect;
	HRESULT ddrval;
	
	
	

	SetRect(&rcRect,0,0,640,480);
	ddrval = ddraw.getTheBoard()->BltFast(0,0, theScene, &rcRect, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
	
	if(theStage.stage != StageControl::DEMO)
		thePlayer.process();
	theVictim.process();
	theAttacker.process();
	theStage.process();
	
	
	ddraw.flip();

}




int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPreInstance,
				   LPSTR cmdLine, int cmdShow)
{
	char* appName = "Rain";
	const int width  = 200;
	const int height = 200;
	
	IDirectDrawSurface* character[5];
	IDirectDrawSurface* death;
	IDirectDrawSurface* explosion;
	IDirectDrawSurface* bomb;
	IDirectDrawSurface* text;

	const int GROUND_LEVEL = 470;
	
	capp.Set(hThisInstance, cmdLine, cmdShow);
	if(!cwin.Prepare(appName, width, height, hThisInstance))
		return 0;

	cwin.Show(cmdShow);


	CoInitialize(NULL);
	ddraw.init(cwin.hwnd, 640, 480, 16);
	dinput.init(hThisInstance, cwin.hwnd);
	
	
	
	dmusic.init(cwin.hwnd);
	dmusic.setSearchDirectory(NULL);
	
	
				// load images
	theScene  = ddraw.loadBitmap("theScene.bmp");
	character[0] = ddraw.loadBitmap("character0.bmp");
	character[1] = ddraw.loadBitmap("character1.bmp");
	character[2] = ddraw.loadBitmap("character2.bmp");
	character[3] = ddraw.loadBitmap("character3.bmp");
	character[4] = ddraw.loadBitmap("character4.bmp");
	death     = ddraw.loadBitmap("death.bmp");
	explosion = ddraw.loadBitmap("explosion.bmp");
	bomb	  = ddraw.loadBitmap("bomb.bmp");
	text	  = ddraw.loadBitmap("text.bmp");
	
	for(int i=0;i<5;i++)
		ddraw.setColorkey(character[i], 0,0);
	ddraw.setColorkey(death, 0,0);
	ddraw.setColorkey(explosion, 0,0);
	ddraw.setColorkey(bomb, 0,0);
	ddraw.setColorkey(text, 0,0);

	

			// assemble the data with units
	WalkingPerson::death = new CS_Series(death, 32, 48, 0,0, true,4);
	Bomb::explosion = new CS_Series(explosion, 160, 160, 0,0, true, 4);
	Bomb::setTarget(theVictim.getList());
	Bomb::setGroundLevel(GROUND_LEVEL);
	
	
			// prepare
	theAttacker.dds_bomb = bomb;
	for(i=1;i<5;i++)
		theVictim.dds_people[i-1] = character[i];

	thePlayer.init(&dinput, &theVictim, character[0]);

	theStage.init(&dmusic, &theVictim, &theAttacker, &thePlayer);
	Label* title;
	Label* gameover;
	RECT rect;
	SetRect(&rect, 0,0,400,100);
	title = new Label(text, rect);
	SetRect(&rect, 0,100,400,180);
	gameover = new Label(text, rect);
	theStage.attachImages(title,gameover,new CS_Cycle(text,400, 45, 0, 4, false, 2));
		

	screenTimer.timeDelay=1;
	screenTimer.start();



	MSG temp = cwin.Run();
			

	dmusic.stopAll();		// stop all music.


		// delete static data saved in the unit class
	delete WalkingPerson::death;
	delete Bomb::explosion;
	
			// release directdraw surfaces.
	theScene->Release();
	death->Release();
	explosion->Release();
	bomb->Release();
	text->Release();
	for(i=0;i<5;i++)
		character[i]->Release();

	
	
	
	dinput.release();		// release input devices
	theStage.release();		// release the music segment.
	dmusic.release();		// release music loader and performance


	CoUninitialize();
	
	return temp.wParam;
}