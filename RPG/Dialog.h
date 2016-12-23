#ifndef _RPG_DIALOGBOX_
#define _RPG_DIALOGBOX_

#include <windows.h>
#include "commonDX.h"

class RPG_DialogBox
{
 private:
         int x,y,w,h;            // position to draw the dialog box
                                 // dimension of the dialog box
	 
	     DDrawPackage* ddraw;	
         DWORD textColor;
         RECT getTextRECT();
 public:
         static const int BOUNDARY;
         static const int ROWLENGTH;
         static const int MAXROW;
         static const int MAXLENGTH;
         static const int TEXT_BRUSH;
		 static const int ROWHEIGHT;

    	 enum{ DB_USEPOINT=1};
	 
		 RPG_DialogBox();

		 void setDDrawPackage(DDrawPackage*);
         void setTextColor(DWORD);
         void displayOn(HDC hdc, char* string,int talkx, int talky,int status);
		 void display(char* string,int talkx, int talky,int status);
         void setTalkingPosition(int x,int y);
};


#endif