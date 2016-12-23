#include "Manager.h"

GameMap45* EffectManager::theMap=0;

EffectManager::EffectManager(int listSize)
{
	theList =  new List(listSize);
}

EffectManager::~EffectManager()
{
	clearList();	
	delete theList;
}


void EffectManager::setMap(GameMap45* m)
{theMap=m;}

void EffectManager::clearList()
{
	while(theList->getSize()>0)
		delete (ProcessableEffect*) theList->remove(0);	
}

List* EffectManager::getListPointer()
{return theList;}

bool EffectManager::addEffect(ProcessableEffect* pE)
{ return theList->add(pE);}



//+++ Create and insert the screen objects of the effects to
//+++ the screen object list in the map.
//+++ (The map will destroy the dynamically created screen
//+++  objects.)
//+++ (WARNING: This function must be called after appropriate
//+++			size is allocated for the screen object list.
//+++			So, use this only after "theMap->renewScreenObjectList()")
void EffectManager::insertScreenObject()
{
	for(int i=0;i<theList->getSize();i++)
	{
		
		ProcessableEffect* pE = (ProcessableEffect*) theList->getData(i);

		ScreenObject* pSO = new ScreenObject();
		*pSO = pE->getScreenObject();

		theMap->addScreenObject(pSO, pE->getDistance());

	
	}
}


void EffectManager::updateList()
{

	int i=0;
	ProcessableEffect* pE;

	while(i<theList->getSize())
	{
		pE = (ProcessableEffect*) theList->getData(i);
		if(pE->process())		// if the effect is expired after process
		{
			theList->remove(i);
			delete pE;
		}
		else
			++i;
	}
	


}



/////////////////////////////////////////////////////////////////////////
GameMap45* UnitManager::theMap=0;

UnitManager::UnitManager(int listsSize)
{
	playerUnitList = new List(listsSize);
	enemyUnitList = new List(listsSize);
}


UnitManager::~UnitManager()
{
	clearPlayerList();
	clearEnemyList();
	delete playerUnitList;
	delete enemyUnitList;
}

void UnitManager::setMap(GameMap45* m)
{ theMap=m;}

void UnitManager::clearPlayerList()
{
	while(playerUnitList->getSize()>0)
		delete (CombatUnit*) playerUnitList->remove(0);	
}

void UnitManager::clearEnemyList()
{
	while(enemyUnitList->getSize()>0)
		delete (CombatUnit*) enemyUnitList->remove(0);	
}

List* UnitManager::getPlayerListPointer()
{ return playerUnitList;}

List* UnitManager::getEnemyListPointer()
{ return enemyUnitList;}


bool UnitManager::addToPlayerList(CombatUnit* pGU)
{ return playerUnitList->add(pGU);}

bool UnitManager::addToEnemyList(CombatUnit* pGU)
{return enemyUnitList->add(pGU);}

void UnitManager::insertScreenObject()
{
	for(int i=0;i<playerUnitList->getSize();i++)
	{
		CombatUnit* pUnit = (CombatUnit*) playerUnitList->getData(i);
		ScreenObject* pSO = new ScreenObject();
		*pSO = pUnit->getScreenObject();
		theMap->addScreenObject(pSO, pUnit->getDistance());
	}

	for(int j=0;j<enemyUnitList->getSize();j++)
	{
		CombatUnit* pUnit = (CombatUnit*) enemyUnitList->getData(j);
		ScreenObject* pSO = new ScreenObject();
		*pSO = pUnit->getScreenObject();
		theMap->addScreenObject(pSO, pUnit->getDistance());
	}

}

