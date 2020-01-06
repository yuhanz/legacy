////////////////////////////////////////////////////////////////
//	Author:		Yuhan Zhang
//	Date:		11/9/08
//	Purpose:	CS 599 Bioinformatics, HW2
//				Alignment programming:
//				- Hamming Distance, Levenschtein Distance,
//				- DOTPLOT
//				- NW algorithm
//				- FASTA algorthm
////////////////////////////////////////////////////////////////


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <sdl.h>
#include <windows.h>
#include "resource.h"

#include "Alignment.h"


// Do a DOTPLOT into an HTML file
void HTMLDotPlotToFile(const char* str1, const char* str2, FILE* fp) {
	int len1 = strlen(str1);
	int len2 = strlen(str2);

	bool* dotPlot = new bool[len1*len2];
	for(int i=0;i<len1;i++) {
		for(int j=0;j<len2;j++){
			dotPlot[i*len2+j] = str1[i]==str2[j];
		}
	}


	fprintf(fp,"\n<table style='background-color:#f0f0f0'>\n<tr><td></td>");
	for(int j=0;j<len2;j++){
		fprintf(fp,"<td style='background-color:#00ff00'><b>%c</b></td>", str2[j]);
	}
	fprintf(fp,"</tr>");
	

	for(int i=0;i<len1;i++) {
		fprintf(fp,"\n<tr><td style='background-color:#00ff00'><b>%c</b></td>", str1[i]);
		for(int j=0;j<len2;j++){
			bool dot = dotPlot[i*len2+j];
			int x, y;
			bool colorCoded = dot && ((x=j-1)>=0 && (y=i-1)>=0 && dotPlot[y*len2+x] ||(x=j+1)<len2 && (y=i+1)<len1 && dotPlot[y*len2+x]);
			fprintf(fp,"<td%s>%s</td>", colorCoded?" style='background-color:#ff0000'":"", dot?"x":"");
		}
		fprintf(fp,"</tr>");
	}

	fprintf(fp,"\n</table>");

	delete[] dotPlot;
}


