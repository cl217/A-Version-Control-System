#include "WTFheader.h"


int main( int argc, char** argv ){
	
	//Make client1, client2, server
	createDir("./client1");
	createDir("./client2");
	createDir("./server");
	
	
	//spawn child process to run the server
	pid_t pid=fork();
	if(pid==0){
		execv("make", argv);
	}
	
	
    return 0;	
			
	
}