void UnitManager::updateTargets()
{
		//--- search for the closest target for each unit;
	CombatUnit** playerTarget = new CombatUnit*[playerUnitList->getSize()];
	int* dist1 = new int [playerUnitList->getSize()];
	CombatUnit** enemyTarget = new CombatUnit*[enemyUnitList->getSize()];
	int* dist2 = new int [enemyUnitList->getSize()];

	const int MAX=999;		// a value that is far enough to be an initial distance value.
	int i;
	int j;
	for(i=0;i<playerUnitList->getSize();i++)
		dist1[i]=MAX;
	for(j=0;j<enemyUnitList->getSize();j++)
		dist2[j]=MAX;


	for(i=0;i<playerUnitList->getSize();i++)
	{
		CombatUnit* pUnit = (CombatUnit*) playerUnitList->getData(i);
		for(j=0;j<enemyUnitList->getSize();j++)
		{
			CombatUnit* eUnit = (CombatUnit*) enemyUnitList->getData(j);
			int temp=pUnit->distanceFrom(*eUnit);
			if(temp < dist1[i])
			{
				dist1[i]=temp;
				playerTarget[i]=eUnit;
			}
			if(temp < dist2[j])
			{
				dist2[j]=temp;
				enemyTarget[j]=pUnit;
			}
		}
	}

		//-- set the closest target to the unit;
	for(i=0;i<playerUnitList->getSize();i++)
	{
		CombatUnit* pUnit = (CombatUnit*) playerUnitList->getData(i);
		if(dist1[i]>pUnit->searchingRange)	// if the closest enemy is out of the searching range
			pUnit->target=0;
		else
			pUnit->target=playerTarget[i];
	}
	for(j=0;j<enemyUnitList->getSize();j++)		// similar to above process.
	{
		CombatUnit* eUnit = (CombatUnit*) enemyUnitList->getData(j);
		if(dist2[j]>eUnit->searchingRange)
			eUnit->target=0;
		 else
			eUnit->target=enemyTarget[j];
	}

	delete[] playerTarget;
	delete[] enemyTarget;
	delete[] dist1;
	delete[] dist2;
}

void UnitManager::processMovement()
{
	int i,j;

	 	//---  process unit movement.
	for(i=0;i<playerUnitList->getSize();i++)
	{
		CombatUnit* pUnit = (CombatUnit*) playerUnitList->getData(i);
		if(pUnit->combatAI)
		{
			int decision = pUnit->combatAI->makeAttackDecision();
		}
	}
	
	for(j=0;j<enemyUnitList->getSize();j++)
	{
		CombatUnit* pUnit = (CombatUnit*) enemyUnitList->getData(j);
		if(pUnit->combatAI)
		{
			int decision = pUnit->combatAI->makeAttackDecision();
		}

	}
}


//+++ Remove dead unit from the list.
void UnitManager::updateLists()
{
	int i=0;
	CombatUnit* pCU;
	

	while(i<playerUnitList->getSize())
	{
		pCU = (CombatUnit*)playerUnitList->getData(i);

		if(!((CombatUnit*)pCU)->isAlive())
		{
			playerUnitList->remove(i);
			delete pCU;
		}
		else
			++i;
	}

	i=0;
	while(i<enemyUnitList->getSize())
	{

		pCU = (CombatUnit*)enemyUnitList->getData(i);
		
		if(!((CombatUnit*)pCU)->isAlive())
		{
			enemyUnitList->remove(i);
			delete pCU;
		}
		else
			++i;

	}


}

//+++ Return the total number of units in both lists.
int UnitManager::getTotal()
{return (enemyUnitList->getSize())+(playerUnitList->getSize());}


//+++ Find the unit that is closest to the given position.
//+++ If no unit is found, return NULL;
CombatUnit* UnitManager::searchClosest(int x,int y, int listID)
{
	List* theList;
	CombatUnit* rev=0;		// return value
	CombatUnit* temp;		// temp varible for a combat unit.
	int dist;	//distance;
	int tempDist;	// distance for temporary unit.

	if(listID==PLAYER_LIST)
		theList=playerUnitList;
	else
		theList=enemyUnitList;

	if(theList->getSize()>0)
	{
		rev=(CombatUnit*) playerUnitList->getData(0);
		dist=rev->distanceFrom(x,y);
	}

	for(int i=1;i<theList->getSize();i++)
	{
	    temp=(CombatUnit*) playerUnitList->getData(i);
		if(temp->isAlive() && tempDist < dist)
		{
			rev=temp;
			dist=tempDist;
		}
	}

	return rev;

}


