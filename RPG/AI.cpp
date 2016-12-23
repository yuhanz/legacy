#include "AI.h"


////////////////////////////////////////////////////////////////
GridMap* WalkAI::theMap=0;

WalkAI::WalkAI(POINT* pos,int max) : myPos(pos), maxCount(max),mode(0),cellCount(0)
{
  direction.x=0;      direction.y=0;             // no direction.
  lastlyVisited.x=(*myPos).x;  lastlyVisited.y=(*myPos).y;         // from no cell. (From a cell that is out of the board.)
}

WalkAI::~WalkAI()
{
 // delete path;
}

void WalkAI::setWalkMode(int m)
{ mode=m;}


void WalkAI::setDirection(POINT d, int cells)
{
 direction=d;
 cellCount=cells;
}

POINT WalkAI::followingDirection()
{
  if(cellCount>0)
  {
     --cellCount;
     return direction;
  }
  else
  {
     const POINT STOP={0,0};         // don't move
     return STOP;
  }
}

void WalkAI::useRandomWalk()
{
   direction.x=rand()%3-1;
   direction.y=rand()%3-1;
   cellCount = rand()%maxCount;
}


POINT WalkAI::approach(POINT p)
{
  POINT d = {0,0};
  int temp;
  if(temp=p.x - myPos->x)    // if no horizontal movement, remain 0. Else, use an appropriate value
    d.x = temp>0 ? 1 : -1;
  if(temp=p.y - myPos->y)
    d.y = temp>0 ? 1 : -1;   // if no vertical movement, remain 0. Else, use an appropriate value

  return d;
}

POINT WalkAI::evade(POINT p)
{
 POINT d = approach(p);
 d.x=-d.x;
 d.y=-d.y;
 return d;
}

//+++ Input:  Given a direction to go.
//+++ Output: Return a propriate direction to go.
//+++         (By appropriate, it means a direction that is open for movement,
//+++          and leads to a position that is likely to approach to its destination
//+++          if the given direction is blocked.
//+++          A trick in the function is that it tries different directions to
//+++          find a open direction to go and avoid going back to a direction
//+++          that it just comes from.)
POINT WalkAI::yield(POINT d)
{
  int temp = d.x+d.y;
  if(temp==1 || temp==-1)      // the case of directioning along row or column
  {
      POINT heading = {(*myPos).x+d.x, (*myPos).y+d.y};                                 // find the cell it's heading to.
      if(theMap->getBlock(heading.x,heading.y)== -1 && heading!=lastlyVisited)         // no block. go for it!
      {
            lastlyVisited = *myPos;
            return d;
      }
      else
      {
          POINT dir_1={0,0},  // direction 1 and 2 will be on oppositive positions.
                dir_2={0,0},  // along the direction perpendicular to heading direction.
                dir_3={-d.x,-d.y};  // direction 3 is the oppisite of heading direction.

          POINT cell_1=*myPos,  // cell 1 and cell 2 will be on oppositive positions.
                cell_2=*myPos,  // along the direction perpendicular to heading direction.
                cell_3={(*myPos).x-d.x,(*myPos).y-d.y};  // cell 3 is the oppisite of heading direction.
          if(d.x==0)
          {
               dir_1.x=-1;
               dir_2.x=+1;
          }
          else            // case d.y==0
          {
               dir_1.y=-1;
               dir_2.y=+1;
          }

          cell_1.x+=dir_1.x;        cell_1.y+=dir_1.y;
          cell_2.x+=dir_2.x;        cell_2.y+=dir_2.y;

          POINT ret;
          if(theMap->getBlock(cell_1.x,cell_1.y)== -1 && cell_1!=lastlyVisited)      // try one side.
              ret = dir_1;
          else if(theMap->getBlock(cell_2.x,cell_2.y)== -1 && cell_2!=lastlyVisited) // try the other side
              ret = dir_2;
          else if(theMap->getBlock(cell_3.x,cell_3.y)== -1 && cell_3!=lastlyVisited) // try the back.
              ret = dir_3;
          else
          {
                    // if all the ways are blocked, either by obstacles or visited flag,
                    // move back to lastly visited position.
              ret.x=lastlyVisited.x-(*myPos).x;
              ret.y=lastlyVisited.y-(*myPos).y;
          }

          lastlyVisited=(*myPos);
          return ret;

      }

  }
  else                        // the case of directioning along diagnal
  {                                               // cells to be considered.
   POINT cellD={(*myPos).x+d.x,(*myPos).y+d.y},   // cell on diagnal
         cellX={(*myPos).x+d.x, (*myPos).y},      // cell on x axis
         cellY={(*myPos).x, (*myPos).y+d.y};      // cell on y axis

   int open=0;          // count cells that is openning.
   bool cx=false,       // cellX is openning?
        cy=false;       // cellY is openning?
   if(theMap->getBlock(cellX.x,cellX.y)== -1)
   {
       ++open;
       cx=true;
   }
   if(theMap->getBlock(cellY.x,cellY.y)== -1)
   {
       ++open;
       cy=true;
   }

   if(theMap->getBlock(cellD.x,cellD.y)==-1 && open==2)   // no block. Just go along the given direction.
   {
       lastlyVisited=*myPos;
       return d;
   }
   else
      {
         POINT rev={0,0};
         switch(open)
         {
          case 0:             // if both yielding ways are blocked,
                cellX.x =(*myPos).x-d.x;          // get the cells opposite to the blocked opennings
                cellY.y =(*myPos).y-d.y;
                if(cellX!=lastlyVisited )         // In the case opposite cellX is not visited,
                {    rev.x=-d.x;                  // try it.
                     rev = yield(rev);
                }
                else                              // In the case opposite cellY is not visited,
                {    rev.y=-d.y;                  // try it.
                     rev = yield(rev);
                }

          break;

          case 1:          // if only one of the yielding ways is blocked,

               if(cx)         // if the single openning is on x axis,
               {
                  if(cellX==lastlyVisited)  // ...and cellX is visited, then yeild to the opposite direction of the visited cell.
                  {
                      rev.x=-d.x;
                      rev = yield(rev);
                  }
                  else                      // ...and cell is not visited, then go for it
                      rev.x =d.x;
               }
               else // cy==true         // similiarly to the case for x axis, except replacing all x's with y's
               {
                  if(cellY==lastlyVisited)  // ...and cellX is visited, then yeild to the opp0site direction of the visited cell.
                  {
                      rev.y=-d.y;
                      rev = yield(rev);
                  }
                  else                      // ...and cell is not visited, then go for it
                      rev.y =d.y;
               }
          break;

          default: // This one catches case 2.
               if(cellX!=lastlyVisited)     // if cell along x-axis is not visited yet, go along x.
                  rev.x=d.x;
               else                         // Else, go along y.
                  rev.y=d.y;
         }

         lastlyVisited = *myPos;
         return rev;
      }
  }
}

bool WalkAI::anyDirectionToFollow()
{ return cellCount>0;}

