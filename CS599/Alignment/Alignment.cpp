#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include "Alignment.h"

#include <map>
using namespace std;


// Given two strings (NULL terminated), find the Hamming Distance between the two
int HammingDistance(const char* str1, const char* str2) {
	int count = 0;
	char* p1 = (char*)str1;
	char* p2 = (char*)str2;
	while(*p1!=0 && *p2!=0) {
		if(*(p1++) != *(p2++) )
			++count;
	}
	while(*p1!=0) {
		++p1;
		++count;
	}
	while(*p2!=0) {
		++p2;
		++count;
	}
	return count;
}


// Given two strings (NULL terminated), find the LevenshcetenDistance, and return the table if it's given
// NOTE: the table is dynamically needs to be delete[] afterward by programmer if taken.
int LevenschteinDistance(const char* str1, const char* str2, int* pReturnTable ) {
	int len1 = strlen(str1);
	int len2 = strlen(str2);
	int tableWidth = len1+1;
	int tableHeight = len2+1;
	int* pTable = new int[tableWidth*tableHeight];
	for(int i=0;i<tableWidth;i++)
		pTable[i] = i;
	for(int j=0;j<tableHeight;j++)
		pTable[j*tableWidth] = j;

	char* p2 = (char*) str2;
	for(int j=1;j<tableHeight;j++) {
		char* p1 = (char*)str1;
		for(int i=1;i<tableWidth;i++){
			int currentIndex = j*tableWidth+i;
			int verticalIndex = currentIndex-tableWidth;
			int score = pTable[verticalIndex] + 1;	// vertical score
			int horizontalScore = pTable[currentIndex-1] + 1;
			int diagnalScore = pTable[verticalIndex-1] + (*p1 != *p2);
			if(horizontalScore < score)
				score = horizontalScore;
			if(diagnalScore < score)
				score = diagnalScore;
			pTable[currentIndex] = score;
			++p1;
		}
		++p2;
	}

	int distance = pTable[tableWidth*tableHeight-1];
	/*	// DEBUG: display the table
	for(int j=0;j<tableHeight;j++) {
		printf("\n");
		for(int i=0;i<tableWidth;i++){
			printf("%d ", pTable[j*tableWidth+i]);
		}
	}*/

	if(pReturnTable) {
		for(int i=0;i<tableWidth*tableHeight;i++)
			pReturnTable[i] = pTable[i];
	}

	delete[] pTable;
	return distance;
}

// Do an in-place reversion of the string
void ReverseString(char* str) {
	int len = strlen(str);
	char* p1 = str;
	char* p2 = str + len-1;
	char temp;
	while(p1<p2) {
		temp = *p1;
		*p1++ = *p2;
		*p2-- = temp;
	}
}


// Given two strings, and their table from LevenschteinDistance, find out the alignment.
void NeedlemanWunsch(const char* str1, const char* str2, const int* pTable, char* alignment1, char* alignment2) {
	int len1 = strlen(str1);
	int len2 = strlen(str2);
	int tableWidth = len1+1;
	int tableHeight = len2+1;
	int i=len1;
	int j=len2;
	char* p1 = alignment1;
	char* p2 = alignment2;
	while(i>0 || j>0) {
		int from;	// 0: from diagnal; 1: from vertical; 2: from horizontal
		enum {DIAGNAL, VERTICAL, HORIZONTAL};
		char c1 = str1[i-1];
		char c2 = str2[j-1];

		if(i==0)
			from = VERTICAL;
		else if(j==0)
			from = HORIZONTAL;
		else {
			int currentIndex = j*tableWidth+i;
			int verticalIndex = currentIndex-tableWidth;
			int verticalScore = pTable[verticalIndex] + 1;	// vertical score
			int horizontalScore = pTable[currentIndex-1] + 1;
			int score = pTable[verticalIndex-1] + (c1!=c2);
			from = DIAGNAL;
			if(verticalScore < score) {
				score = verticalScore;
				from = VERTICAL;
			} else if(horizontalScore < score) {
				score = horizontalScore;
				from = HORIZONTAL;
			}
		}

		if(from==DIAGNAL) {
			*p1++ = c1;
			*p2++ = c2;
			--i;
			--j;
		} else if(from==VERTICAL) {
			*p1++ = '-';
			*p2++ = c2;
			--j;
		} else {	// from horizontal
			*p1++ = c1;
			*p2++ = '-';
			--i;
		}
	}
	*p1 = 0;	// zero terminated string
	*p2 = 0;	// zero terminated string
	ReverseString(alignment1);
	ReverseString(alignment2);
}




