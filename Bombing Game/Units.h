//////////////////////////////////////////////////////////////
//	Programmer:		Yuhan Zhang
//	EMail:			Yuhan_Zhang@netease.com
//	Date:			July 2, 2003
//	project:		Units that are used in the bombing game,
//					including walking character and bombs
//
//////////////////////////////////////////////////////////////

//===  Note: This file needs "Sprite.h" and <typeinfo>
//===		 Link with "ddraw.lib dxguid.lib dinput8.lib dsound.lib"

/*--------------------------------------------------------
 * Unit
 *		It's a base class of the other units classes.
 *
 *		In the class 'Unit', there's only one varible stored
 * in order to indicate whether this unit is effective or not.
 *--------------------------------------------------------
 */
class Unit
{
	public:
		bool effective;
		CSprite* appearance;
		POINT position;
		int speed;
		
		Unit() : effective(true) {}
		virtual ~Unit() { delete appearance; }
		virtual void move(int)=0;
};

/*--------------------------------------------------------
 * WalkingPerson
 *		A person that can move on the screen (only horizontally)
 * with a certain speed when the function 'move' is called, if
 * he/she is still alive.
 *--------------------------------------------------------
 */
class WalkingPerson : public Unit
{

	private:
		void updatePosition(int);

	public:
		bool alive;				// indicate whether it's alive.
		enum {MIN_POS=16, MAX_POS= 640-16};		
		static CS_Series* death;
		
		
		WalkingPerson(CSprite*, POINT p);
		virtual ~WalkingPerson() {}
		void move(int direction);
		void getKilled();
		void setSpeed(int);
		int getPosition();
};



/*--------------------------------------------------------
 * AI_WalkingPerson
 *		A computer controled walking person. It keeps walking
 * to the random destination.
 *--------------------------------------------------------
 */
class AI_WalkingPerson : public WalkingPerson
{
	private:
		int destination;
		int direction;
			
	public:
		AI_WalkingPerson(CSprite*, POINT position);
		virtual ~AI_WalkingPerson() {}
		void setNewDestination();
		bool isArrived();
		void process();
		int getDirection();
};



/*--------------------------------------------------------
 * Bomb
 *		A bomb will drop from the mid-air and kill the
 * people ('WalkingPerson's) on the ground.
 *--------------------------------------------------------
 */
class Bomb : public Unit
{
	private:
		static List* target;				// list of possible candidate
		int range;					// used for searching for victims.
		static int groundLevel;			// used for the condition of
									// exploding.
		bool exploded;				// whether the bomb has exploded
		
		void kill(WalkingPerson*);
		

	public:
		static CS_Series* explosion;
		
		Bomb(CSprite*, POINT);
		virtual ~Bomb() {}
		void move(int);
		static void setTarget(List*);	// set a list of possible target.
		void setRange(int);				// set the damage range of the explosure
		static void setGroundLevel(int);
		static int getGroundLevel();
};




///////////////////////////////////////////////////////////
//================  implementation section ================
///////////////////////////////////////////////////////////




//----------- memebers of 'WalkingPerson' ------------------




CS_Series* WalkingPerson::death=0;

WalkingPerson::WalkingPerson(CSprite* s, POINT currentPos)
: alive(true)
{
	appearance=s;
	speed = 3;
	position = currentPos;
	appearance->setDrawPosition(currentPos.x, currentPos.y);
	
}


//+++ Move the person according to the given direction
//+++ (However, don't move the person if s/he's not alive
//+++  or if the movement will bring her/him out of bound.
void WalkingPerson::move(int direction)
{

	if(alive==true)
	{
		updatePosition(direction);
		POINT ap = appearance->getFrameSize();
		appearance->setDrawPosition(position.x - ap.x/2, position.y-ap.y);
	}
	else
	{
		if(((CS_Series*) appearance)->isMaxState() && appearance->theTimer.hasEnoughDelay())
			effective = false;
	}

}



void WalkingPerson::setSpeed(int s)
{
	appearance->theTimer.timeDelay /= s/speed;
	speed = s;
}


//+++ kill the person. Set the living status to dead and
//+++ change the sprite to display how it's killed.
void WalkingPerson::getKilled()
{
	alive = false;
	
	POINT dp = appearance->getDrawPosition();
	delete appearance;
	
	appearance = new CS_Series(*death);
	appearance->setDrawPosition(dp.x, dp.y);


}


