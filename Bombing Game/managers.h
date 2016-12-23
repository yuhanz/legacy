//////////////////////////////////////////////////////////////
//	Programmer:		Yuhan Zhang
//	EMail:			Yuhan_Zhang@netease.com
//	Date:			July 3, 2003
//	project:		Managers that that are used in the bombing
//					to controll walking characters and bombs
//
//////////////////////////////////////////////////////////////

// --- needs <typeinfo>, "units.h", "utility.h", "commonDX.h"
// ---  included.

extern DDrawPackage ddraw;


/*--------------------------------------------------------
 * ListManager:
 *		This object will be used to controll the amount
 * of units in the list. It will remove ineffective units
 * and add new ones according to a given time delay. It also
 * move the units and draw them on the screen.
 *
 *	Functions that need to be overrided:
 *			void moveAll();
 *			void addNew();
 *			void drawAll();
 *--------------------------------------------------------
 */
class ListManager
{
	protected:
		List* theList;
		DelayCounter theTimer;
	
	private:
		
		void removeBadUnits();	// remove ineffective units
		virtual void moveAndDraw()=0;
		virtual void addNew()=0;

	public:
		ListManager(int);
		virtual ~ListManager();
		
		void startAdding();			// start to add new units
		void stopAdding();			// stop adding new units
		void setAddingDelay(long);	// set the time delay between adding two units.
		void process();
		List* getList();
		void clearList();				// clear the list.

};



/*--------------------------------------------------------
 * BombManager:
 *		A derived class from 'ListManager':
 * 
 *		It manages the bombs in the list.
 *
 *	(Note: assign a directdraw surface with a bitmap to
 *			varible 'dds_bomb' before using.)
 *--------------------------------------------------------
 */
class BombManager : public ListManager
{
	private:
				
		void moveAndDraw();
		void addNew();

	public:
		IDirectDrawSurface* dds_bomb;

		BombManager(int);
		virtual ~BombManager() {}
};


/*--------------------------------------------------------
 * VictimManager:
 *		A derived class from 'ListManager':
 * 
 *		It manages all the AI controlled walking persons in
 *	the list. However, it left player
 *  controlled character. The 'ListManager' will only d
 * (Note: There are 5 public fields for the people's image,
 *		  which needs to be assigned surface with BMP files
 *		  before using this object.
 *--------------------------------------------------------
 */
class VictimManager : public ListManager
{
	private:
			
		WalkingPerson* playerUnit;
		bool flag;		// if true, add a new person to the leftmost of the screen.
						// if false, add it to the rightmost.
		void moveAndDraw();
		void addNew();
		AI_WalkingPerson* createAIPerson();

	public:
		IDirectDrawSurface* dds_people[4];

		VictimManager(int);
		virtual ~VictimManager() {}
		
		WalkingPerson** getNotifier();
};






///////////////////////////////////////////////////////////
/////		 IMPLEMENTATION SECTION			///////////////
///////////////////////////////////////////////////////////





//----------------- members of 'ListManager' ----------------------
		



ListManager::ListManager(int listSize)
{
	theList =  new List(listSize);
}

ListManager::~ListManager()
{
	clearList();	
	delete theList;
}


void ListManager::clearList()
{
	while(theList->getSize()>0)
		delete (Unit*) theList->remove(0);	
}


//+++ set flag in the timer to start
void ListManager::startAdding()
{ theTimer.start(); }

//+++ set flage in the timer to stop
void ListManager::stopAdding()
{ theTimer.stop(); }

//+++ remove ineffective units in the list.
void ListManager::removeBadUnits()
{
	int i=0;
	Unit* u;

	while(i<theList->getSize())
	{
		u = (Unit*) theList->getData(i);
		if(!u->effective)
		{
			theList->remove(i);
			delete u;		// Note: 'u' is an unit pointer,
		}					//		 'remove()' returns a void pointer.
		else
			++i;
	}
}



void ListManager::process()
{
		removeBadUnits();
		
		if(!theTimer.isStopped())
		{
			addNew();
			//moveAndDraw();
		}
		moveAndDraw();
}


void ListManager::setAddingDelay(long t)
{ theTimer.delay=t; }


List* ListManager::getList()
{ return theList; }


//----------------- members of 'BombManager' ----------------------

BombManager::BombManager(int s)
		   : ListManager(s) {}



//+++ move all the bombs
void BombManager::moveAndDraw()
{
	Bomb* temp;
	
	for(int i=0;i<theList->getSize();i++)
	{
		temp = (Bomb*) theList->getData(i);

		temp->move(NULL);
		temp->appearance->getNextFrame(NULL);
		temp->appearance->draw(ddraw.getTheBoard());
	}
}


//+++ Add new bombs if the timer is started and time delay's enough
void BombManager::addNew()
{
	if(!theTimer.isStopped() && theTimer.hasEnoughDelay() && !theList->isFull())
	{
		POINT p;
		p.x = RandomInt(WalkingPerson::MIN_POS, WalkingPerson::MAX_POS);
		p.y = 0;

		Bomb* temp = new Bomb(new CS_Series(dds_bomb,32,69,0,0,true,1), p);
		temp->setRange(32);

		theList->add(temp);
	}
}




//----------------- members of 'VictimManager' ----------------------




VictimManager::VictimManager(int listSize)
		     : ListManager(listSize) {}


//+++ move and draw all the AI controlled person.
//+++ (NOTE: It won't draw the player controlled person nor
//+++		 attemp to move it. The player unit is taken cared
//+++		 in the class 'Player'.)
void VictimManager::moveAndDraw()
{
	
	for(int i=0;i<theList->getSize();i++)
	{
		WalkingPerson* wp  = (WalkingPerson*) theList->getData(i);
		AI_WalkingPerson* awp = (AI_WalkingPerson*) wp;

		if(wp!=playerUnit)
		{
			awp->process();
			awp->appearance->getNextFrame(awp->getDirection());			
		}
		
		wp->appearance->draw(ddraw.getTheBoard());

	}
}



//+++ Add new bombs if the timer is started and time delay's enough
void VictimManager::addNew()
{
	if(!theTimer.isStopped() && theTimer.hasEnoughDelay() && theList->getSize() < theList->getMax()-1)
		theList->add(createAIPerson());
}


//+++ Randamly create an AI controlled person.
AI_WalkingPerson* VictimManager::createAIPerson()
{
	POINT p;
	
	p.y = Bomb::getGroundLevel();
	
	if(flag)
	   p.x = 640;
	else
	   p.x = -10;
	

	flag = !flag;

	return new AI_WalkingPerson(new CS_Walking(dds_people[RandomInt(0,3)],32,48), p);
}


//+++ Return the address of the pointer to the player controlled unit
WalkingPerson** VictimManager::getNotifier()
{ return &playerUnit; }