// Given two strings, find out the local alignment.
void SmithWaterman(const char* str1, const char* str2, char* alignment1, char* alignment2) {
	int len1 = strlen(str1);
	int len2 = strlen(str2);
	int tableWidth = len1+1;
	int tableHeight = len2+1;
	int* pTable = new int[tableWidth*tableHeight];
	for(int i=0;i<tableWidth;i++)
		pTable[i] = 0;
	for(int j=0;j<tableHeight;j++)
		pTable[j*tableWidth] = 0;

	char* p2 = (char*) str2;
	char* p1;
	for(int j=1;j<tableHeight;j++) {
		p1 = (char*)str1;
		for(int i=1;i<tableWidth;i++){
			int currentIndex = j*tableWidth+i;
			int verticalIndex = currentIndex-tableWidth;
			int score = pTable[verticalIndex] - 2;	// vertical score
			int horizontalScore = pTable[currentIndex-1] - 2;
			int diagnalScore = pTable[verticalIndex-1] + (*p1==*p2 ? 2 : -3);
			if(score<0)
				score = 0;
			if(horizontalScore<0)
				horizontalScore=0;
			if(diagnalScore<0)
				diagnalScore=0;
			if(horizontalScore > score)
				score = horizontalScore;
			if(diagnalScore > score)
				score = diagnalScore;
			pTable[currentIndex] = score;
			++p1;
		}
		++p2;
	}
	
	// find the maximum
	p1 = alignment1;
	p2 = alignment2;
	int x,y;
	int maxIndex=0;		// index of the max value in the table
	int maxValue=0;
	int from;	// 0: from diagnal; 1: from vertical; 2: from horizontal
	enum {DIAGNAL, VERTICAL, HORIZONTAL};

	for(int i=0;i<tableHeight*tableWidth;i++) {
		if(pTable[i]>maxValue) {
			maxIndex = i;
			maxValue = pTable[i];
		}
	}
	x = maxIndex % tableWidth;
	y = maxIndex / tableWidth;
	int currentIndex = maxIndex;
	while(pTable[currentIndex]>0) {
		char c1 = str1[x-1];
		char c2 = str2[y-1];
		int verticalIndex = currentIndex-tableWidth;
		int verticalScore = pTable[verticalIndex] - 2;	// vertical score
		int horizontalScore = pTable[currentIndex-1] - 2;
		int score = pTable[verticalIndex-1] + (c1==c2 ? 2 : -3);
		if(score<0)
			score = 0;
		if(horizontalScore<0)
			horizontalScore=0;
		if(verticalScore<0)
			verticalScore=0;

		from = DIAGNAL;
		if(verticalScore > score) {
			score = verticalScore;
			from = VERTICAL;
		} else if(horizontalScore > score) {
			score = horizontalScore;
			from = HORIZONTAL;
		}

		if(from==DIAGNAL) {
			*p1++ = c1;
			*p2++ = c2;
			--x;
			--y;
		} else if(from==VERTICAL) {
			*p1++ = '-';
			*p2++ = c2;
			--y;
		} else {	// from horizontal
			*p1++ = c1;
			*p2++ = '-';
			--x;
		}

		currentIndex = y * tableWidth + x;
	}
	*p1 = 0;	// zero terminated string
	*p2 = 0;	// zero terminated string
	ReverseString(alignment1);
	ReverseString(alignment2);

	delete[] pTable;
}




// Convert all lower case letter to upper case letters.
// If non-alphabetical character is found, stop the process and return E_FAILED.
bool IsUppercaseString(const char* str) {
	char c;
	while((c=*str)!=0) {
		if(c<'A' || c>'Z')
			return false;
		str++;
	}
	return true;
}

int _26ToKey(char* p, int k) {
	int key=0;
	for(int j=0;j<k;j++) {
		key *= 26;
		key += *p;
		++p;
	}
	return key;
}


// Given a subject string a pattern string, find the positions that have the similiarity.
// Store the result in a match string, whose length equal to the length of the subject.
// In the match string 0 means unmatch, and an non-zero value indicate the length of the matches.
//	(trucate the match score to 0xFF if the number of consecutive matches greater than 255
// Assume the strings are limited to 26 characters.
HRESULT FastAMatching(const char* subject, const char* pattern, int ktup, unsigned char* match) {
	if(ktup>=7 || ktup<=1)		// not designed to handle 7-tuples or larger pattern. (because it will be hard to encode a key
		return E_FAIL;
	if(!IsUppercaseString(subject) || !IsUppercaseString(pattern))
		return E_FAIL;	// reject ill formatted input
	int len[2];
	len[0] = strlen(subject);
	len[1] = strlen(pattern);

	// make the alphabetical string into numbers: 0-25
	char* str[2];
	for(int i=0;i<2;i++) {
		char* p =str[i] = new char[len[i]+1];
		if(i==0)
			strcpy(str[0], subject);
		else
			strcpy(str[1], pattern);
		while(*p!=0) {
			*p = *p - 'A';
			++p;
		}
	}

	map<unsigned int, unsigned int> subjectHashtable;
	map<unsigned int, unsigned int> patternHashtable;
	map<unsigned int, unsigned int>::iterator it;
	map<unsigned int, unsigned int>* pHashtable;

	unsigned int* sequence = new unsigned int [len[0]-ktup+1];

	for(int m=0;m<2;m++) {
		char* p = str[m];
		unsigned int count = 0;
		pHashtable = m==0? &subjectHashtable : &patternHashtable;
		for(unsigned int i=0;i<len[m]-ktup+1;i++) {
			unsigned int key = _26ToKey(p,ktup);

			if((it=pHashtable->find(key))==pHashtable->end()) {
				if(m==0)
					sequence[i] = count;
				(*pHashtable)[key] = count++;
			}
			else {
				if(m==0)
					sequence[i] = (*it).second;
			}
			++p;
		}
	}


	char* p = str[0];
	const unsigned int NO_MATCH = - pHashtable->size();
	int diff = NO_MATCH;		// the gap between mapped subject string and mapped patter string.
	int sameCount=0;	// the number of the same conscicutive gaps encountered.
	for(int i=0;i<len[0]-ktup+1;i++) {
		int key = _26ToKey(p,ktup);
		if((it=patternHashtable.find(key))==patternHashtable.end()) {
			// no corresponding sequence found in the pattern.
			sameCount=0;
			diff = NO_MATCH;
		} else {
			int newDiff = sequence[i] - (*it).second;
			if(newDiff==diff) {
				++sameCount;
			} else {
				diff = newDiff;
				sameCount = 1;
			}
		}
		++p;
		match[i] = sameCount>255? 255: sameCount;
	}


	for(int i=0;i<2;i++) {
		delete[] str[i];
	}
	subjectHashtable.clear();
	patternHashtable.clear();
	delete[] sequence;


	return S_OK;
}
