#ifndef _RPG_CONTROL_
#define _RPG_CONTROL_

#include "Manager.h"
#include <stdlib.h>
#include <math.h>
#include <typeinfo.h>
#include "commonDX.h"
#include "Dialog.h"
#include "common.h"
#include "MusicDevice.h"

class Trap : public Processable
{
	private:
		char scriptName[50];
	public:
		Trap(char*);
		virtual ~Trap();
		virtual bool process();		// return true means trap is activated.
		char* getScriptName();
};

///////////////////////////////////////////////////////


class RectTrap : public Trap
{
		RECT rect;
	public:
		static CombatUnit* thePlayer;
		RectTrap(char*, int,int,int,int);

		virtual bool process();

};


///////////////////////////////////////////////////////
class PlayerControl
{	
	private:
		Leader* pUnit;
		DInputPackage* dinput;
	public:

		PlayerControl(DInputPackage*,Leader*);
		PlayerControl(DInputPackage*);

		void process();
		Leader* getPlayer();
		void setPlayer(Leader*);
};

///////////////////////////////////////////////////////
class ScreenController
{
	private:
		GameMap45* theMap;

		int mode;		// FOCUS? ROLL? SHAKE? or IDLE?
		int screenWidth, screenHeight;
		POINT centerDrawingPos;		// the screen position that the unit's drawing position will be moved to.
		CombatUnit* cameraTarget;

		POINT iniRecord;
		int shaking;		// Gratitude of shaking (in pixels)

		int rollStep;		// distance to roll. Must be greater than 1.4. Otherwise it will failed in rolling in many cases.

		POINT rollDestination;	//which cell on the map to roll to


	public:
		
		enum {  IDLE=0,
				FOCUS=1,
			    SHAKE=1<<2,
				ROLL=1<<3};

		ScreenController();
		ScreenController(GameMap45*);
		void setScreenSize(int,int);
		void setCamera(CombatUnit*);
		void cameraOff();
		void setMode(int);
		void centerCombatUnit();
		void placeCamera();
		void pointCameraAtCell(int,int);

		void setGratitude(int);
		void startShaking();
		void stopShaking();

		void rollToCell(int,int);
		void setRollStep(int);




};

///////////////////////////////////////////////////////

/*------------------------------------------------------------
-- ScriptProcessor
--		It manages a trap list, and takes over the control when
--	a trap is activated, reacting according to the script.
------------------------------------------------------------*/
class ScriptProcessor
{
	private:
		GameMap45* theMap;
		DInputPackage* dinput;
		DDrawPackage* ddraw;
		MusicDevice* musicDevice;
		UnitManager* uManager;
		IDirectDrawSurface** face;
		EffectManager* eManager;

		PlayerControl* playerControl;
		ScreenController* screenControl;

		RPG_DialogBox dBox;
		List* trapList;		// a list of traps;
		DelayTimer delayTimer;

		int scriptLevel;	// Which level of scirpt a "runScript()" function is at?
							// For error checking purpose only.

	public:
		enum {PLAYER_LIST=0, ENEMY_LIST};
		
		ScriptProcessor(int maxTrap);
		virtual ~ScriptProcessor();
		void updateList();
		void clearList();
		void restartTheGame();

			// connections
		void setMap(GameMap45*);
		void setDInput(DInputPackage*);
		void setDDraw(DDrawPackage*);
		void setUnitManager(UnitManager*);
		void setEffectManager(EffectManager*);
		void setCharacterSurface(IDirectDrawSurface**);
		void setScreenControl(ScreenController*);
		void setMusicDevice(MusicDevice*);
		void setPlayerControl(PlayerControl*);

			// inner functions
		void repaintMap();
		void waitForConfirm();	// loop until <space>, <Esc> or <Enter> is pressed.
		bool makeDecision();	// return true if 'y' is pressed. return false if 'n' is pressed.
		int	 makeChoice();		// return 0-9, according to the key that is pressed on main keyboard.
		bool addTrap(Trap*);
		void playWithoutPlayer();

			// Script functions
		void loadMap(char* s);
		void showDialog(char* s);
		void setTrap(char* s,int l,int t,int r,int b);
        void runScript(char*);
		Leader* createLeader(char* name, char* statusFile, int faceID,int listID,int x,int y,int capacity, int closest, int farthest);
		void createCombatUnit(char* name, char* statusFile, int faceID,int listID,int x,int y);
		void addFollower(char* leaderName, char* followerName);
		void addFollowerMonster(char* leaderName, char* followerName);
		void activateMonsterAI(char* name);
		void removeAI(char* name);
		void setUnitPos(char* name,int x, int y);
		void unitTalk(char* name,char* talk);
		void setFacingDir(char* name,int dx, int dy);
		void removeUnit(char* name);
		void playMusic(char* name);
		void paintScreen(HBRUSH);


};

//////////////////////////////////////////////////////////


#endif
