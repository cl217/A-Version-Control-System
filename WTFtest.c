#include "WTFheader.h"

#define PORT "12943"
#define IP "127.0.0.1"

pid_t serverProc;
void exitSignalHandler( int sig_num ){
	kill(serverProc, SIGINT);
}

void execWTF(char* folder, char* args[]){
	int complete;
	pid_t clientProc=fork();
	if(clientProc==0){
		chdir(folder);
		execv("./WTF", args);
	}else{
		waitpid(clientProc, &complete,0);
	}
}

void execTwo(char*folder1, char*args1[], char*folder2, char*args2[]){
	int complete;
	pid_t clientProc=fork();
	if(clientProc==0){
		chdir(folder1);
		execv("./WTF", args1);
		chdir(folder2);
		execv("./WTF", args2);
		
	}else{
		waitpid(clientProc, &complete,0);
	}
}



int main( int argc, char** argv ){

	signal(SIGINT, exitSignalHandler);	
	
	//Connection setup
	printf("\nserver: ./WTFserver <PORT>\n");
	serverProc = fork();
	if(serverProc==0){
		chdir("./server");
		execv("./WTFserver", (char*[]) {"./WTFserver", PORT, NULL} );
	}
	
	printf("\nclient1: ./WTF configure <IP> <PORT>\n");
	execWTF("./client1", (char*[]) {"./WTF", "configure", IP, PORT, NULL} );
	
	printf("\nclient2: ./WTF configure <IP> <PORT>\n");
	execWTF("./client2", (char*[]) {"./WTF", "configure", IP, PORT, NULL} );
	
	
	
	
	//Project setup
	printf("\nclient1: ./WTF create testproj\n");
	execWTF("./client1", (char*[]) {"./WTF", "create", "testproj", NULL} );		

	printf("\nclient2: ./WTF checkout testproj\n");
	execWTF("./client2", (char*[]) {"./WTF", "checkout", "testproj", NULL} );
	
	
	
	//Client1: adding file1, commit and push
	printf("\nclient1: ./WTF add testproj <filepath>\n");
	execWTF("./client1", (char*[]) {"./WTF", "add", "testproj", "folder1/file1.txt", NULL} );
		
	printf("\nclient1: ./WTF commit testproj\n");
	execWTF("./client1", (char*[]) {"./WTF", "commit", "testproj", NULL} );
	
	printf("\nclient1: ./WTF push testproj\n");
	execWTF("./client1", (char*[]) {"./WTF", "push", "testproj", NULL} );	
	
	
	//Client2: update and upgrade
	printf("\nclient2: ./WTF update testproj\n");
	execWTF("./client2", (char*[]) {"./WTF", "update", "testproj", NULL} );	
	
	printf("\nclient2: ./WTF upgrade testproj\n");
	execWTF("./client2", (char*[]) {"./WTF", "upgrade", "testproj", NULL} );
	
	
	//Client2: remove file1, commit and push
	printf("\nclient2: ./WTF remove testproj folder1/file1.txt");
	execWTF("./client2", (char*[]) {"./WTF", "remove", "testproj", "folder1/file1.txt", NULL} );
	
	printf("\nclient2: ./WTF commit testproj\n");
	execWTF("./client2", (char*[]) {"./WTF", "commit", "testproj", NULL} );
	
	printf("\nclient2: ./WTF push testproj\n");
	execWTF("./client2", (char*[]) {"./WTF", "push", "testproj", NULL} );	
	
	
	//Client1: update and upgrade
	printf("\nclient1: ./WTF update testproj\n");
	execWTF("./client1", (char*[]) {"./WTF", "update", "testproj", NULL} );	
	/*
	printf("\nclient1: ./WTF upgrade testproj\n");
	execWTF("./client1", (char*[]) {"./WTF", "upgrade", "testproj", NULL} );
	*/
	
	/*
	//Client1: history
	printf("\nclient1: ./WTF history testproj\n");
	execWTF("./client1", (char*[]) {"./WTF", "history", "testproj", NULL} );
	
	//Client1: rollback to version 2 (with the file1.txt)
	printf("\nclient1: ./WTF rollback testproj 2\n");
	execWTF("./client1", (char*[]) {"./WTF", "rollback", "testproj", "2", NULL} );
	
	//Client1: currentversion
	printf("\nclient1: ./WTF currentversion testproj\n");
	execWTF("./client1", (char*[]) {"./WTF", "currentversion", "testproj", NULL} );


	//Client2:
	
	
	//Client2: destroy 		
	printf("client2: ./WTF destroy testproj\n");
	execWTF("./client2", (char*[]) {"./WTF", "destroy", "testproj", NULL} );
	*/
	
	//kill(serverProc, SIGINT);
    	return 0;	
}
