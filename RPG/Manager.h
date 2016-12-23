/*------------------------------------------------------------
-- Classes in this file:
--		EffectManager
--		UnitManager
--		CombatUnit
------------------------------------------------------------*/

#ifndef _UNIT_AND_EFFECT_LIST_MANAGER
#define _UNIT_AND_EFFECT_LIST_MANAGER

#include <typeinfo.h>
#include "Utility.h"
#include "Game45.h"
#include "AI.h"
#include "CombatStatus.h"

class CombatUnit;
class CombatAI;
class Leader;
class FollowerAI;
/*------------------------------------------------------------
--	EffectManager
--	*	Effect manager will keep a list of effect.
--  *   Adding effect to the list is allowed.
--	*	The effects will be processed, and be removed after
--		being expired.
--  *	Effects will be output to map as screen objects.
--		(screen objects will be displayed on the screen by
--		 the map object.)
-- (Note: Though called 'EffectManager', it only handles
--        'ProcessableEffect'.)
------------------------------------------------------------*/
class EffectManager
{
	private:
		List* theList;		// a list for storing effects
		static GameMap45* theMap;

	public:
		EffectManager(int);
		~EffectManager();

		static void setMap(GameMap45*);
		void clearList();
		bool addEffect(ProcessableEffect*);
		void insertScreenObject();
		List* getListPointer();
		void updateList();
};
	


/*------------------------------------------------------------
--	UnitManager
--  *	A class similar to Effect manager, except that the data
--		will be unit objects, instead of effects.
--	*	There will be two lists: one for player units, the other
--		for computer units (enemies).
--	*	A unit is removed when it is dead.
------------------------------------------------------------*/

class UnitManager
{
	private:
		List* playerUnitList;
		List* enemyUnitList;

	public:
		static GameMap45* theMap;

	public:
		UnitManager(int);
		~UnitManager();

		enum {PLAYER_LIST, ENEMY_LIST};
		static void setMap(GameMap45*);
		void clearPlayerList();
		void clearEnemyList();
		bool addToPlayerList(CombatUnit*);
		bool addToEnemyList(CombatUnit*);

		void insertScreenObject();
		void updateLists();
		void updateTargets();		//search the closest target for each unit.
		void processMovement();


		List* getPlayerListPointer();
		List* getEnemyListPointer();
		int getTotal();

		CombatUnit* searchClosest(int x,int y, int listID);
		CombatUnit* searchByName(char*);
		CombatUnit* removeUnitByName(char* name, int listID);
		void enemyToFriend(char* name);
};


//////////////////////////////////////////////////////////////////////

class CombatUnit : public GameUnit, public CombatStatus
{
	protected:
		int searchingRange; // the range to search for an enemy.
		int fightingRange; // the max distance to reach an enemy;

	public:
		char name[21];		// the max length for a name is 20. (1 for null terminal)
		CombatUnit* target;
		CombatAI* combatAI;
		WalkAI walkAI;

		enum {BLOOD_0=0, BLADE_0};
		static Tile* effectTile[2];
		static int numberOfFrames[2];
		static EffectManager* eManager;


		CombatUnit();
		virtual ~CombatUnit();
		void createCombatAI();
		void releaseCombatAI();
		bool isAIControlled();
		void createMonsterAI();
		void createCowardAI();
		void createPatrolAI(int,int,int,int);

		void setSearchingRange(int);
		void setFightingRange(int);
		int	getSearchingRange();
		int getFightingRange();

		virtual void attack(CombatUnit&);
		ProcessableEffect* createProcessableEffect(int effectType);


		friend class CombatAI;
		friend class MonsterAI;
		friend class FollowerAI;
		friend class CowardAI;
		friend class PatrolAI;
		friend class UnitManager;

};
//////////////////////////////////////////////////////////////////////

class CombatAI
{
	public:
		CombatUnit* theUnit;
		enum {	D_IDLE=0,
				D_ATTACK,		// decisions
				D_WALK};

		CombatAI(CombatUnit*);
		virtual ~CombatAI();
		virtual int makeAttackDecision();				// determine movement in order to make a valid attack.
};

//////////////////////////////////////////////////////////////////////
class MonsterAI : public CombatAI
{
	public:
		MonsterAI(CombatUnit*);
		virtual ~MonsterAI();
		virtual int makeAttackDecision();

};

//////////////////////////////////////////////////////////////////////
class PatrolAI : public CombatAI
{
	private:
		POINT p[2];		// p : patrol points. This unit will patrol between these two points
		POINT* pd;		// pd: pointer to the current destination.
	public:
		PatrolAI(CombatUnit*,int,int,int,int);
		virtual int makeAttackDecision();
};

//////////////////////////////////////////////////////////////////////
class CowardAI : public CombatAI
{
	public:
		CowardAI(CombatUnit*);
		virtual int makeAttackDecision();

};

//////////////////////////////////////////////////////////////////////
/*----------------------------------------------------------------
-- It controls the unit to fight when a target is insight,
-- When no target exists, the unit will try to move into appropriate
-- distance to the leader, following the leader's walking.
----------------------------------------------------------------*/

class FollowerAI : public CombatAI
{
	protected:
		Leader* theLeader;
	public:
		FollowerAI(CombatUnit* me, Leader* leader);
		virtual ~FollowerAI();
		virtual int makeAttackDecision();
};

//////////////////////////////////////////////////////////////////////
/*----------------------------------------------------------------
-- Basically a Follower AI, but it will run randomly when it has an
-- approapriate distance to the leader.
----------------------------------------------------------------*/
class FollowerMonsterAI : public FollowerAI
{
	protected:
		MonsterAI* monsterAI;

	public:
		FollowerMonsterAI(CombatUnit* me, Leader* leader);
		virtual ~FollowerMonsterAI();
		virtual int makeAttackDecision();
};

//////////////////////////////////////////////////////////////////////
class Leader : public CombatUnit
{
	private:
		List* followerAIList;
		int farthest;		// the farthest distance the followers keep
		int closest;		// the closest distance the followers keep
	public:
		Leader(int listSize, int c,int f);
		virtual ~Leader();
		bool addFollower(CombatUnit*);
		bool addFollowerMonster(CombatUnit*);
		void setClosest(int);
		void setFarthest(int);
		void setCapacity(int);
		
		friend class FollowerAI;
		friend class FollowerMonsterAI;
};

#endif