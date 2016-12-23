#include "MyLogo.h"


AIsLogo::AIsLogo():x(340),y(240),gap(8),textColor(RGB(20,180,20))
{strcpy(ai,"aiiiiiiiii");}

void AIsLogo::setPosition(int _x,int _y)
{
	x=_x;
	y=_y;
}

void AIsLogo::setGap(int g) {gap=g;}
void AIsLogo::setTextColor(DWORD dw) {textColor=dw;}
void AIsLogo::setText(char* s){strcpy(ai,s);}
DWORD AIsLogo::getTextColor() {return textColor;}

POINT AIsLogo::getPosition()
{
	POINT p={x,y};
	return p;
}

char* AIsLogo::getText() {return ai;}
int AIsLogo::getGap() {return gap;}

void AIsLogo::displayOn(DDrawPackage* ddraw, DInputPackage* dinput,int time,int endx)
{
	char string[]="Jan 2004. Yong Ding Gong Zuo Shi";
	int partx[10];		// x position for each body part
	DelayTimer theTimer(time);
	
	int i;
	for(i=0;i<10;i++)
		partx[i]=x+i*gap;
	
	DWORD bkColor=RGB(0,0,0);

	--i;	// from here i=9	
	theTimer.start();

	HDC boardDC;
	ddraw->getFrontBoard()->GetDC(&boardDC);
	RECT rect = {0,0,640,480};
	FillRect(boardDC, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
	SetBkMode(boardDC,TRANSPARENT);
	SetTextColor(boardDC,textColor);
	TextOut(boardDC,300,400,string,strlen(string));

	while(x>endx)		// When x is not reach the destination
	{
		if(partx[i]-x > gap*(i-1))	// if the body part has not reached out enough distance
		{
			partx[i]-=5;		// Move left this part.
							
					//==Paint the worm on the screen;
			SetTextColor(boardDC,textColor);
			int j;
			for(j=0;j<10;j++)
				TextOut(boardDC,partx[j],y,ai+j,1);
			
			while(!theTimer.hasEnoughDelay())
			{
				//==check key for exist while in delay
				dinput->updateKey();
				if( dinput->checkKey(DIK_ESCAPE) ||
					dinput->checkKey(DIK_RETURN) ||
					dinput->checkKey(DIK_SPACE))
					{
						ddraw->getFrontBoard()->ReleaseDC(boardDC);
						return;
					}
			}
			SetTextColor(boardDC,bkColor);	//erase the worm
			for(j=0;j<10;j++)
				TextOut(boardDC,partx[j],y,ai+j,1);
		
			
		}
		else	// If the body part has reached out enough distance
		{
			--i;
			if(i==-1)
			{
				x=partx[0];
				i=9;
			}
		}
	}

	ddraw->getFrontBoard()->ReleaseDC(boardDC);
	return;
}
