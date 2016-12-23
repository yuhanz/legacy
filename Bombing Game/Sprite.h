//////////////////////////////////////////////////////////////
//	Programmer:		Yuhan Zhang
//	EMail:			Yuhan_Zhang@netease.com
//	Date:			July 1, 2003
//	project:		Classes that are associated with
//					displaying sprites.(using DirectX8)
//////////////////////////////////////////////////////////////

//--	To use this header file, <time.h> and "common.h"
//-- must be included. 

/*---------------------------------------------------------
 * Class Label
 *		Given a directdraw surface with a bitmap loaded,
 * and a rectanglar dimension to frame a picture from it,
 * this object will be able to draw itself on the given position.
 * 
 *  
 *---------------------------------------------------------	
 */
class Label
{
	public:

		IDirectDrawSurface* source;	// where the bitmap is stored.
		RECT frame;					// where to getthe image.
		POINT whereToDraw;			// position of upper-left corner
		bool transparent;			// whether color key's used.

		
		Label(IDirectDrawSurface* s,RECT r) : source(s), frame(r) {}
		HRESULT draw(LPDIRECTDRAWSURFACE theBoard);		// draw everything
		
};





/*---------------------------------------------------------
 * Class CSprite:
 *		contains a directdraw surface and knows where to
 * draw the sprite.
 *
 * Protected data:
 *		POINT state
 * (This is an abstract class, which must be extended before
 *   using by specify the virtual funcitons 'void getNextFrame')
 *---------------------------------------------------------	
 */


class CSprite
{

	private:
		IDirectDrawSurface* source;	// DDsurface that contains a bitmap
		POINT frameSize;			// size of a frame
		POINT whereToDraw;			// screen position to draw the sprite.
	

			
	public:
		DelayTimer theTimer;				 
		POINT state;				// state of the sprite
		
		CSprite(IDirectDrawSurface*, int, int);
		virtual ~CSprite();
		HRESULT draw(LPDIRECTDRAWSURFACE theBoard);
		void setDrawPosition(int x, int y);
		POINT getDrawPosition();
		virtual bool getNextFrame(int wayOfChange)=0;
		POINT getFrameSize();
		IDirectDrawSurface* getSurface();
};





/*---------------------------------------------------------
 * Class CS_Walking:
 *	  Derived from 'CSprite'.
 * 
 * The source picture should be a bitmap with 3 X 4 frames:
 *    column 0: staying state   (state.x==0)
 *	  column 1: walking state1	(state.x==1)
 *	  column 2: walking state2	(state.x==2)
 *    row 0:    down			(state.y==0)
 *	  row 1:	left			(state.y==1)
 *    row 2:    up				(state.y==2)
 *	  row 3:	right			(state.y==3)
 *
 *		(lastTime = -1 means the sprite is in staying state.)
 *---------------------------------------------------------	
 */


class CS_Walking : public CSprite
{
	private:
		
		bool walkingState;	// ture means walking state1 (state.x==1)
	public:
		enum {DOWN, LEFT, UP, RIGHT, STOP};

		CS_Walking(IDirectDrawSurface*,int,int);
		virtual ~CS_Walking() {};
		bool getNextFrame(int);

};



/*---------------------------------------------------------
 * Class CS_Series:
 *	  Derived from 'CSprite'.
 *    This picture source must contain a series of frames
 *	lining up vertically or horizontally. Each call of
 *	'getNextFrame()' will bring the frame under it or on 
 *  its right. After having reached the last frame,
 *	the function 'getNextFrame()' will not update frame
 *  any longer.
 *  
 *---------------------------------------------------------	
 */
class CS_Series : public CSprite
{
	private:
		bool isHorizontal;	// way of getting next frame (horizontally or vertically)
		int maxState;		// the index of the last state. Note: this is not the max number of frames.
	public:
		enum {VERTICAL, HORIZONTAL};		
		CS_Series(IDirectDrawSurface*,int,int,int,int,bool,int);
		virtual ~CS_Series() {};
		virtual bool getNextFrame(int);
		void setMaxState(int);
		bool isMaxState();
		
};

/*---------------------------------------------------------
 * Class CS_Cycle:
 *	  Derived from 'CS_Series'.
 *	  It's basicly a 'CS_Series', except once the state
 *	reaches the last frame, it's set back to the first
 *  one again.
 *---------------------------------------------------------	
 */
class CS_Cycle : public CS_Series
{
	private:
		POINT startingState;
	
	public:
		CS_Cycle(IDirectDrawSurface*,int,int,int,int,bool,int);
		virtual ~CS_Cycle() {};
		bool getNextFrame(int);

};


////////////////////////////////////////////////////////////////



/*==============================================================
== IMPLEMENTATION SECTION
== (The portion from here on can be seperated into another file)
==============================================================*/


HRESULT Label::draw(LPDIRECTDRAWSURFACE theBoard)
{
	return theBoard->BltFast(whereToDraw.x,
							 whereToDraw.y,
							 source, &frame, (transparent? DDBLTFAST_SRCCOLORKEY : DDBLTFAST_NOCOLORKEY) | DDBLTFAST_WAIT);
}


		


//----------------------------------------
//------ members of 'CSprite' ------------
//----------------------------------------




CSprite::CSprite(IDirectDrawSurface* s,
				 int fWidth,
				 int fHeight
				 ) : source(s)
{
	frameSize.x = fWidth;
	frameSize.y = fHeight;
	
	theTimer.timeDelay = 200;	// default time delay.
}


CSprite::~CSprite()
{}


