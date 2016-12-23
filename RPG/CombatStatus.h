#ifndef _RPG_COMBAT_STATUS_
#define _RPG_COMBAT_STATUS_

#include <fstream.h>
#include "common.h"
#include "Tile.h"
#include <windows.h>
#include <stdio.h>
#include "CommonDX.h"

class CombatStatus
{
  public:
      int hp; // Hit points
      int mhp; // Max HP
      int mp; // Magic points
      int mmp; // Max MP
      int ap; // Attacking points
      int dp; // Defencing points
      int ar; // attacking rate
      int dr; // defencing rate
      int exp; // experience
      int mexp;  // max experience. (To have next level)
      int level;
      DelayCounter attackDelay;
      DelayCounter defenceDelay;
      enum {DELAY,MISS,HIT};         // for the result of attack.

      CombatStatus();
      bool isAlive();
      bool enoughDelay();
      int getAttackPower(CombatStatus&);
      void hurt(int);           // get attacked with certainty
      float getAttackRate(CombatStatus&);
      int attack(CombatStatus&); // Attack enemy by chance.
      void levelUp(); // Change the BasicUnit status
      void saveStatus(char*);
      void loadStatus(char*);
};


//////////////////////////////////////////////////////////

class CombatStatusBar
{
	private:
		POINT screenPosition;	// where to draw the bar
		Tile frameTile;
		int barWidth;			// width of the status bar.
		int barHeight;			// height of the status bar.
		int textBarWidth;		// width of the text bar.
		DDrawPackage* ddraw;

	public:
		
		CombatStatusBar();
		CombatStatusBar(IDirectDrawSurface*);
		void setScreenPosition(int,int);
		POINT getScreenPosition();
		void setBarSize(int w, int h);
		void setTextBarWidth(int w);
		void setFrameTile(Tile);
		void drawOn(DDrawPackage*,CombatStatus*);


};

#endif
