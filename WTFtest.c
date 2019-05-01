#include "WTFheader.h"

#define PORT "23892"
#define IP "127.0.0.1"

int main( int argc, char** argv ){
	int complete;
	
	//server: ./WTFserver <PORT>
	pid_t serverProc = fork();
	if(serverProc==0){
		chdir("./server");
		char* arr[] = {"./WTFserver", PORT, NULL};
		execv("./WTFserver", arr);
	}
	
	//client1: ./WTF configure <IP> <PORT>
	pid_t clientProc=fork();
	if(clientProc==0){
		chdir("./client1");
		char* arr[] = {"./WTF", "configure", IP, PORT, NULL};
		execv("./WTF", arr);
	}else{
		waitpid(clientProc, &complete,0);
	}
	
	//client2: ./WTF configure <IP> <PORT>
	clientProc=fork();
	if(clientProc==0){
		chdir("./client2");
		char* arr[] = {"./WTF", "configure", IP, PORT, NULL};
		execv("./WTF", arr);
	}else{
		waitpid(clientProc, &complete,0);
	}

	//client1: ./WTF create testproj
	clientProc=fork();
	if(clientProc==0){
		chdir("./client1");
		char* arr[] = {"./WTF", "create", "testproj", NULL};
		execv("./WTF", arr);
	}else{
		waitpid(clientProc, &complete,0);
	}

	// client2: ./WTF checkout testproj
	clientProc=fork();
	if(clientProc==0){
		chdir("./client2");
		char* arr[] = {"./WTF", "checkout", "testproj", NULL};
		execv("./WTF", arr);
	}else{
		waitpid(clientProc, &complete,0);
	}
	
	
	// client1: ./WTF add testproj <filepath>
	clientProc=fork();
	if(clientProc==0){
		chdir("./client1");
		char* arr[] = {"./WTF", "add", "testproj", "folder1/file1.txt", NULL};
		execv("./WTF", arr);
	}else{
		waitpid(clientProc, &complete,0);
	}
	
	// client1: ./WTF commit testproj
	clientProc=fork();
	if(clientProc==0){
		chdir("./client1");
		char* arr[] = {"./WTF", "commit", "testproj", NULL};
		execv("./WTF", arr);
	}else{
		waitpid(clientProc, &complete,0);
	}
	
	// client1: ./WTF push testproj
	clientProc=fork();
	if(clientProc==0){
		chdir("./client1");
		char* arr[] = {"./WTF", "push", "testproj", NULL};
		execv("./WTF", arr);
	}else{
		waitpid(clientProc, &complete,0);
	}
	
	// client2: ./WTF update testproj
	clientProc=fork();
	if(clientProc==0){
		chdir("./client2");
		char* arr[] = {"./WTF", "update", "testproj", NULL};
		execv("./WTF", arr);
	}else{
		waitpid(clientProc, &complete,0);
	}
	
	//client2: ./WTF upgrade testproj
	clientProc=fork();
	if(clientProc==0){
		chdir("./client2");
		char* arr[] = {"./WTF", "upgrade", "testproj", NULL};
		execv("./WTF", arr);
	}else{
		waitpid(clientProc, &complete,0);
	}
	
	
	
	//TODO: 
	// ./WTF remove
	// ./WTF rollback
	// ./WTF versionhistory
	// ./WTF currentversion
	
	/*
	//client2: ./WTF destroy testproj
	clientProc=fork();
	if(clientProc==0){
		chdir("./client2");
		char* arr[] = {"./WTF", "destroy", "testproj", NULL};
		execv("./WTF", arr);
	}else{
		waitpid(clientProc, &complete,0);
	}
	*/
	
	kill(serverProc, SIGINT);
    	return 0;	
	
}