//+++ Draw the frame onto the given surface, namely, 'theBoard'
//+++ Return an HRESULT of the drawing.
HRESULT CSprite::draw(LPDIRECTDRAWSURFACE theBoard)
{
	RECT rcRect;
					// upper-left corner of the clip
	POINT cul = {state.x * frameSize.x, state.y * frameSize.y};
					//lower-right corner of the clip
	POINT clr = {cul.x + frameSize.x, cul.y + frameSize.y };
	
	
	SetRect(&rcRect, cul.x, cul.y, clr.x, clr.y);

	HRESULT ddval =  theBoard->BltFast(whereToDraw.x,
							 whereToDraw.y,
							 source, &rcRect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
	
	
			// try to fix the out-of-bound problem by a clipper.
	if(ddval != DD_OK)
	{
				// upper-left corner of the draw position.
		POINT dul = whereToDraw;
				// lower-right corner of the draw position.
		POINT dlr = {dul.x + frameSize.x, dul.y + frameSize.y};

		
				// return if the image is completely out of screen.
		if(    dul.x<0 && dlr.x<0 || dul.x > 640 && dlr.x >640
		    || dul.y<0 && dlr.y<0 || dul.y > 480 && dul.y >480)
			return ddval;

		
		if(dul.x < 0)
		{
			cul.x -= dul.x;
			dul.x = 0;
		}

		if(dul.y <0)
		{
			cul.y -= dul.y;
			dul.y = 0;
		}

		if(dlr.x > 640)
		{
			clr.x -= dlr.x - 640;
		}

		if(dlr.y > 480)
		{
			clr.y -= dlr.y - 480;
		}
		
		
		SetRect(&rcRect, cul.x, cul.y, clr.x, clr.y);

		ddval =   theBoard->BltFast(dul.x, dul.y, source,  &rcRect,
							DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

	}

	return ddval;

}

//+++ set where to draw +++
void CSprite::setDrawPosition(int x, int y)
{
 whereToDraw.x=x;
 whereToDraw.y=y;
}
	 

POINT CSprite::getDrawPosition()
{ return whereToDraw; }


//+++ Get the size of an frame +++
POINT CSprite::getFrameSize()
{ return frameSize; }

IDirectDrawSurface* CSprite::getSurface()
{ return source; }





//=======================================================
//========== IMPLEMENTATION SECTION =====================
//=======================================================




//---------------- members of 'CS_Walking'-----------------





CS_Walking::CS_Walking(IDirectDrawSurface* s, int w, int h)
			: CSprite(s, w, h)
{
	state.x=0;
	state.y=0;
}

		
//+++ Set the state for the next frame based on
//+++ the way of walking and the time delay.
//+++ Return true if any modification is made.
//+++ Return false if no state is altered.
bool CS_Walking::getNextFrame(int wayOfWalking)
{
		
	switch(wayOfWalking)
	{
		case STOP:
			state.x=0;
			if(theTimer.isStopped()) // return false when trying to stop from stopping state
				return false;
			else 
			{
				theTimer.stop();	// stop the timer.
			}
			MessageBox(NULL, "STOP processed", "ok" , MB_OK);
			break;
		case UP:
		case DOWN:
		case LEFT:
		case RIGHT:
			state.y = wayOfWalking;
			if(theTimer.isStopped())	// starts to walk from staying
			{
				theTimer.start();
			}
			else
			{
				if(!theTimer.hasEnoughDelay())
					return false;	// refuse to  update without enough delay
				else
				{
					if(state.x!=0)
						state.x=0;
					else
					{
						if(walkingState)
							state.x=1;
						else
							state.x=2;
						
						walkingState = !walkingState;
					}
				}
			}
			break;
		default:
			MessageBox(NULL, "Error in getNextFrame", "Error" , MB_OK);
		
	}
	return true;
	
}





//-------------------- members of 'CS_Series' --------------------





//+++ sx = starting state.x
//+++ sy = starting state.y
//+++ ih = horizontal method or not
//+++ nf = number of frames
CS_Series::CS_Series(IDirectDrawSurface* s, int w, int h, int sx, int sy, bool ih, int nf)
			: CSprite(s, w, h)
{
	isHorizontal = ih;
	state.x = sx;
	state.y = sy;
	
	if(isHorizontal)
		maxState = state.x + nf - 1;
	else
		maxState = state.y + nf - 1;
}	

//+++ When the time delay is reached, change the state to the 
//+++ nearby cell by increasing state.x or state.y, depending
//+++ on the flag 'isHorizontal'.
//+++ Return whether the state is updated or not.
bool CS_Series::getNextFrame(int trivail)
{
	if(theTimer.isStopped())
	{
		theTimer.start();
		return false;
	}
	
	long currentTime = clock();
	if(!theTimer.hasEnoughDelay())
		return false;

	if(isHorizontal)
	{
		if(state.x >= maxState)
			return false;
		else
			++state.x;
	}
	else
	{
		if(state.y >= maxState)
			return false;
		else
			++state.y;
	}

	return true;
}
		
//+++ return if the sprite has reached the max state.
bool CS_Series::isMaxState()
{ return  isHorizontal ? state.x>= maxState : state.y>=maxState; }

void CS_Series::setMaxState(int m)
{ maxState = m; }





//---------------- members of 'CS_Cycle' ---------------------





CS_Cycle::CS_Cycle(IDirectDrawSurface* s,int w,int h,int sx,int sy,bool ih,int nf)
		: CS_Series(s,w,h,sx,sy,ih,nf+1)
{
	startingState.x=sx;
	startingState.y=sy;
}


//+++ Do the series sprite. Set the frame back to starting
//+++ position when it's done a cycle.
bool CS_Cycle::getNextFrame(int trivial)
{ 
	bool rval = CS_Series::getNextFrame(NULL);
	
	if(isMaxState())
	{
		state= startingState;
		return true;
	}
	else
		return rval;


}




