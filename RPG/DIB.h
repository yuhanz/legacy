#ifndef _DIB_
#define _DIB_

#include <windows.h>
#include <ddraw.h>

class DIB
{
	private:
		BITMAPINFO bmpInfo;
		HDC hdc;
		HBITMAP hbmDIB;
		HBITMAP old;
		POINT dimension;
		unsigned char* pDIB;
	public:

		DIB(int x,int y);
		~DIB();
		unsigned char* getPointer() {return pDIB;}
		POINT getDimension() {return dimension;}
		HDC getDC() {return hdc;}

	    void drawOnto(IDirectDrawSurface* target, int tx, int ty);
		void copyFrom(IDirectDrawSurface* source, int sx, int sy);
		void erase();
};

#endif