CombatUnit* UnitManager::searchByName(char* name)
{
	List* theList=playerUnitList;
	int i=0;

	for(int j=0;j<2;j++)
	{
		while(i<theList->getSize() && strcmp(((CombatUnit*) theList->getData(i))->name,name)!=0)
		++i;
	
		if(i<theList->getSize())
			return (CombatUnit*) theList->getData(i);
		else
		{
			theList=enemyUnitList;
			i=0;
		}
	}

	return i<theList->getSize()? (CombatUnit*) theList->getData(i) : 0;
}


//+++ Given the unit name and its list ID, find the unit and return its index.
//+++ If the unit is not found, return -1;
CombatUnit* UnitManager::removeUnitByName(char* name, int listID)
{
	List* theList;
	if(listID==PLAYER_LIST)
		theList=playerUnitList;
	else
		theList=enemyUnitList;

	int i=0;
	while(i<theList->getSize() && strcmp(((CombatUnit*) theList->getData(i))->name,name)!=0)
		++i;

	return i<theList->getSize() ? (CombatUnit*)theList->remove(i) : 0;
}

void UnitManager::enemyToFriend(char* name)
{
	List* theList=enemyUnitList;
	int i=0;
	while(i<theList->getSize() && strcmp(((CombatUnit*) theList->getData(i))->name,name)!=0)
		++i;
	if(i<theList->getSize())
		playerUnitList->add(enemyUnitList->remove(i));
}

//////////////////////////////////////////////////////////////////
Tile* CombatUnit::effectTile[2]= {0,0};
int CombatUnit::numberOfFrames[2] = {0,0};
EffectManager* CombatUnit::eManager=0;

CombatUnit::CombatUnit()
:walkAI(&position,10),searchingRange(3),fightingRange(1),target(0),combatAI(0)
{}

CombatUnit::~CombatUnit()
{ releaseCombatAI();}

void CombatUnit::createCombatAI()
{ combatAI = new CombatAI(this);}

void CombatUnit::releaseCombatAI()
{
	if(combatAI)
		delete combatAI;
	combatAI=0;
}

void CombatUnit::createMonsterAI()
{	releaseCombatAI();
	combatAI = new MonsterAI(this);
}

void CombatUnit::createCowardAI()
{	releaseCombatAI();
	combatAI = new CowardAI(this);
}


void CombatUnit::createPatrolAI(int x1,int y1, int x2, int y2)
{	releaseCombatAI();
	combatAI = new PatrolAI(this,x1,y1,x2,y2);
}

void CombatUnit::setSearchingRange(int r)
{ searchingRange=r;}

void CombatUnit::setFightingRange(int r)
{ fightingRange=r;}

int CombatUnit::getSearchingRange()
{ return searchingRange;}

int CombatUnit::getFightingRange()
{ return fightingRange;}


void CombatUnit::attack(CombatUnit& enemy)
{
	int result=CombatStatus::attack(enemy);		// deduce health point

						// face the enemy
	POINT d = walkAI.approach(enemy.getPosition());
	appearance.changeDirection(d);
	appearance.update();

	if(result==CombatStatus::DELAY)
		return;

	ProcessableEffect* pPE;
	if(result==CombatStatus::HIT)	// let enemy bleed
	{
		pPE =  enemy.createProcessableEffect(CombatUnit::BLOOD_0);
		pPE->putInForeground();
		pPE->zShift+=theMap->getFloorSize().y/3;
		eManager->addEffect(pPE);
	}
						// let the unit display attack effect.
	pPE = createProcessableEffect(CombatUnit::BLADE_0);
	pPE->putInForeground();
	pPE->zShift+=theMap->getFloorSize().y/3;
	eManager->addEffect(pPE);

}

