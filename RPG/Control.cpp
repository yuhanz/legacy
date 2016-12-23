#include "Control.h"


////////////////////////////////////////////////////////////////
Trap::Trap(char* fileName)
{strcpy(scriptName,fileName);}

Trap::~Trap(){}

bool Trap::process()
{return true;}

char* Trap::getScriptName()
{return scriptName;}


////////////////////////////////////////////////////////////////
CombatUnit* RectTrap::thePlayer=0;

RectTrap::RectTrap(char* s,int l,int t,int r,int b) : Trap(s)
{
 rect.left=l;
 rect.top=t;
 rect.right=r;
 rect.bottom=b;
}

bool RectTrap::process()
{return thePlayer==0? true : PointInRect((int)thePlayer->getFloatX(), (int)thePlayer->getFloatY(),rect);}



////////////////////////////////////////////////////////////////


ScriptProcessor::ScriptProcessor(int maxTrap):scriptLevel(0)
{
	trapList = new List(maxTrap);
}

ScriptProcessor::~ScriptProcessor()
{
 clearList();
 delete trapList;
}

void ScriptProcessor::clearList()
{
	while(trapList->getSize()>0)
		delete (Trap*) trapList->remove(0);
}

bool ScriptProcessor::addTrap(Trap* pTrap)
{ return trapList->add(pTrap);}

void ScriptProcessor::updateList()
{
    int i=0;
    Trap* pTrap;
	while(i<trapList->getSize())
	{
		pTrap = (Trap*) trapList->getData(i);
		if(pTrap->process())		// if the effect is expired after process
		{
			runScript(((Trap*)trapList->remove(i))->getScriptName());
			delete pTrap;
		}
		else
			++i;
	}
}


void ScriptProcessor::restartTheGame()
{
	musicDevice->stopMusic();

	clearList();		// clear trap list
	uManager->clearPlayerList();
	uManager->clearEnemyList();
	eManager->clearList();

	playerControl->setPlayer(createLeader("player", "unit.cst", 0, PLAYER_LIST, 3,3,5,2,5));
	RectTrap::thePlayer=playerControl->getPlayer();
	screenControl->setCamera(playerControl->getPlayer());
	runScript("Start.txt");

}

	/*--------------------
	----- Connections ----
	--------------------*/
void ScriptProcessor::setMap(GameMap45* m)
{theMap=m;}
		
void ScriptProcessor::setDInput(DInputPackage* d)
{dinput=d;}

void ScriptProcessor::setDDraw(DDrawPackage* d)
{
	ddraw=d;
	dBox.setDDrawPackage(d);
}

void ScriptProcessor::setUnitManager(UnitManager* um)
{uManager=um;}

void ScriptProcessor::setEffectManager(EffectManager* em)
{eManager=em;}

void ScriptProcessor::setCharacterSurface(IDirectDrawSurface** f)
{face=f;}

void ScriptProcessor::setScreenControl(ScreenController* s)
{screenControl=s;}

void ScriptProcessor::setMusicDevice(MusicDevice* m)
{musicDevice=m;}

void ScriptProcessor::setPlayerControl(PlayerControl* pc)
{playerControl=pc;}

	/*-----------------------
	---- inner functions ----
	-----------------------*/
