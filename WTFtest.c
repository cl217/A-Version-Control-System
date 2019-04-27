#include "WTFheader.h"


int main( int argc, char** argv ){
	
	int code = atoi(argv[1]);
	
	if( code == 1 ){ //create client and server
		//Make server, client1, client2
		createDir("./server");
		createDir("./client1");
		createDir("./client2");
		
		//Make executable client server exe
		system("make");
	
		//Move WTFserver to server
		system("mv WTFserver server");
	
		//Copy WTF to client1
		system("cp WTF client1");
	
		//Move WTF to client2
		system("mv WTF client2");
	
		//Copy configure to client1 and client2
		system("cp .configure client1");
		system("cp .configure client2");
	}
	
	if( code == 2 ){
		system("./client1/WTF create testproj");
	}
	
	if( code == 3 ){
		//client1: add test files
		int fileFD = createFile("client1/testproj/file1.txt"); 
		close(fileFD);
		fileFD = createFile("client1/testproj/file2.c");
		write(fileFD, "text1", 5);
		close(fileFD);
		
		/*
		system("./client1/WTF add testproj file1.txt");
		system("./client1/WTF add testproj file2.c");
		system("./client1/WTF commit testproj");
		system("./client1/WTF push testproj");
		*/
	}
	
	return 0;
}
