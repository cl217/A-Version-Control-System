#include "WTFheader.h"

//helper to build the token while reading in
//append("dest", "src") returns "destsrc"
char* append( char* dest, char* src, int length ){
	if( dest == NULL ){
		char* newString = (char*)malloc(2*sizeof(char));
		strcpy(newString, src);
		return newString;
	}
	char* newString = (char*)malloc((length+1)*sizeof(char));
	memcpy(newString, dest, length);
	newString[length] = '\0';
	strcat(newString, src);
	return newString;
}
