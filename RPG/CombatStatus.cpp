#include "CombatStatus.h"

CombatStatus::CombatStatus()
{}


//+++ Return the hit points to reduce in an attack.
int CombatStatus::getAttackPower(CombatStatus& enemy)
{return ap* ap/(ap+enemy.dp)*2*level/(level+enemy.level)+1;}

bool CombatStatus::isAlive()
{return hp>0;}
	

//+++ Input: The point that is lost in this attack.
void CombatStatus::hurt(int point)
{ hp -= point;
  hp = hp>=0? hp : 0;
}

//+++ Return the rate to have a successful hit in an attack.
float CombatStatus::getAttackRate(CombatStatus& enemy)
{return (float)ar/(ar+enemy.dr) * 2 * level/ (level+enemy.level); }

int CombatStatus::attack(CombatStatus& enemy)
{
           // attack when no delay is set.
  if(attackDelay.isStopped() && defenceDelay.isStopped())
  {
     int rev = MISS;
     if(TakeChance((int)(100*getAttackRate(enemy)),100))   // take chance to see if hit.
     {
 	     enemy.hurt(getAttackPower(enemy));		// deduce health point
         enemy.defenceDelay.start();
         rev = HIT;
     }

      attackDelay.start();

      exp += (enemy.isAlive()?2:20) * (level + enemy.level) / level;
      if(exp>=mexp)
          levelUp();
      return rev;
  }
  else
  {                 // update delay if delay is set.
    if(!attackDelay.isStopped() && attackDelay.hasEnoughDelay())
         attackDelay.stop();
    if(!defenceDelay.isStopped()&&defenceDelay.hasEnoughDelay())
         defenceDelay.stop();
    return DELAY;
  }

}

void CombatStatus::levelUp()
{
  int l=0;
  while(exp>=mexp)
  {
    exp-=mexp;
    ++l;
	++level;
    mexp= (int)(mexp*1.1);
  }
  if(l<1)
	  return;

  // Update fighting status.
  float growthRate=1.1f;
  float multiplier=1;

  int i;
  for(i=0;i<l;i++)
        multiplier*=growthRate;
  cout <<  "\nMultiplier" <<multiplier;
  int* pInt=&hp;
  for(i=0;i<8;i++)
      *pInt++ = (int)((*pInt)*multiplier);

}


void CombatStatus::saveStatus(char* fileName)
{
  ofstream os(fileName, ios::out | ios::binary);
  os.write((char*)&hp,sizeof(int)*11);
  os.write((char*)&attackDelay.delay,sizeof(int));
  os.write((char*)&defenceDelay.delay,sizeof(int));
  os.close();
}

void CombatStatus::loadStatus(char* fileName)
{
   ifstream is(fileName, ios::in | ios::binary);
   is.read((char*)&hp,sizeof(int)*11);
   is.read((char*)&attackDelay.delay,sizeof(int));
   is.read((char*)&defenceDelay.delay,sizeof(int));
   is.close();
}

/////////////////////////////////////////////////////////////

		POINT screenPosition;	// where to draw the bar
		Tile frameTile;
		int barLength;			// length of the status bar.
		int barHeight;			// height of the bar.


//--- Input: a direct draw surface loaded with the status bar bitmap.
CombatStatusBar::CombatStatusBar()
{}

CombatStatusBar::CombatStatusBar(IDirectDrawSurface* pdds)
:barWidth(130),barHeight(20),textBarWidth(68)
{
	screenPosition.x=0;
	screenPosition.y=0;
	RECT rect={0,0,198,80};
	frameTile = Tile(pdds,rect);
}

void CombatStatusBar::setScreenPosition(int x,int y)
{screenPosition.x=x; screenPosition.y=y;}

POINT CombatStatusBar::getScreenPosition() {return screenPosition;}

void CombatStatusBar::setBarSize(int w, int h)
{barWidth=w; barHeight=h;}

void CombatStatusBar::setTextBarWidth(int w){textBarWidth=w;}

void CombatStatusBar::setFrameTile(Tile t) {frameTile=t;}

void CombatStatusBar::drawOn(DDrawPackage* ddraw, CombatStatus* status)
{
	frameTile.draw(ddraw->getTheBoard(),screenPosition.x,screenPosition.y);
	
	if(status==0)		// don't try to draw status when no status is input
		return;

/*
	
	int x=textBarWidth+barWidth/2;	// x,y defines the position to draw to bar
	int y=barHeight*3+2;
	int sx=frameTile.getSize().x; // sx is the x position of clip to get the status bar


	char string[10]="";
	sprintf(string,"%d",status->level);
	DWORD color = RGB(255,255,0);
	HDC boardDC;
	ddraw->getTheBoard()->GetDC(&boardDC);
	SetBkMode(boardDC,TRANSPARENT);
	SetTextColor(boardDC,color);
	TextOut(boardDC,x,y,string,strlen(string));
	ddraw->getTheBoard()->ReleaseDC(&boardDC);
*/
	int x=textBarWidth;	// x,y defines the position to draw to bar
	int y=0;
	int sx=frameTile.getSize().x; // sx is the x position of clip to get the status bar

		// draw HP
	RECT rect= {sx,0,sx+barWidth*status->hp/status->mhp,barHeight};
	Tile bar(frameTile.getSource(),rect);
	bar.draw(ddraw->getTheBoard(),x,y);

		// draw MP
	rect.top+=barHeight;
	rect.bottom+=barHeight;
	rect.right=sx+barWidth*status->mp/status->mmp;
	y+=barHeight;
	bar.setClip(rect);
	bar.draw(ddraw->getTheBoard(),x,y);

		// draw exp
	rect.top+=barHeight;
	rect.bottom+=barHeight;
	rect.right=sx+barWidth*status->exp/status->mexp;
	y+=barHeight;
	bar.setClip(rect);
	bar.draw(ddraw->getTheBoard(),x,y);


	// draw experience
	rect.top+=barHeight;
	rect.bottom+=barHeight;
	rect.right=sx+barWidth*(status->level>99?100:status->level)/100;
	y+=barHeight;
	bar.setClip(rect);
	bar.draw(ddraw->getTheBoard(),x,y);

}