#include "WTFheader.h"


int main( int argc, char** argv ){

	char* projectname = "folder";
	char* compressPath = "./compress/1.gz";
	char* decompress = "./decompress";
	
	compressProject(projectname, compressPath);
	printf("client11\n");
	
	decompressDir(compressPath, decompress);
	
	return 0;
}