//+++ change the person's position on the screen
void WalkingPerson::updatePosition(int direction)
{
	if(direction==CS_Walking::RIGHT)
		position.x += speed;
	else if (direction==CS_Walking::LEFT)
		position.x -= speed;

							// bring the out-of-bound person back.
	if(position.x < MIN_POS)	
		position.x = MIN_POS;

	if(position.x > MAX_POS)
		position.x = MAX_POS;
		
}

//-------------- members of 'AI_WalkingPerson' --------------------




AI_WalkingPerson::AI_WalkingPerson(CSprite* s, POINT p)
				: WalkingPerson(s,p)
{
	destination=0;
	direction=CS_Walking::RIGHT;
}

//+++ Set an random destination
void AI_WalkingPerson::setNewDestination()
{
	int currentPosition = position.x;
	
	do
	{
		destination = RandomInt(MIN_POS, MAX_POS);
	}
	while(destination==currentPosition);

	direction = destination-currentPosition <0 ? CS_Walking::LEFT : CS_Walking::RIGHT;
	
}

//+++ According to the person's current position (p), determine
//+++ whether the person has arrived the destination or not.
bool AI_WalkingPerson::isArrived()
{
   int p=position.x;
   
   return    p==destination
		 || (p>destination && direction==CS_Walking::RIGHT)
		 || (p<destination && direction==CS_Walking::LEFT);
}


//+++ Decide a direction and make a movement when the person is alive.
void AI_WalkingPerson::process()
{

		if(isArrived())
			setNewDestination();
		else
			move(direction);
}


int AI_WalkingPerson::getDirection()
{ return direction; }





//----------------- members of 'Bomb' ----------------------



List* Bomb::target=0;
CS_Series* Bomb::explosion=0;
int Bomb::groundLevel=0;

Bomb::Bomb(CSprite* s, POINT p) : exploded(false)
{
	appearance=s;
	position=p;
	speed= 5;

	POINT ap = appearance->getFrameSize();
	appearance->setDrawPosition(position.x - ap.x/2, position.y-ap.y);
}

void Bomb::setRange(int r)
{range = r;}

void Bomb::setTarget(List* t)
{target=t;}


void Bomb::setGroundLevel(int gl)
{groundLevel=gl;}

int Bomb::getGroundLevel()
{return groundLevel;}

//+++ If the bomb is not at exploding condition, keep dropping
//+++ it. If it is, stop any movement of its position and 
//+++ search the list to kill canidates within its range.
void Bomb::move(int)
{
	if(exploded)	  // after it's exploded, check the sprite is finished.
	{
		if(((CS_Series*) appearance)->isMaxState())
		{
		
			// THE CODE BELOW IS A PATCH TO DISPLAY THE EXPLODING SPRITE CORRECTLY.
			if(appearance->state.y==2)
				effective = false;		// sprite finished here
			else
			{
				if(appearance->state.y==1)
				{
					((CS_Series*) appearance)->setMaxState(1);
				}
				
				appearance->state.x=0;
				appearance->state.y++;

			} // PATCH ENDS HERE
		}
	}
	else			
	{					// Check if it hit the ground.
		if(position.y>= groundLevel)
		{
			exploded = true;

			WalkingPerson* wp;		
			for(int i=0;i<target->getSize();i++)
			{
				wp = (WalkingPerson*) target->getData(i);
				if(wp->alive)
				{
								// get the distance to the victim
					int distance = wp->position.x-position.x;
					distance = distance < 0 ? - distance : distance;

								// Kill the person in its range.
					if(distance < range)
					{
						wp->alive=false;
						wp->getKilled();
					}
				}
			}
							
			delete appearance;
			appearance = new CS_Series(*explosion);
			POINT fs = appearance->getFrameSize();
			appearance->setDrawPosition(position.x - fs.x/2, position.y - fs.y);
		}
		else
		{					// move the bomb if it's in air.
			position.y += speed;
			POINT dp = appearance->getDrawPosition();
			appearance->setDrawPosition(dp.x, dp.y += speed);
		}


	}



}


		

