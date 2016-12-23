#include "DIB.h"


DIB::DIB(int x,int y)
{
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	bmpInfo.bmiHeader.biHeight = y;
	bmpInfo.bmiHeader.biWidth  = x;
	bmpInfo.bmiHeader.biBitCount = 24;
	bmpInfo.bmiHeader.biClrUsed = 0;
	bmpInfo.bmiHeader.biPlanes = 1;

	dimension.x=x;
	dimension.y=y;

	hbmDIB = CreateDIBSection(hdc, &bmpInfo, DIB_RGB_COLORS, (void**)&pDIB,0,0);
	hdc = CreateCompatibleDC(NULL);
	old = (HBITMAP) SelectObject(hdc, hbmDIB);

}

DIB::~DIB()
{
	SelectObject(hdc,old);
	DeleteObject(hbmDIB);
	DeleteDC(hdc);
}



void DIB::drawOnto(IDirectDrawSurface* target, int tx, int ty)
{
	HDC boardDC;
	target->GetDC(&boardDC);
	//BitBlt(boardDC,tx,ty,tx+dimension.x,ty+dimension.y,hdc,0,0,SRCCOPY);
	SetDIBitsToDevice(boardDC,tx,ty,dimension.x,dimension.y,0,0,0,dimension.y,pDIB,&bmpInfo,0);
	target->ReleaseDC(boardDC);
}

void DIB::copyFrom(IDirectDrawSurface* source, int sx, int sy)
{
	HDC boardDC;
	source->GetDC(&boardDC);
	BitBlt(hdc,0,0,dimension.x,dimension.y,boardDC,sx,sy,SRCCOPY);
	source->ReleaseDC(boardDC);
}

//+++ erase with black color.
void DIB::erase()
{
	RECT rect={0,0,dimension.x, dimension.y};
	FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
}