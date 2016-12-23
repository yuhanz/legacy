#include "MapFunctions.h"


// Dummy funcitons.
bool operator==(POINT,POINT)
{return true;}							
bool operator>(POINT,POINT)
{return true;}							
bool operator<(POINT,POINT)
{return true;}							
bool operator!=(POINT,POINT)
{return true;}							


/*------------------------------------------------------------
Function:		FindShortestPath
Description:	On the given grid map, find shortest path between
				two points, and store the path into a list.
				Then return the pointer to the list.
				If no path's found, return NULL.
Input:			The map and two points. (One for starting, and one
				for destination)
------------------------------------------------------------*/
list<POINT>* FindShortestPath(GridMap& map, POINT iniPos, POINT destPos)
{
	


	int WIDTH=map.getSize().x, HEIGHT=map.getSize().y;

      bool** record;                // keep a record of the cells to
                                                // see which ones have been considered,
												// so that we won't repeat the work
												// and fall in infinant loop.



      record = new bool*[HEIGHT];
	  for(int j=0;j<HEIGHT;j++)
	  {
		record[j] = new bool[WIDTH];
		  for(int i=0;i<WIDTH;i++)
             record[j][i]= false;				// initialize of record to be "unchecked" (false)
	  }
      


      list<list<POINT>*> stackQueue;           // a queue of stacks.
                                               // note: list<POINT> will be
                                               //       used as a stack.

                     // Initiate the starting point of the pathes.
                     // (They are formed of the adjacent cells of the
                     //  initial point.)
      record[iniPos.x][iniPos.y]=true;
      list<POINT>* pStack = new list<POINT>();
      pStack->push_back(iniPos);
	  stackQueue.push_back(pStack);


             // Start searching until the queue is empty. (no fath's found)
             // Or break when finding a path.
             // (The search will be the same as the way to initiate the pathes,
             //  except this time we use the cell index from the path stack,
             //  instead of using initial position.)
      POINT temp;					// a buffer for the extracted data from the stack.
	  list<POINT>* retVal= NULL;	// return value

	  while(!stackQueue.empty() && !retVal)
      {
         pStack = stackQueue.front();	// extract an unfinished path from the queue.
         stackQueue.pop_front();

         for(int j=-1;j<=1;j++)	    // for all the surrounding cells (actually it also check this cell, too.
		 {							// But it will fail anyway because of the record.)

	 
			 for(int i=-1;i<=1;i++)
            {
			 temp.x = pStack->back().x+i;
             temp.y = pStack->back().y+j;
			 POINT direction = {i,j};

			 // if that cell has been checked for path, or the it's not accessible to be used as part of the path, leave it alone.
			 if( temp.x<0 || temp.y<0 || temp.x>=WIDTH || temp.y>=HEIGHT
				 || record[temp.y][temp.x]!=  false
				 || map.checkAccess(pStack->back(), direction)==false)
				 continue;

             record[temp.y][temp.x]=true;        // mark this cell so that we won't repeat the work.
             list<POINT>* pNewStack = new list<POINT>();
             list<POINT>::iterator it;
             for(it=pStack->begin();it!=pStack->end();it++)// copy the old path stack.
                  pNewStack->push_back(*it);

             pNewStack->push_back(temp);    // add the new position to the path stack.

             if(temp.x==destPos.x && temp.y==destPos.y)
                  {
 				   retVal = pNewStack;	// find the path! ready to break all the loops.
				   break;				// break the inner loop. (the "for i" loop)
                  }

             stackQueue.push_back(pNewStack);


			 }

		  if(retVal)	// if a path is found, break the loop. (the "for j")
			  break;
		  
		}
        delete pStack;     // release the old information
		
      }

               // clear up the stackQueue and the stacks in the queue.
      while(!stackQueue.empty())
      {
         pStack = stackQueue.front();
         stackQueue.pop_front();
         while(!pStack->empty())
             pStack->pop_front();
         delete pStack;
      }


	  
	  for(j=0;j<HEIGHT;j++)		// free the space for the temporary map.
	  	delete[] record[j];
	  delete[] record;
	  
	  return retVal;

}
