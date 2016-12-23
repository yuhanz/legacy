//////////////////////////////////////////////////////////////
//	Programmer:		Yuhan Zhang
//	EMail:			Yuhan_Zhang@netease.com
//	Date:			July 4, 2003
//	Description::		Class player stores basic info ob
//					a player for the bombing game.
//////////////////////////////////////////////////////////////

// --- needs "units.h", "Utility.h", "common.h" to be included.

extern DDrawPackage	ddraw;
extern DInputPackage dinput;

/*--------------------------------------------------------
 * Player:
 *		It contains a device for getting input,
 *	a pointer to the player controlled character
 *	(a 'WalkingPerson'), the walking direction
 *  and a timer to count the invincible time when
 *	the character first enter the game.
 *		This object will receive an input device.
 *	and control the character through this device.
 *		It will need the access to the victim list
 *	because the character controlled
 *	character must be placed into the list to
 *	be bombed after the invincible time passes.
 *	Number of lives will be counted here
 *	as well. Also, This object will be used to
 *	signal the program to display info when "gameove"
 *	is reached.
 *
 *--------------------------------------------------------
 */


class Player
{
	private:	
		IDirectDrawSurface* look;
		WalkingPerson* playerCharacter;
		int walkingDirection;				//Walking direction
		DInputPackage* dinput;
		DelayTimer invincibleTime;
		bool flash;					// indicate to flash or not
		List* victimList;
		int life;					// number of lives.
		WalkingPerson** notifier;	// a pointer to the pointer 'playerUnit'
									// in object Victim Manager.
		
		WalkingPerson* generatePerson();
		bool finished;				// a flag show whether the player is finished.
	
	public:
		
		void init(DInputPackage*, VictimManager*, IDirectDrawSurface*);
		void processInput();		// check keyboard to change walking direction
		void process();				// move the person.
		
		void restart();				// restart an game.
		bool getNewLife();			// bring another character
									// after the first one's killed
		bool isFinished();			// indicate whether a game's finished.

};






/*--------------------------------------------------------
 * StageControl
 *		This object will take care of background music and
 * organize bomb manager and victim manager's action.
 *		After a certain time delay, the stage control will
 * change the stage by replace the background music and the
 * adding frequency of two managers.
 *		It also bring text such as "level 1" and "Game over".
 *--------------------------------------------------------
 */

class StageControl
{
	private:

		Label* title;
		Label* gameover;
		CSprite* message;
				
		DelayTimer theTimer;
		

		static const POINT drawPosition[3];
		static const int STAGE_TIME[9];
		static const int VICTIM_DELAY[9];
		static const int BOMB_DELAY[9];
		IDirectMusicSegment8* pdms[9];
		static WCHAR* fileName[4];
		
		DMusicPackage* pdmp;
		VictimManager* pvm;
		BombManager* pbm;
		Player*	player;
	
	public:
		int stage;
		enum {	STAGE1, TRANSITION1,
				STAGE2, TRANSITION2,
				STAGE3, TRANSITION3,
				WIN, GAMEOVER, DEMO};
		
		
		StageControl() {}
		~StageControl();
		void init(DMusicPackage*,VictimManager*,BombManager*,Player*);
		void attachImages(Label*,Label*,CSprite*);
		
		void setStage(int i);	// set adding delay, music, level, time, etc
		void stop();			// stop music, adding bombs, timer, etc
		void start();			// start music, adding bombs, timer, etc
		void process();			// check time and change stage.
		void release();			// release all the music segments.
		void newGame();

};





//===================================================
//====			IMPLEMENTATION SECTION			=====
//===================================================





//--------------   members of 'StageControl' ------------------



const POINT StageControl::drawPosition[3] = {{120,100}, {120,200}, {120,400}};
const int StageControl::STAGE_TIME[9] = {100*1000, 5*1000, 57*1000, 5*1000, 55*1000, 7*1000, 8*1000, 4*1000, NULL};
const int StageControl::VICTIM_DELAY[9] = {10, 10, 7, 7, 5, 5, 5, 7, 10};
const int StageControl::BOMB_DELAY[9] = {5, 5, 3, 3, 0, 0, 100*1000, 100*1000, 10};
WCHAR* StageControl::fileName[4] = {L"Music0.mid", L"Music1.mid", L"Music2.mid", L"Music3.mid"};


StageControl::~StageControl()
{
	delete title;
	delete gameover;
	delete message;		
}



void StageControl::attachImages(Label* t,Label* g,CSprite* m)
{
	title = t; gameover = g; message = m;
	
	title->transparent = true;
	gameover->transparent = true;
	title->whereToDraw = drawPosition[0];
	gameover->whereToDraw = drawPosition[1];
	message->setDrawPosition(drawPosition[2].x, drawPosition[2].y);
}


