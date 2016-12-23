#include "TextDrawer.h"

//+++ Input:     Desired direct draw surface to draw on, the string for output,
//+++            pixel position for the string to be drawn.
//+++ Output:    Draw the string on (x,y) pixel position on the ddraw surface.
void TextDrawer::drawLine(IDirectDrawSurface* board, char* string, int x, int y)
{

	int i=0;
	while(string[i]!=0)
	{
		getTile(string[i++]).draw(board,x,y);
		x+=font.x;
		
	}

}


//+++ return the related tile that contain a character
//+++ corresponding to the input character.
//+++ Index:
//+++			0		blank space
//+++			1-10	numbers
//+++			11-36	capital letter
//+++			37-62	lower case letter
//+++			63		period
//+++			64		comma
//+++			65		question mark
//+++			66		exclaimer
Tile& TextDrawer::getTile(char c)
{
	if(c>='0' && c<= '9')
		return tile[c-'0'+1];
	else if(c>='A' && c<= 'Z')
		return tile[c-'A'+11];
	else if(c>='a' && c<= 'z')
		return tile[c-'a'+37];
	else if(c=='.')
		return tile[63];
	else if(c==',')
		return tile[64];
	else if(c=='?')
		return tile[65];
	else if(c=='!')
		return tile[66];
	else 					// for blank space
		return tile[0];

}


//+++ read font information from a file.
//+++ Input:    a ddraw surface that has a bitmap for the characters loaded.
//+++           A binary file that contains font info for the bitmap.
//+++ File format:  The first two int value (POINT) will be the dimension of
//+++               a character. The following 10 POINT values will be the
//+++               upper-left corner position of the clip rectangles that are
//+++               used to get the number images from the source bitmap.
//+++               The next 26 for upper-case alphabetic letters. Then, 26
//+++               POINT values for lower-case. At last, period, comma,
//+++			    question mark, exclaimer.
void TextDrawer::loadFont(IDirectDrawSurface* source, char* fileName)
{
	
	ifstream is(fileName, ios::binary);
	
	is.read((char*)&font, sizeof(POINT));
	for(int i=1;i<MAX;i++)
	{
		POINT buff;
		is.read((char*)&buff, sizeof(POINT));
		RECT rect = {buff.x, buff.y, buff.x+font.x, buff.y+font.y};
		tile[i] = Tile(source, rect);
	}

	is.close();

	RECT rect={0,0,0,0};	// blank space
	tile[0] = Tile(source,rect);
}
