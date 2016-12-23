#ifndef _RPG_GAME_AI_
#define _RPG_GAME_AI_

#include <windows.h>
#include "Game45.h"


class WalkAI
{
 private:
        POINT* myPos;           // my position.
        int mode;                // walking mode, indicating how to get next way of walking
        //Path* path;           // a path to follow.
        POINT direction;        // a direction to follow
        int cellCount;           // the cells to across following a randon direction.
        int maxCount;           // the max cells to across following a randon direction.
        POINT lastlyVisited;    // the cell where I come from last time.
 public:
        enum { APPROACH=0,      // approaching a point mode.
               EVADE,           // evading from a point mode.
               DIRECTION,       // following a direction mode.
               PATH            // following a path mode.
               };
        static GridMap* theMap;

        WalkAI(POINT*,int);
        virtual ~WalkAI();
        void setWalkMode(int);

                              // functions for APPROACH and EVADE mode
        POINT approach(POINT);      // the direction to approach a position. The function uses yield if necessary.
        POINT evade(POINT);         // the direction to evade from a position. The function uses yield if necessary.
        POINT yield(POINT);         // if the given direction is blocked, find a reasonable direction.

                              // functions for PATH mode
        //Path* findPath(POINT);      // find an exact path.
        //POINT followFath();         // follow an exact path.

                              // functions for DIRECTION mode.
        POINT followingDirection();     // follow the direction.
        void setDirection(POINT,int);   // set direction to follow, and cells to across.
        void useRandomWalk();           // set direction to follow, and cells to across.
		bool anyDirectionToFollow();

};


/////////////////////////////////////////////////////////////////////////
/*
class AI_CombatUnit : public CombatUnit
{
	public:
		AI_CombatUnit();

		void MakeAttackDecision();				// determine movement in order to make a valid attack.
		void respond(CombatUnit*);	// determine proper response towards an attack.
};
*/

#endif