void ScriptProcessor::repaintMap()
{
	HDC boardDC;
	ddraw->getTheBoard()->GetDC(&boardDC);
	RECT rect = {0,0,640,480};
	FillRect(boardDC, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
	ddraw->getTheBoard()->ReleaseDC(boardDC);

	theMap->renewScreenObjectList(eManager->getListPointer()->getSize()+uManager->getTotal());
	eManager->insertScreenObject();
	uManager->insertScreenObject();
	theMap->drawMap(ddraw->getTheBoard());

	ddraw->flip();  
}

void ScriptProcessor::waitForConfirm()
{
		//=== Give a little delay to make the keyboard less sensitive.
	Pause(200);

	while(true)
	{
		dinput->updateKey();
		if( dinput->checkKey(DIK_RETURN) ||
			dinput->checkKey(DIK_SPACE)  ||
			dinput->checkKey(DIK_ESCAPE) ||
			dinput->checkKey(DIK_NUMPADENTER) )
			return;
	}
}

bool ScriptProcessor::makeDecision()
{
	while(true)
	{
		dinput->updateKey();
		if(dinput->checkKey(DIK_Y))
			return true;
		else if(dinput->checkKey(DIK_N))
			return false;
	}
		
}

int ScriptProcessor::makeChoice()
{

	while(true)
	{
		dinput->updateKey();
		if(dinput->checkKey(DIK_0))
			return 0;

		int key=DIK_1;
		for(int i=0;i<9;i++)
		{
			if(dinput->checkKey(key))
				return key+1-DIK_1;;
			++key;
		}
	}
}



void ScriptProcessor::loadMap(char* s)
{theMap->loadMap(s);}


void ScriptProcessor::showDialog(char* s)
{
	repaintMap();
	dBox.display(s,0,0,0);
	waitForConfirm();
}

void ScriptProcessor::removeAI(char* name)
{
	CombatUnit* tempUnit = uManager->searchByName(name);

	if(tempUnit)
		tempUnit->releaseCombatAI();
}

void ScriptProcessor::setUnitPos(char* name, int x, int y)
{
	CombatUnit* tempUnit = uManager->searchByName(name);
	if(tempUnit)
		((GameUnit*)tempUnit)->setPosition(x,y);
}

void ScriptProcessor::unitTalk(char* name,char* talk)
{
	CombatUnit* tempUnit = uManager->searchByName(name);
	if(tempUnit)
	{
		POINT sp = tempUnit->getScreenPosition();
		repaintMap();
		dBox.display(talk,sp.x,sp.y,1);
		waitForConfirm();

	}
}

void ScriptProcessor::setFacingDir(char* name,int dx, int dy)
{
	CombatUnit* tempUnit = uManager->searchByName(name);
	if(tempUnit)
	{
		POINT d={dx,dy};
		tempUnit->appearance.changeDirection(d);
		tempUnit->appearance.nextFrame();
	}
}

void ScriptProcessor::setTrap(char* s,int l,int t,int r,int b)
{
	Trap* theTrap = new RectTrap (s, l,t,r,b);
	addTrap(theTrap);
}

//+++ Create a combat unit and insert it to the unit list.
void ScriptProcessor::createCombatUnit(char* name, char* statusFile, int faceID,int listID,int x,int y)
{
	CombatUnit* tempUnit = new CombatUnit();

	strcpy(tempUnit->name,name);
	tempUnit->setPosition(x,y);
	tempUnit->appearance = Walk_TSprite2(TileSet(face[faceID],32,32));
	tempUnit->appearance.move();
	tempUnit->loadStatus(statusFile);

	if(listID==PLAYER_LIST)
		uManager->addToPlayerList(tempUnit);
	else
		uManager->addToEnemyList(tempUnit);
}

//+++ Create a leader unit and insert it to the unit list. Return the pointer to this unit.
Leader* ScriptProcessor::createLeader(char* name, char* statusFile, int faceID,int listID,int x,int y,int capacity, int closest, int farthest)
{
	Leader* tempUnit = new Leader(capacity,closest,farthest);

	strcpy(tempUnit->name,name);
	tempUnit->setPosition(x,y);
	tempUnit->appearance = Walk_TSprite2(TileSet(face[faceID],32,32));
	tempUnit->appearance.move();
	tempUnit->loadStatus(statusFile);

	if(listID==PLAYER_LIST)
		uManager->addToPlayerList(tempUnit);
	else
		uManager->addToEnemyList(tempUnit);

	return tempUnit;
}

void ScriptProcessor::activateMonsterAI(char* name)
{
	CombatUnit* tempUnit = uManager->searchByName(name);

	if(tempUnit)
		tempUnit->createMonsterAI();
}


//+++ Find the leader and the unit in the same list. and connect them.
//+++ (WARNING: the first one must be a leader type combat unit.)
void ScriptProcessor::addFollower(char* leaderName, char* followerName)
{
	Leader* theLeader =(Leader*) uManager->searchByName(leaderName);

	if(theLeader)
	{
		CombatUnit* tempUnit = uManager->searchByName(followerName);

		if(tempUnit)
			theLeader->addFollower(tempUnit);
	}
}

//+++ Find the leader and the unit in the same list. and connect them.
//+++ (WARNING: the first one must be a leader type combat unit.)
void ScriptProcessor::addFollowerMonster(char* leaderName, char* followerName)
{
	Leader* theLeader =(Leader*) uManager->searchByName(leaderName);

	if(theLeader)
	{
		CombatUnit* tempUnit = uManager->searchByName(followerName);

		if(tempUnit)
			theLeader->addFollowerMonster(tempUnit);
	}
}


void ScriptProcessor::removeUnit(char* name)
{
	// Try to search both lists to remove it.
	CombatUnit* tempUnit = uManager->removeUnitByName(name,0);
	if(tempUnit)
		delete tempUnit;
	else if(tempUnit=uManager->removeUnitByName(name,1))
		delete tempUnit;
}

void ScriptProcessor::playMusic(char* fileName)
{
	WCHAR ws[50];
	CStringToWString(fileName,ws);
	if(!musicDevice->playMusic(ws))
	{
		char buff[40]="Failed in playing ";
		strcat(buff,fileName);
		paintScreen((HBRUSH)GetStockObject(BLACK_BRUSH));
		dBox.display(buff,0,0,0);
		waitForConfirm();
	}
}

void ScriptProcessor::paintScreen(HBRUSH hbrush)
{
	HDC boardDC;
	ddraw->getTheBoard()->GetDC(&boardDC);
	RECT rect = {0,0,640,480};
	FillRect(boardDC, &rect, hbrush);
	ddraw->getTheBoard()->ReleaseDC(boardDC);

	ddraw->flip();
}

void ScriptProcessor::playWithoutPlayer()
{
	eManager->updateList();			// Update the effects to next state, and remove dead effects.
	uManager->updateTargets();		// Update the targets of the units
	uManager->processMovement();		// Process movement of each unit
	//screenControl->centerCombatUnit();
	screenControl->placeCamera();
	uManager->updateLists();			// remove dead units.

}

void ScriptProcessor::runScript(char* fileName)
{
	++scriptLevel;		// remember the script level of next recursive calling

	ifstream is(fileName, ios::in);
  while(is)
 {
    const int MAX=50;
    char tag[MAX]="";
    char s1[MAX*8]="";
    char s2[MAX*8]="";
    int x,y,l,r,t,b;
	float fx,fy;
	CombatUnit* tempUnit;

    is >> tag;
    if(strcmp(tag,"<LoadMap>")==0)
    {
        is >> s1;
        loadMap(s1);
    }
    else if(strcmp(tag,"<ShowDialog>")==0)
    {
		char dummy;
		is.get(dummy);		// eat the unnecessary space.
        is.get(s1,MAX*8,(char)1);
        showDialog(s1);

    }
    else if(strcmp(tag,"<SetTrap>")==0)
    {
        is >> l >> t >> r >> b >> s1;
        setTrap(s1,l,t,r,b);
    }
    else if(strcmp(tag,"<RunScript>")==0)
    {
        is >> s1;
        runScript(s1);
    }
    else if(strcmp(tag,"<AddFollower>")==0)
	{
		//-------------------------------
		// s1 : leader name;
		// s2 : follower name
		//-------------------------------
		is >> s1 >> s2;
		addFollower(s1,s2);
	}
    else if(strcmp(tag,"<AddFollowerMonster>")==0)
	{
		//-------------------------------
		// s1 : leader name;
		// s2 : follower name
		//-------------------------------
		is >> s1 >> s2;
		addFollowerMonster(s1,s2);
	}

    else if(strcmp(tag,"<ActivateMonsterAI>")==0)
	{
		//-------------------------------
		// s1 : unit name;
		//-------------------------------
		is >> s1;
		activateMonsterAI(s1);
	}
    else if(strcmp(tag,"<ActivateCowardAI>")==0)
	{
		//-------------------------------
		// s1 : unit name;
		//-------------------------------
		is >> s1;
		tempUnit = uManager->searchByName(s1);
		if(tempUnit)
			tempUnit->createCowardAI();
	}
    else if(strcmp(tag,"<ActivatePatrolAI>")==0)
	{
		//-------------------------------
		// s1 : unit name;
		//-------------------------------
		is >> s1 >> x >> y >> l >> t;
		tempUnit = uManager->searchByName(s1);
		if(tempUnit)
			tempUnit->createPatrolAI(x,y,l,t);
	}
    else if(strcmp(tag,"<RemoveAI>")==0)
	{
		//-------------------------------
		// s1 : unit name;
		//-------------------------------
		is >> s1;
		removeAI(s1);
	}
    else if(strcmp(tag,"<CreateEnemy>")==0)
	{
		//-------------------------------
		// s1 : unit name
		// s2 : combat status file name.
		// r  : face id (0-14)
		// x,y: unit position.
		//-------------------------------
		is >> s1 >> s2 >> r >> x >> y;
		createCombatUnit(s1,s2,r,1,x,y);
	}
    else if(strcmp(tag,"<CreateFriend>")==0)
	{
		//-------------------------------
		// s1 : unit name
		// s2 : combat status file name.
		// r  : face id (0-14)
		// x,y: unit position.
		//-------------------------------
		is >> s1 >> s2 >> r >> x >> y;
		createCombatUnit(s1,s2,r,0,x,y);
	}
    else if(strcmp(tag,"<CreateEnemyLeader>")==0)
	{
		//-------------------------------
		// s1 : unit name
		// s2 : combat status file name.
		// r  : face id (0-14)
		// x,y: unit position.
		// l  : capacity
		// b  : closest distance to keep
		// f  : farthest distance to keep
		//-------------------------------
		is >> s1 >> s2 >> r >> x >> y >> l >> b >> t;
		createLeader(s1,s2,r,1,x,y,l,b,t);
	}
    else if(strcmp(tag,"<CreateFriendLeader>")==0)
	{
		//-------------------------------
		// s1 : unit name
		// s2 : combat status file name.
		// r  : face id (0-14)
		// x,y: unit position.
		// l  : capacity
		// b  : closest distance to keep
		// f  : farthest distance to keep
		//-------------------------------
		is >> s1 >> s2 >> r >> x >> y >> l >> b >> t;
		createLeader(s1,s2,r,0,x,y,l,b,t);
	}
	else if(strcmp(tag,"<SetUnitPosition>")==0)
	{
		//-------------------------------
		// s1 : unit name
		// x,y: unit position.
		//-------------------------------
		is >> s1 >> x >> y;
		setUnitPos(s1,x,y);
	}
	else if(strcmp(tag,"<UnitTalk>")==0)
	{
		//-------------------------------
		// s1 : unit name
		// s2 : content
		//-------------------------------
		is >> s1;

		char dummy;
		is.get(dummy);		// eat the unnecessary space.
		is.get(s2,MAX*8,(char)1);
		unitTalk(s1,s2);
	}
	else if(strcmp(tag,"<SetFacingDirection>")==0)
	{
		//-------------------------------
		// s1 : unit name
		// x,y:
		//-------------------------------
		is >> s1 >> x >> y;
		setFacingDir(s1,x,y);
	}
	else if(strcmp(tag,"<SetSearchingRange>")==0)
	{
		//-------------------------------
		// s1 : unit name
		// r  : searching range
		//-------------------------------
		is >> s1 >> r;
		tempUnit = uManager->searchByName(s1);
		if(tempUnit)
			tempUnit->setSearchingRange(r);
	}
	else if(strcmp(tag,"<RemoveUnit>")==0)
	{
		//-------------------------------
		// s1 : unit name
		//-------------------------------
		is >> s1;
		removeUnit(s1);		
	}
	else if(strcmp(tag,"<ClearEnemies>")==0)
	{
		uManager->clearEnemyList();
	}
	else if(strcmp(tag,"<ClearFriends>")==0)
	{
		uManager->removeUnitByName("player",0);
		uManager->clearPlayerList();
		uManager->addToPlayerList(playerControl->getPlayer());
	}
	else if(strcmp(tag,"<ClearTraps>")==0)
	{
		clearList();
	}
	else if(strcmp(tag,"<ClearEffects>")==0)
	{
		eManager->clearList();
	}
	else if(strcmp(tag,"<PlayMusic>")==0)
	{
		//-------------------------------
		// s1 : file name
		//-------------------------------
		is >> s1;
		playMusic(s1);		
	}
	else if(strcmp(tag,"<StopMusic>")==0)
	{
		musicDevice->stopMusic();
	}
	else if(strcmp(tag,"<Heal>")==0)
	{
		//-------------------------------
		// s1 : unit name
		//-------------------------------
		is >> s1;
		tempUnit = uManager->searchByName(s1);
		if(tempUnit)
		{
			tempUnit->hp=tempUnit->mhp;
			tempUnit->mp=tempUnit->mmp;
		}
	}
	else if(strcmp(tag,"<BlackScreen>")==0)
	{
		paintScreen((HBRUSH)GetStockObject(BLACK_BRUSH));
	}
	else if(strcmp(tag,"<WhiteScreen>")==0)
	{
		paintScreen((HBRUSH)GetStockObject(WHITE_BRUSH));
	}
	else if(strcmp(tag,"<Pause>")==0)
	{
		//-------------------------------
		// t : time (in miliseconds)
		//-------------------------------
		is >> t;
		Pause(t);
	}
	else if(strcmp(tag,"<SetUnitSpeed>")==0)
	{
		//-------------------------------
		// s1 : unit name
		// fx : relative speed
		//		(2 means 2 times faster.
		//		 0.5 means 2 times slower.)
		//-------------------------------
		is >> s1 >> fx;
		tempUnit = uManager->searchByName(s1);
		if(tempUnit)
			tempUnit->setStepsAcross((int)(MapUnit::REGULAR_STEPS/fx));
	}
	else if(strcmp(tag,"<DemoMode>")==0)
	{
		//-------------------------------
		// t  : turns that free mode last.
		//	(In Demo mode, the system will go free
		//	without player's interuption, until
		//  a certain turns is passed)
		//-------------------------------
		//=== Check whether a map is loaded ===		
		if(theMap->getSize().x<1 || theMap->getSize().y<1)
		{
			paintScreen((HBRUSH)GetStockObject(BLACK_BRUSH));
			dBox.display("Error: No map is loaded!",0,0,0);
			waitForConfirm();
		}

		is >> t;
		DelayCounter theCounter(t);
		theCounter.start();
		delayTimer.timeDelay=45;
		delayTimer.start();
		while(!theCounter.hasEnoughDelay())
		{
			while(!delayTimer.hasEnoughDelay())
				;
			playWithoutPlayer();
			repaintMap();
		}
	}
	else if(strcmp(tag,"<SetScreen>")==0)
	{
		//-------------------------------
		// x,y: Grid position on the map
		// (The camera will be set to center that 
		//  grid position.)
		//-------------------------------
		is >> x >> y;
		screenControl->pointCameraAtCell(x,y);
		repaintMap();
	}
	else if(strcmp(tag,"<SystemScreen>")==0)
	{
		//-------------------------------
		// s1 : text content
		//-------------------------------
		char dummy;
		is.get(dummy);		// eat the unnecessary space.
		is.get(s1,MAX*8,(char)1);
		paintScreen((HBRUSH)GetStockObject(BLACK_BRUSH));
		dBox.display(s1,0,0,0);
		waitForConfirm();
	}
	else if(strcmp(tag,"<LoadStatus>")==0)
	{
		//-------------------------------
		// s1 : unit name
		// s2 : status file
		//-------------------------------
		is >> s1 >> s2;
		tempUnit = uManager->searchByName(s1);
		if(tempUnit)
			tempUnit->loadStatus(s2);
	}
	else if(strcmp(tag,"<CameraOnUnit>")==0)
	{
		//-------------------------------
		// s1 : unit name
		//-------------------------------
		is >> s1;
		tempUnit = uManager->searchByName(s1);
		if(tempUnit)
			screenControl->setCamera(tempUnit);
	}
	else if(strcmp(tag,"<CameraOff>")==0)
	{
		//-------------------------------
		// s1 : unit name
		//-------------------------------
		screenControl->cameraOff();
	}
	else if(strcmp(tag,"<GetExperience>")==0)
	{
		//-------------------------------
		// s1 : unit name
		// x  : amount of experience
		//-------------------------------
		is >> s1 >> x;
		tempUnit = uManager->searchByName(s1);
		if(tempUnit)
		{
			tempUnit->exp+=x;
			tempUnit->levelUp();
		}
	}
	else if(strcmp(tag,"<SetMapCell>")==0)
	{
		//-------------------------------
		// x,y  : grid position
		// t    : cell type. (-1 is empty)
		//-------------------------------
		is >> x >> y >> t;
		int index=theMap->point2Index(x,y);
        if(index>=0 && index < theMap->getSize().x*theMap->getSize().y)
			theMap->setBlock(x,y,t);
		else
			MessageBox(NULL,"Set cell out of map","Script",MB_OK);
	}
	else if(strcmp(tag,"<ConvertEnemy>")==0)
	{
		//-------------------------------
		// s1	: unit name
		//-------------------------------
		is >> s1;
		uManager->enemyToFriend(s1);
	}
	else if(strcmp(tag,"<GameOver>")==0)
	{
		playerControl->setPlayer(0);
	}
	else if(strcmp(tag,"<ShakeScreenStart>")==0)
	{
		screenControl->startShaking();
	}
	else if(strcmp(tag,"<ShakeScreenStop>")==0)
	{
		screenControl->stopShaking();
	}
	else if(strcmp(tag,"<RollScreen>")==0)
	{
		is >> x >> y;
		//MessageBox(NULL,"","",MB_OK);
		screenControl->rollToCell(x,y);
	}
	else if(strcmp(tag,"<SetShakingForce>")==0)
	{
		is >> x;
		screenControl->setGratitude(x);
	}
	else if(strcmp(tag,"<SetRollingSpeed>")==0)
	{
		is >> x >> y;
		screenControl->setRollStep(x);
	}
	else if(strcmp(tag,"<SetLeaderRange>")==0)
	{
			// Warning: make sure the unit is a "Leader" type
		is >> s1 >> x >> y;
		tempUnit = uManager->searchByName(s1);
		if(tempUnit)
		{
			((Leader*)tempUnit)->setClosest(x);
			((Leader*)tempUnit)->setFarthest(y);
		}
	}
	else if(strcmp(tag,"<SetLeaderCapacity>")==0)
	{
			// Warning: make sure the unit is a "Leader" type
		is >> s1 >> x;
		tempUnit = uManager->searchByName(s1);
		if(tempUnit)
			((Leader*)tempUnit)->setCapacity(x);

	}
	else
    {
        // unknown tag. Do nothing.
       // cout << "Event:" << "Unknown" << endl;
    }
 }
   is.close();

   --scriptLevel;		// After this line, scriptLevel==0 means the first
						// time of calling this funciton.
						// For level>0, the funtion is in some recursive call.
			// Check player position and map if the "runScript" function is
			// about to end.
   if(!scriptLevel)
   {				
		//=== Check player position ===
		if(!theMap->withinBound(playerControl->getPlayer()->getPosition().x,playerControl->getPlayer()->getPosition().y))
		{
			paintScreen((HBRUSH)GetStockObject(BLACK_BRUSH));
			dBox.display("Error: Player is out of map in play mode!",0,0,0);
			waitForConfirm();
		}

		//=== Check whether a map is loaded ===		
		if(theMap->getSize().x<1 || theMap->getSize().y<1)
		{
			paintScreen((HBRUSH)GetStockObject(BLACK_BRUSH));
			dBox.display("Error: No map is loaded!",0,0,0);
			waitForConfirm();
		}
   }


}

//////////////////////////////////////////////////////////////////////

ScreenController::ScreenController():screenWidth(640),screenHeight(480)
{}

ScreenController::ScreenController(GameMap45* m):screenWidth(640),screenHeight(480), theMap(m), shaking(5), rollStep(3), mode(0),cameraTarget(0),iniRecord(theMap->getInitialPosition())
{
	centerDrawingPos.x=320;
	centerDrawingPos.y=240;
}

void ScreenController::setScreenSize(int w,int h)
{screenWidth=w, screenHeight=h;}

void ScreenController::setCamera(CombatUnit* target)
{
	cameraTarget=target;
	POINT frameSize=cameraTarget->appearance.getSize();
	centerDrawingPos.x=(screenWidth-frameSize.x)/2;
	centerDrawingPos.y=(screenHeight-frameSize.y)/2;
	mode |= FOCUS;
}

void ScreenController::cameraOff()
{
	cameraTarget=0;
	mode &= FOCUS^0xffffffff;
}

void ScreenController::setMode(int m) {mode=m;}

void ScreenController::startShaking()
{ mode |= SHAKE; }
void ScreenController::stopShaking()
{
	mode &= (SHAKE^0xffffffff);
//	SET_BIT_OFF(mode,2);
}


void ScreenController::centerCombatUnit()
{
	if(cameraTarget)
	{
		if(!cameraTarget->isAlive())
		{
			cameraTarget=0;
			return;
		}

		POINT ini = theMap->getInitialPosition();
		POINT uPos = cameraTarget->getScreenPosition();
		theMap->setInitialPosition( ini.x+=centerDrawingPos.x-uPos.x,
									ini.y+=centerDrawingPos.y-uPos.y);
	}
}


//+++ Set the cemera in appropriate position, with currrent displaying mode
//+++ (Mode SHAKE can coexist along with FOCUS or ROLL)
void ScreenController::placeCamera()
{
	if(mode & FOCUS)
	{
		if(cameraTarget && cameraTarget->isAlive())
		{
			iniRecord = theMap->getInitialPosition();
			POINT uPos = cameraTarget->getScreenPosition();

			theMap->setInitialPosition( iniRecord.x+=centerDrawingPos.x-uPos.x,
										iniRecord.y+=centerDrawingPos.y-uPos.y);
		}
		else
			cameraTarget=0;
	}
	else if(mode & ROLL)
	{
		POINT uPos = theMap->getFloorPosition(rollDestination.x,rollDestination.y);
		POINT frameSize=theMap->getFloorSize();
		POINT dist = {	(screenWidth-frameSize.x)/2-uPos.x,			// record x distance and y distance
						(screenHeight-frameSize.y)/2-uPos.y};

		//MessageBox(NULL,"","",MB_OK);

		int d=sqrt(dist.x*dist.x+dist.y*dist.y);		// absolute distance
		//if(d<1320)
		//	mode &= ROLL^0xffffffff;

		//char buff[40]="";
		//sprintf(buff,"After %d %d. distance=%d.", dist.x,dist.y,d);
		//MessageBox(NULL,buff,"",MB_OK);


		if(d>=rollStep)
		{
			//MessageBox(NULL,"","",MB_OK);
			
			POINT speed = {	rollStep*dist.x/d,
							rollStep*dist.y/d};
			theMap->setInitialPosition(	iniRecord.x+=speed.x,
										iniRecord.y+=speed.y);
		}
		else
			mode &= ROLL^0xffffffff;
	}

/*
	char buff[40]="";
		sprintf(buff,"focus: %d %d. %d %d", iniRecord.x,iniRecord.y, theMap->getInitialPosition().x, theMap->getInitialPosition().y);
		MessageBox(NULL,buff,"",MB_OK);
*/

	if(mode & SHAKE)
	{
		int s=shaking/2;
		theMap->setInitialPosition( iniRecord.x+=RandomInt(-s,s),
									iniRecord.y+=RandomInt(-s,s));
	}

}


void ScreenController::setGratitude(int g)
{shaking=g;}

void ScreenController::setRollStep(int rs)
{rollStep=rs;}

void ScreenController::rollToCell(int gx,int gy)
{
	cameraOff();
	rollDestination.x=gx;
	rollDestination.y=gy;
	mode |= ROLL;
}

void ScreenController::pointCameraAtCell(int x,int y)
{
	/*
	POINT ini = theMap->getInitialPosition();
	POINT uPos = theMap->getFloorPosition(x,y);
	POINT frameSize=theMap->getFloorSize();
	theMap->setInitialPosition( ini.x+=(screenWidth-frameSize.x)/2-uPos.x,
								ini.y+=(screenHeight-frameSize.y)/2-uPos.y);
	iniRecord = theMap->getInitialPosition();
	*/
	iniRecord = theMap->getInitialPosition();
	POINT uPos = theMap->getFloorPosition(x,y);
	POINT frameSize=theMap->getFloorSize();
	theMap->setInitialPosition( iniRecord.x+=(screenWidth-frameSize.x)/2-uPos.x,
								iniRecord.y+=(screenHeight-frameSize.y)/2-uPos.y);
	mode=IDLE;
}

//////////////////////////////////////////////////////////////////
PlayerControl::PlayerControl(DInputPackage* i,Leader* p)
: pUnit(p), dinput(i)
{pUnit->setSearchingRange(pUnit->getFightingRange());}

PlayerControl::PlayerControl(DInputPackage* i):pUnit(0), dinput(i)
{}

Leader* PlayerControl::getPlayer()
{return pUnit;}


void PlayerControl::setPlayer(Leader* l)
{
	pUnit=l;
 if(pUnit)
	pUnit->setSearchingRange(pUnit->getFightingRange());
}


void PlayerControl::process()
{
	if(dinput->updateKey())
	{

		POINT nd={0,0};
		
		if(dinput->checkKey(DIK_NUMPAD1))
			nd.y=1;
		else if(dinput->checkKey(DIK_NUMPAD9))
			nd.y=-1;
		else if(dinput->checkKey(DIK_NUMPAD3))
			nd.x=1;
		else if(dinput->checkKey(DIK_NUMPAD7))
			nd.x=-1;
		else if(dinput->checkKey(DIK_NUMPAD8))
		{
			nd.x=-1;
			nd.y=-1;
		}
		else if(dinput->checkKey(DIK_NUMPAD4))
		{
			nd.x=-1;
			nd.y=1;
		}
		else if(dinput->checkKey(DIK_NUMPAD6))
		{
			nd.x=1;
			nd.y=-1;
		}
		else if(dinput->checkKey(DIK_NUMPAD2))
		{
			nd.x=1;
			nd.y=1;
		}


		if(dinput->checkKey(DIK_ESCAPE))
			PostQuitMessage(NULL);

		if(dinput->checkKey(DIK_SPACE))
		{
			if(pUnit && pUnit->target)
				pUnit->attack(*pUnit->target);
		}

		if(pUnit && pUnit->isAlive())
		{
			pUnit->setDirection(nd);
			pUnit->update();	
		}
	}

}