ProcessableEffect* CombatUnit::createProcessableEffect(int type)
{return new DiminishingEffect(getFloatX(), getFloatY(),0, new Series_TSprite(effectTile[type],numberOfFrames[type]));}


///////////////////////////////////////////////////////////////////
CombatAI::CombatAI(CombatUnit* pCU)
{theUnit=pCU;}

CombatAI::~CombatAI()
{}

//+++ make proper decision and behave.
int CombatAI::makeAttackDecision()
{
	if(theUnit->target)
	{
		if(theUnit->distanceFrom(*theUnit->target)<=theUnit->fightingRange)
		{
			theUnit->attack(*theUnit->target);
			return D_ATTACK;
		}
		else
		{
		  if(theUnit->currentStep==0)
			theUnit->setDirection(theUnit->walkAI.yield(theUnit->walkAI.approach(theUnit->target->getPosition())));
		  theUnit->update();
		  return D_WALK;
		}
	}
	return D_IDLE;
}



////////////////////////////////////////////////////////////////

MonsterAI::MonsterAI(CombatUnit* me) : CombatAI(me)
{}

MonsterAI::~MonsterAI()
{}

int MonsterAI::makeAttackDecision()
{
	if(CombatAI::makeAttackDecision()==D_IDLE)
	{
		if(theUnit->currentStep==0)
		{
			if(!theUnit->walkAI.anyDirectionToFollow())
				theUnit->walkAI.useRandomWalk();

			theUnit->setDirection(theUnit->walkAI.followingDirection());
		}
		theUnit->update();
	}
	return D_WALK;
}

//////////////////////////////////////////////////////////////////////
PatrolAI::PatrolAI(CombatUnit* me,int x1,int y1,int x2,int y2)
: CombatAI(me), pd(p)
{
	p[0].x=x1;
	p[0].y=y1;
	p[1].x=x2;
	p[1].y=y2;
}


int PatrolAI::makeAttackDecision()
{
	int d=CombatAI::makeAttackDecision();
	if(d==D_IDLE)
	{
		if(theUnit->currentStep==0)
		{
			if(theUnit->getPosition()==*pd)
			{
				if(pd==p)
					pd=&p[1];
				else
					pd=p;

			}
			else
				theUnit->setDirection(theUnit->walkAI.yield(theUnit->walkAI.approach(*pd)));
		}
		theUnit->update();
		return D_WALK;

	}
	
	return d;
}

//////////////////////////////////////////////////////////////////////
CowardAI::CowardAI(CombatUnit* u):CombatAI(u)
{}

int CowardAI::makeAttackDecision()
{
	if(theUnit->currentStep==0)
	{
		if(theUnit->target)
			theUnit->setDirection(theUnit->walkAI.yield(theUnit->walkAI.evade(theUnit->target->getPosition())));
		else
		{
			if(!theUnit->walkAI.anyDirectionToFollow())
				theUnit->walkAI.useRandomWalk();
			theUnit->setDirection(theUnit->walkAI.followingDirection());
		}
	}			

	theUnit->update();
	return D_WALK;
}


//////////////////////////////////////////////////////////////////////
Leader::Leader(int listSize, int c,int f) : closest(c),farthest(f)
{followerAIList =  new List(listSize);}

Leader::~Leader()
{
	
	while(followerAIList->getSize()>0)
	{
		CombatUnit* follower = (CombatUnit*)followerAIList->getData(0);
		followerAIList->remove(0);		// must remove the AI before it's destroyed.
		follower->releaseCombatAI();	// Otherwise the destruction falls into a loop.
		follower->combatAI = new MonsterAI(follower);
	}

	delete followerAIList;
}

