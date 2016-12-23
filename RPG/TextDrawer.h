#ifndef _TEXT_DRAWER_
#define _TEXT_DRAWER_

#include "Tile.h"
#include <windows.h>
#include <ddraw.h>
#include <fstream.h>

class TextDrawer
{
	private:
	public:	
		enum {MAX=67};
		POINT font;		// in pixel value
		Tile tile[MAX];

	public:

		TextDrawer() {}
		void drawLine(IDirectDrawSurface* board, char* string, int x, int y);
		void loadFont(IDirectDrawSurface* source, char* fileName);
		Tile& getTile(char);

};



#endif