void StageControl::init(DMusicPackage* m,VictimManager* v,BombManager* b, Player* p)
{
		
	pdmp=m;
	pvm=v;
	pbm=b;
	player=p;

	for(int i=0;i<8;i++)
		pdms[i] = NULL;
	
	pdmp->loadMusic(fileName[0],pdms[0]);
	pdmp->loadMusic(fileName[1],pdms[2]);
	pdmp->loadMusic(fileName[2],pdms[4]);
	pdmp->loadMusic(fileName[3],pdms[6]);
	
	stage = DEMO;
	start();	
	
	
}



void StageControl::setStage(int i)
{

	theTimer.timeDelay = STAGE_TIME[i];
	pvm->setAddingDelay(VICTIM_DELAY[i]);
	pbm->setAddingDelay(BOMB_DELAY[i]);
	stage = i;	
}

		
void StageControl::stop()
{
	theTimer.stop();
	pvm->stopAdding();
	pbm->stopAdding();
	if(pdms[stage])
		pdmp->stop(pdms[stage]);

}

void StageControl::start()
{
	theTimer.start();
	pvm->startAdding();
	
	if(stage== STAGE1 || stage==STAGE2 || stage==STAGE3 || stage==DEMO)
		pbm->startAdding();
	
	if(pdms[stage])
		pdmp->play(pdms[stage]);

}
		

void StageControl::release()
{
	for(int i=0;i<6;i++)
		if(pdms[i])
		{
			pdmp->unload(pdms[i]);
			pdms[i]->Release();
			pdms[i] = NULL;
		}

}




void StageControl::process()
{
		
	
	if(stage==DEMO)
	{
		title->draw(ddraw.getTheBoard());
		message->getNextFrame(NULL);
		message->draw(ddraw.getTheBoard());
		
		if(dinput.updateKey())
		{
			if(dinput.checkKey(DIK_SPACE))
				newGame();
			else if(dinput.checkKey(DIK_ESCAPE))
				PostQuitMessage(NULL);
		}
		return;
	}

	
	if(player->isFinished() && stage!=GAMEOVER)
	{		
		stop();
		setStage(GAMEOVER);
		start();
	}	
	
	
	if(theTimer.hasEnoughDelay())
	{
		stop();
		
		if(stage==GAMEOVER)
		{
			pvm->clearList();
			pbm->clearList();
			setStage(DEMO);
		}
		else
			setStage(++stage);
		
		start();

	}
	else if(stage==GAMEOVER)	
		gameover->draw(ddraw.getTheBoard());
		
}


void StageControl::newGame()
{
	stop();
	setStage(0);
	start();
	pvm->clearList();
	pbm->clearList();
	player->restart();
	
}




//---------------- members of "Player" -------------------------





void Player::init(DInputPackage* dip, VictimManager* vm, IDirectDrawSurface* pdds)
{
	dinput = dip;
	victimList = vm->getList();
	look = pdds;
	
	notifier = vm->getNotifier();

	invincibleTime.timeDelay = 3000;


}


//+++ Check left and right arraw key. set direction accordingly.
void Player::processInput()
{
	if(dinput->updateKey())
	{
		if(dinput->checkKey(DIK_LEFT))
			walkingDirection = CS_Walking::LEFT;
		else if(dinput->checkKey(DIK_RIGHT))
			walkingDirection = CS_Walking::RIGHT;
		else if(dinput->checkKey(DIK_ESCAPE))
			PostQuitMessage(NULL);
	}
}	
		

//+++	If the person is effective, get input and move it.
//+++ If it's invincible, draw it with flash. Put it
//+++ back to the list to be killed after the invincible
//+++ time passes, and stops the timer.
//+++	Get a new character if the old one's dead, and there
//+++ are still some credits left. If it's not get gameover.
void Player::process()
{
	processInput();
	if(finished)
		return;
	
	playerCharacter->move(walkingDirection);
	playerCharacter->appearance->getNextFrame(walkingDirection);

	if(!playerCharacter->effective)
	{
		if(!getNewLife())
			finished = true;	
	} 

	// if the character's in invincible mode.
	if(!invincibleTime.isStopped())
	{
		if(invincibleTime.hasEnoughDelay())
		{
			victimList->add(playerCharacter);
			*notifier = playerCharacter;
			invincibleTime.stop();				
		}
		else if(flash)
			playerCharacter->appearance->draw(ddraw.getTheBoard());
		
		flash = !flash;
		
				
	}

	

}
		
void Player::restart()
{
	playerCharacter = generatePerson();
	victimList->add(playerCharacter);
	*notifier = playerCharacter;
	life=2;
	finished = false;
	walkingDirection = CS_Walking::STOP;
}

bool Player::getNewLife()
{
	
	--life;
	if(life>=0)
	{
		invincibleTime.start();
		playerCharacter = generatePerson();
		walkingDirection = CS_Walking::STOP;
		return true;
	}
	else
		return false;
	
}


//+++ Generate a new walking person and place it at the center of
//+++ the screen.
WalkingPerson* Player::generatePerson()
{	
	POINT p;
	p.x = 320;
	p.y = Bomb::getGroundLevel();

	return new WalkingPerson(new CS_Walking(look,32,48),p);
}

bool Player::isFinished()
{	return finished; }