// Generate an HTML report showing: DOTPLOT, Hamming Distance, Lenvenschtein Distance, Global Alignment, Local Alignment, FastA search
bool GenerateHTMLReportToFile(const char* subject, const char* pattern, int ktup, const char* filename  ) {
	FILE* fp;
	if((fp=fopen(filename, "wb"))==NULL) {
		printf("Failed creating report file ");
		exit(1);
	}

	int table[1024];
	fprintf(fp,"\n<b>Subject string:</b> %s\n<br><b>Pattern string:</b> %s\n<p><b>Hamming distance:</b> %d", subject, pattern, HammingDistance(subject, pattern));
	fprintf(fp,"\n<br><b>Levenschtein distance:</b> %d ",LevenschteinDistance(subject,pattern,table) );

	char alignment1[256]="";
	char alignment2[256]="";
	NeedlemanWunsch(subject, pattern, table, alignment1, alignment2);

	fprintf(fp,"\n<p><b>Needman-Wunsch alignment:</b>");
	fprintf(fp,"\n<blockquote>");
	int n = strlen(alignment1);
	fprintf(fp,"\n<table><tr>");
	for(int i=0;i<n;i++) {
		fprintf(fp,"\n<td>%c</td>", alignment1[i]);
	}
	fprintf(fp,"</tr><tr>");
	for(int i=0;i<n;i++) {
		fprintf(fp,"\n<td>%c</td>", alignment1[i]==alignment2[i]?'|':' ');
	}
	fprintf(fp,"</tr><tr>");
	for(int i=0;i<n;i++) {
		fprintf(fp,"\n<td>%c</td>", alignment2[i]);
	}
	fprintf(fp,"</tr>\n</table>");
	fprintf(fp,"\n</blockquote>");


	SmithWaterman(subject, pattern, alignment1, alignment2);
	fprintf(fp,"\n<p><b>Smith-Waterman alignment:</b>");
	fprintf(fp,"\n<blockquote>");
	n = strlen(alignment1);
	fprintf(fp,"\n<table><tr>");
	for(int i=0;i<n;i++) {
		fprintf(fp,"\n<td>%c</td>", alignment1[i]);
	}
	fprintf(fp,"</tr><tr>");
	for(int i=0;i<n;i++) {
		fprintf(fp,"\n<td>%c</td>", alignment1[i]==alignment2[i]?'|':' ');
	}
	fprintf(fp,"</tr><tr>");
	for(int i=0;i<n;i++) {
		fprintf(fp,"\n<td>%c</td>", alignment2[i]);
	}
	fprintf(fp,"</tr>\n</table>");
	fprintf(fp,"\n</blockquote>");



	fprintf(fp,"\n<p><b>Dot plot:</b></p>");
	HTMLDotPlotToFile(subject, pattern, fp);

	fprintf(fp,"\n<p><b>FastA match:</b><blockquote>");
	bool fastaOK = false;
	unsigned char match[256];
	if(SUCCEEDED(FastAMatching(subject, pattern, ktup, match))) {
		fastaOK = true;
		int len = strlen(subject);
		const char COLOR_CODE[][7] = { "ff0000", "00ff00"};
		int colorIndex = 0;
		int fallout = 0;
		bool notMatch = true;
		for(int i=0;i<len-ktup+1;i++) {
			if(!notMatch) {
				// check for not match
				notMatch = match[i]<=match[i-1];
			}
			if(notMatch && match[i]>0) {
				if(notMatch) {
					notMatch = false;
					colorIndex = !colorIndex;
				}
			}
			if(match[i]>0)
				fallout = ktup;

			fprintf(fp,"<font style='background-color:#%s'>%s%c%s</font>", fallout?COLOR_CODE[colorIndex]:"ffffff",
																	fallout?"<u>":"" , subject[i], fallout?"</u>":"" );
			--fallout;
			if(fallout<0)
				fallout = 0;

		}

		for(int i=len-ktup+1;i<len;i++)
			fprintf(fp,"<font style='background-color:#%s'>%s%c%s</font>", fallout?COLOR_CODE[colorIndex]:"ffffff",
																	fallout?"<u>":"" , subject[i], fallout?"</u>":"" );
	} else {
		fprintf(fp,"Invalid Input - make sure they are all uppercase characters.");
	}
	fprintf(fp,"\n</blockquote>");

	fclose(fp);

	return fastaOK;
}



BOOL CALLBACK EditDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_INITDIALOG:
			SetWindowText(hDlg, "Input Alignment strings");
			break;
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
			case IDOK:
				{
					HWND editHwnd;
					char buff[10];
					char subjectStr[200];
					char patternStr[200];
					int ktup;
					editHwnd = GetDlgItem(hDlg, IDC_EDIT1);
					GetWindowText(editHwnd, subjectStr, 200);
					editHwnd = GetDlgItem(hDlg, IDC_EDIT2);
					GetWindowText(editHwnd, patternStr, 200);
					editHwnd = GetDlgItem(hDlg, IDC_EDIT3);
					GetWindowText(editHwnd, buff, 200);
					sscanf(buff, "%d", &ktup);
					if(ktup<2 || ktup>=7) {
						MessageBox(hDlg, "Please enter ktup between 2 to 6", "Note", MB_OK);
					} else {
						GenerateHTMLReportToFile(subjectStr, patternStr, ktup, "CS599_report.html");
						MessageBox(hDlg, "The report has been written to CS599_report.html", "Success", MB_OK);
					}
				}
				break;
			case IDCANCEL:
				EndDialog(hDlg,0);
				return TRUE;
			}
	}
	return FALSE;
}


int main( int argc, char* argv[] ) {

	if( SDL_Init(SDL_INIT_VIDEO)<0 || !SDL_GetVideoInfo())
		return 1;
	const int WINDOW_WIDTH=300, WINDOW_HEIGHT=300;
	SDL_SetVideoMode(WINDOW_WIDTH,WINDOW_HEIGHT, SDL_GetVideoInfo()->vfmt->BitsPerPixel, SDL_RESIZABLE | SDL_SWSURFACE);

	HWND hwnd = GetActiveWindow();
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);

	SetWindowText(hwnd, "Alignment - CS 599 HW2");
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, EditDlgProc, (LPARAM)NULL);

	return 0;

}