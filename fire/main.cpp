#include <windows.h>
#include <stdio.h>
#include "commonwin.h"
#include "commonDX.h"


#include "common.h"


CApplication	capp;
CWindow			cwin;
DDrawPackage	ddraw;	


DelayTimer delayTimer(50);

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
		    break;

		case WM_SETCURSOR:	 //ÉèÖÃ¹â±ê
			SetCursor(NULL);
			return true;
		break;

		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}



void Render()
{

	while(!delayTimer.hasEnoughDelay())
		;
	//------ Erase the board ------
	HDC boardDC;
	ddraw.getTheBoard()->GetDC(&boardDC);
	RECT rect = {0,0,640,480};
	FillRect(boardDC, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
	ddraw.getTheBoard()->ReleaseDC(boardDC);


//------------ Getting surface array begins ------------

	static const int W=160,H=50;
	static unsigned short fireBoard[W*H]={0};


	//--- Place Coal ---

	for(int i=0;i<W;i++)
		fireBoard[W*H-W+i]=0x0;

	for(i= rand()%23;i<W;i+= rand()%23)
		fireBoard[W*H-W+i]= 0xFFFF;


	for(i=0;i<5;i++)
		if(rand()%2)fireBoard[(rand()%15+25)*W+rand()%(W-2)+1]= 0xEFFF;


	//--- Average heat ---
	for(i=W;i<W*H;i++)
	{
		if(i%W==0 || i%W==W-1)
			continue;
		int firevalue= (
						  fireBoard[i-1]
						+ fireBoard[i+1]
						+ fireBoard[i+W-1]
						+ fireBoard[i+W+1]
						+ fireBoard[i+W]
						+ fireBoard[i-W-1]
						+ fireBoard[i-W+1]
						+ fireBoard[i-W])/8;
		
		fireBoard[i] = firevalue;    // Plot that new color

		static const int HEATLOST=50;
		if(fireBoard[i]>HEATLOST && i/W<37)
			fireBoard[i]-=HEATLOST;
		//else if(fireBoard[i] && i/W<=35	&& i/W>15)
		//	fireBoard[i]+=5;
		//else if(fireBoard[i]>HEATLOST && i/W<=15)
		//	fireBoard[i]-=HEATLOST;
	}

		

	//--- output heat to the screen ---
	
	DDSURFACEDESC ddsd;
	HRESULT ret = DDERR_WASSTILLDRAWING;;
	ddsd.dwSize = sizeof(ddsd);

	while (ret == DDERR_WASSTILLDRAWING)
		ret = ddraw.getTheBoard()->Lock(NULL, &ddsd, 0, NULL);

	unsigned short* surfaceArray= (unsigned short *)ddsd.lpSurface;

/*
	for(int j=0;j<H;j++)
		for(int k=0;k<W;k++)
			surfaceArray[j*640+k] = fireBoard[j*W+k];
*/
	for(int j=0;j<H;j++)
		for(int k=0;k<W;k++)
			//surfaceArray[j*640+k] = (fireBoard[j*W+k]>>8)<<12;
		{
			int firevalue =  (fireBoard[j*W+k]>>8)<<12;
			if(fireBoard[j*W+k]>>12>0)
				firevalue |= 0x0F00;

			surfaceArray[j*5*640+k*4] = firevalue;
			surfaceArray[j*5*640+k*4+1] = firevalue;
			surfaceArray[j*5*640+k*4+2] = firevalue;
			surfaceArray[j*5*640+k*4+3] = firevalue;

			surfaceArray[(j*5+1)*640+k*4] = firevalue;
			surfaceArray[(j*5+1)*640+k*4+1] = firevalue;
			surfaceArray[(j*5+1)*640+k*4+2] = firevalue;
			surfaceArray[(j*5+1)*640+k*4+3] = firevalue;

			surfaceArray[(j*5+2)*640+k*4] = firevalue;
			surfaceArray[(j*5+2)*640+k*4+1] = firevalue;
			surfaceArray[(j*5+2)*640+k*4+2] = firevalue;
			surfaceArray[(j*5+2)*640+k*4+3] = firevalue;

			surfaceArray[(j*5+3)*640+k*4] = firevalue;
			surfaceArray[(j*5+3)*640+k*4+1] = firevalue;
			surfaceArray[(j*5+3)*640+k*4+2] = firevalue;
			surfaceArray[(j*5+3)*640+k*4+3] = firevalue;

			surfaceArray[(j*5+4)*640+k*4] = firevalue;
			surfaceArray[(j*5+4)*640+k*4+1] = firevalue;
			surfaceArray[(j*5+4)*640+k*4+2] = firevalue;
			surfaceArray[(j*5+4)*640+k*4+3] = firevalue;

		}



    ddraw.getTheBoard()->Unlock(NULL);

//------------ Getting surface array ends ------------


	ddraw.flip();  

//screenControl.cameraOff();
}




int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPreInstance,
				   LPSTR cmdLine, int cmdShow)
{

	char* appName = "RPG";
	const int width  = 200;
	const int height = 200;
	
	capp.Set(hThisInstance, cmdLine, cmdShow);
	if(!cwin.Prepare(appName, width, height, hThisInstance))
		return 0;


	ddraw.init(cwin.hwnd, 640, 480, 16);

	delayTimer.start();

	cwin.Show(cmdShow);


	MSG temp = cwin.Run();



	return temp.wParam;
	

}
