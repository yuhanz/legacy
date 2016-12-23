//////////////////////////////////////////////////////////////
//	Programmer:		Yuhan Zhang
//	EMail:			Yuhan_Zhang@netease.com
//	Date:			July 3, 2003
//	description:		This object organizes the main menu
//					of the bombing game, which guides player
//					to the game, help, and exit.
//////////////////////////////////////////////////////////////

extern Menu mainMenu;
extern DInputPackage dinput;
extern DDrawPackage ddraw;


//+++ This funciton is used as the idle funciton in the windows
//+++ procedure when menu is used.
void menuProcedure()
{

}





/*--------------------------------------------------------
-- class Menu:
--
--		This class will use globle varibles:
--			DInputPackage dinput;
--			DDrawPackage ddraw;
--
--------------------------------------------------------*/
class Menu
{
	private:
		IDirectDrawSurface* menu;	// where the curros image and menu image are stored.
		POINT mul, mlr;				// upper-left and lower-right corner of the menu on the bitmap;
		POINT cul, clr;				// upper-left and lower-right corner of the cursor on the bitmap;
		StageControl* theGame;
		int cursorPosition;			// indicate cursor postion, such as pointing at start, or exit.
		enum {START, HELP, EXIT};

	public:
		void init(IDirectDrawSurface*,StageControl*);
		void processInput();		// used to set curso position. Then redraw it on the screen.
		void layMenu();				// output the menu on the screen
		void printHelp();			// print help info
		void drawCursor();			// draw cursor at proper position.
		void startGame();			// start a new game, set idle function to 'render'.
		void resetMenu();			// set menu to proper state and draw it.
		

};


//+++ connect to the other objects
void Menu::init(IDirectDrawSurface* d,StageControl* sc)
{
	pdds = d;
	theGame = sc;
}




void Menu::processInput()
{
	if(dinput->updateKey())
	{
		if(dinput->checkKey(DIK_UP) && cursorPosition > START)
			--cursorPosition;
		else if(dinput->checkKey(DIK_DOWN) && cursorPosition < EXIT)
			++cursorPosition;
		
		drawCursor();
	}
}

void Menu::layMenu()
{

	//SetRect(&rcRect,0,0,640,480);
	//ddrval = ddraw.getTheBoard()->BltFast(0,0, theScene, &rcRect, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
}

void Menu::printHelp()
{}

void Menu::drawCursor()
{}

void Menu::startGame()
{}


void Menu::resetMenu()
{
	cursorPosition=START;
	layMenu();
	drawCursor();
}