bool Leader::addFollower(CombatUnit* follower)
{
			// if the list is full or the follower has followed other unit, don't add it.
	if(followerAIList->isFull() || typeid(follower->combatAI)==typeid(FollowerAI))
		return false;
	
	follower->releaseCombatAI();	// release pre-existen combat AI.
	follower->combatAI = new FollowerAI(follower,this);	// create new follower AI.
	followerAIList->add(follower);
		return true;
}

bool Leader::addFollowerMonster(CombatUnit* follower)
{
			// if the list is full or the follower has followed other unit, don't add it.
	if(followerAIList->isFull() && typeid(follower->combatAI)==typeid(FollowerAI))
		return false;
	
	follower->releaseCombatAI();	// release pre-existen combat AI.
	follower->combatAI = new FollowerMonsterAI(follower,this);	// create new follower AI.
	followerAIList->add(follower);
		return true;

}

void Leader::setClosest(int c)
{closest=c;}

void Leader::setFarthest(int f)
{farthest=f;}

void Leader::setCapacity(int c)
{
	List* newList = new List(c);

	int max= followerAIList->getSize() > c? followerAIList->getSize():c;

			// copy the entry from old list to the new one. and clear the ones
			// that are remained in the old list when the new list is not big
			// enough to hold them.
	for(int i=0;i<max;i++)
		newList->add(followerAIList->remove(0));
	while(followerAIList->getSize()>0)
		((CombatUnit*)followerAIList->remove(0))->createMonsterAI();

	delete followerAIList;
	followerAIList=newList;
}

//////////////////////////////////////////////////////////////////////
FollowerAI::FollowerAI(CombatUnit* me, Leader* leader)
:CombatAI(me),theLeader(leader)
{}

int FollowerAI::makeAttackDecision()
{
	if(CombatAI::makeAttackDecision()==D_IDLE)
	{
		if(theLeader->target)
		{
			theUnit->target=theLeader->target;
			return CombatAI::makeAttackDecision();
		}
		else if(theUnit->currentStep==0)
		{
			int dist = theUnit->distanceFrom(*theLeader);
			if(dist > theLeader->farthest)
				theUnit->setDirection(theUnit->walkAI.yield(theUnit->walkAI.approach(theLeader->getPosition())));
			else if(dist< theLeader->closest)
				theUnit->setDirection(theUnit->walkAI.yield(theUnit->walkAI.evade(theLeader->getPosition())));
			else
				theUnit->setDirection(theLeader->getDirection());
		}
		theUnit->update();
	}
	
	return D_WALK;


}

FollowerAI::~FollowerAI()
{
	int index = theLeader->followerAIList->search(theUnit);
	if(index!=-1)
		theLeader->followerAIList->remove(index);
}

//////////////////////////////////////////////////////////////
FollowerMonsterAI::FollowerMonsterAI(CombatUnit* me, Leader* leader)
:FollowerAI(me,leader), monsterAI(new MonsterAI(me))
{}

FollowerMonsterAI::~FollowerMonsterAI()
{delete monsterAI;}

int FollowerMonsterAI::makeAttackDecision()
{
	if(CombatAI::makeAttackDecision()==D_IDLE)
	{
		if(theLeader->target)
		{
			theUnit->target=theLeader->target;
			return CombatAI::makeAttackDecision();
		}
		else if(theUnit->getCurrentStep()==0)
		{
			int dist = theUnit->distanceFrom(*theLeader);
			if(dist > theLeader->farthest)
				theUnit->setDirection(theUnit->walkAI.yield(theUnit->walkAI.approach(theLeader->getPosition())));
			else if(dist< theLeader->closest)
				theUnit->setDirection(theUnit->walkAI.yield(theUnit->walkAI.evade(theLeader->getPosition())));
			else
			{
				monsterAI->makeAttackDecision();
				return D_WALK;
			}
		}
		theUnit->update();
	}
	return D_WALK;
}

