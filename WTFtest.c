#include "WTFheader.h"

#define PORT "4000"
#define IP "127.0.0.1"

pid_t serverProc;
void exitSignalHandler( int sig_num ){
	kill(serverProc, SIGINT);
}

int main( int argc, char** argv ){

	signal(SIGINT, exitSignalHandler);	
	
	int complete;
	
	printf("\nserver: ./WTFserver <PORT>\n");
	serverProc = fork();
	if(serverProc==0){
		chdir("./server");
		char* arr[] = {"./WTFserver", PORT, NULL};
		execv("./WTFserver", arr);
	}
	
	
	printf("\nclient1: ./WTF configure <IP> <PORT>\n");
	pid_t clientProc=fork();
	if(clientProc==0){
		chdir("./client1");
		char* arr[] = {"./WTF", "configure", IP, PORT, NULL};
		execv("./WTF", arr);
	}else{
		waitpid(clientProc, &complete,0);
	}
	
	printf("\nclient2: ./WTF configure <IP> <PORT>\n");
	clientProc=fork();
	if(clientProc==0){
		chdir("./client2");
		char* arr[] = {"./WTF", "configure", IP, PORT, NULL};
		execv("./WTF", arr);
	}else{
		waitpid(clientProc, &complete,0);
	}

	
	printf("\nclient1: ./WTF create testproj\n");
	clientProc=fork();
	if(clientProc==0){
		chdir("./client1");
		char* arr[] = {"./WTF", "create", "testproj", NULL};
		execv("./WTF", arr);
	}else{
		waitpid(clientProc, &complete,0);
	}

	printf("\nclient2: ./WTF checkout testproj\n");
	clientProc=fork();
	if(clientProc==0){
		chdir("./client2");
		char* arr[] = {"./WTF", "checkout", "testproj", NULL};
		execv("./WTF", arr);
	}else{
		waitpid(clientProc, &complete,0);
	}
	
	
	printf("\nclient1: ./WTF add testproj <filepath>\n");
	clientProc=fork();
	if(clientProc==0){
		chdir("./client1");
		char* arr[] = {"./WTF", "add", "testproj", "folder1/file1.txt", NULL};
		execv("./WTF", arr);
	}else{
		waitpid(clientProc, &complete,0);
	}
	
	printf("\nclient1: ./WTF commit testproj\n");
	clientProc=fork();
	if(clientProc==0){
		chdir("./client1");
		char* arr[] = {"./WTF", "commit", "testproj", NULL};
		execv("./WTF", arr);
	}else{
		waitpid(clientProc, &complete,0);
	}
	
	printf("\nclient1: ./WTF push testproj\n");
	clientProc=fork();
	if(clientProc==0){
		chdir("./client1");
		char* arr[] = {"./WTF", "push", "testproj", NULL};
		execv("./WTF", arr);
	}else{
		waitpid(clientProc, &complete,0);
	}
	
	printf("\nclient2: ./WTF update testproj\n");
	clientProc=fork();
	if(clientProc==0){
		chdir("./client2");
		char* arr[] = {"./WTF", "update", "testproj", NULL};
		execv("./WTF", arr);
	}else{
		waitpid(clientProc, &complete,0);
	}
	
	printf("\nclient2: ./WTF upgrade testproj\n");
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
	
	
	//client2: ./WTF destroy testproj
	clientProc=fork();
	if(clientProc==0){
		chdir("./client2");
		char* arr[] = {"./WTF", "destroy", "testproj", NULL};
		execv("./WTF", arr);
	}else{
		waitpid(clientProc, &complete,0);
	}
	
	
	kill(serverProc, SIGINT);
    	return 0;	
	
}
