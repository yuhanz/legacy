#include "Dialog.h"

const int RPG_DialogBox::BOUNDARY=10;
const int RPG_DialogBox::ROWLENGTH=68;
const int RPG_DialogBox::MAXROW=7;
const int RPG_DialogBox::MAXLENGTH=ROWLENGTH*MAXROW;
const int RPG_DialogBox::TEXT_BRUSH = BLACK_BRUSH;
const int RPG_DialogBox::ROWHEIGHT=15;

RPG_DialogBox::RPG_DialogBox() : x(0),y(0),w(640), h(100), ddraw(0)
{ textColor=RGB(20,180,20);}


void RPG_DialogBox::displayOn(HDC hdc, char* string,int talkx, int talky, int status)
{
   RECT rect=getTextRECT();           // erase the board
   FillRect(hdc, &rect, (HBRUSH)GetStockObject(TEXT_BRUSH));

   // initiate text position.
   int tx = rect.top+BOUNDARY;
   int ty = rect.left+BOUNDARY;


   // draw text
   SetBkMode(hdc,TRANSPARENT);
   SetTextColor(hdc,textColor);

   int i=0, max=ROWLENGTH;
   char c;
   while((c=string[i])!=0)
   {
	if(c=='\n')
	{
		string[i]=0;
		TextOut(hdc,tx,ty,string,strlen(string));
		string+=i+1;
		i=0;
		ty+=ROWHEIGHT;
	}


	if(i==max)
	{
		string[i]=0;
		TextOut(hdc,tx,ty,string,strlen(string));
		string+=max;
		i=0;
		string[0]=c;
		ty+=ROWHEIGHT;
	}

	++i;
   }

   TextOut(hdc,tx,ty,string,strlen(string));


   if(status & DB_USEPOINT)
   {
	   // draw the talking way.
		HPEN old= (HPEN)SelectObject(hdc,GetStockObject(BLACK_PEN));
		int _x=x+w/2-BOUNDARY, _y=y+h-1;
		for(;_x<x+w/2+BOUNDARY ; _x++)
		{
			MoveToEx(hdc,talkx,talky,0);
			LineTo(hdc,_x,_y);
		}


		SelectObject(hdc,GetStockObject(WHITE_PEN));
		MoveToEx(hdc,_x,_y+1,0);
		LineTo(hdc, rect.right-1, rect.bottom);
		LineTo(hdc, rect.right-1, rect.top);
		LineTo(hdc, rect.left, rect.top);
		LineTo(hdc, rect.left, rect.bottom);
		LineTo(hdc, x+w/2-BOUNDARY, y+h);
		LineTo(hdc, talkx, talky);
		LineTo(hdc, x+w/2+BOUNDARY, y+h);

		SelectObject(hdc,old);
   }
	else
	{
		int _x=rect.left,_y=rect.top;
		SelectObject(hdc,GetStockObject(WHITE_PEN));
		MoveToEx(hdc,_x,_y,0);
		LineTo(hdc, rect.left, rect.bottom);
		LineTo(hdc, rect.right-1, rect.bottom);
		LineTo(hdc, rect.right-1, rect.top);
		LineTo(hdc, rect.left, rect.top);
	}
   
}

RECT RPG_DialogBox::getTextRECT()
{RECT rect = {x,y, x+w, y+h};
 return rect;}

void RPG_DialogBox::setDDrawPackage(DDrawPackage* d)
{ddraw=d;}

void RPG_DialogBox::display(char* string,int talkx, int talky, int status)
{
	if(ddraw)
	{
		HDC boardDC;
		ddraw->getFrontBoard()->GetDC(&boardDC);
		displayOn(boardDC,string ,talkx, talky,status);
		ddraw->getFrontBoard()->ReleaseDC(boardDC);
	}
}
