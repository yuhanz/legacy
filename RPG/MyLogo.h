#ifndef _AIIIIIIIII_LOGO_
#define _AIIIIIIIII_LOGO_

#include <windows.h>
#include "CommonDX.h"
#include "common.h"

class AIsLogo
{
	private:
		int gap;	// distance between letters
		int x,y;	// position of the head.
        DWORD textColor;
		char ai[11];
	
	public:
		AIsLogo();
		void setPosition(int,int);
		void setGap(int);
		void setTextColor(DWORD);
		void setText(char*);
		DWORD getTextColor();
		POINT getPosition();
		char* getText();
		int getGap();

				// Display the worm on the screen until it hits a.
				// horizontal position "endx". "time" is the time
				// gap between each frame, determing how rapid
				// the worm moves
		void displayOn(DDrawPackage*,DInputPackage*,int time,int endx);
};
   		
#endif