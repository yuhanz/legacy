#ifndef _YUHAN_ZHANG_CS_599_CAL_POLY_FALL_08_
#define _YUHAN_ZHANG_CS_599_CAL_POLY_FALL_08_

// Given two strings (NULL terminated), find the Hamming Distance between the two
int HammingDistance(const char* str1, const char* str2);


// Given two strings (NULL terminated), find the LevenshcetenDistance, and return the table if it's given
// NOTE: the table is dynamically needs to be delete[] afterward by programmer if taken.
int LevenschteinDistance(const char* str1, const char* str2, int* pReturnTable=0 );


void ReverseString(char* str);



void NeedlemanWunsch(const char* str1, const char* str2, const int* pTable, char* alignment1, char* alignment2);

void SmithWaterman(const char* str1, const char* str2, char* alignment1, char* alignment2);


// Convert all lower case letter to upper case letters.
// If non-alphabetical character is found, stop the process and return E_FAILED.
bool IsUppercaseString(const char* str);




// Given a subject string a pattern string, find the positions that have the similiarity.
// Store the result in a match string, whose length equal to the length of the subject.
// In the match string 0 means unmatch, and an non-zero value indicate the length of the matches.
//	(trucate the match score to 0xFF if the number of consecutive matches greater than 255
// Assume the strings are limited to 26 characters.
HRESULT FastAMatching(const char* subject, const char* pattern, int ktup, unsigned char* match);


#endif