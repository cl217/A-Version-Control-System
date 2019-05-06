#include "WTFheader.h"

#define PORT "21213"
#define IP "127.0.0.1"

pid_t serverProc;
void exitSignalHandler( int sig_num ){
	kill(serverProc, SIGINT);
}

void execWTF(char* folder, char* args[]){
	
	int i = 0;
	printf("\n%s: ", folder); 
	while( args[i] != NULL ){
		printf("%s ", args[i]);
		i++;
	}	
	printf("\n");
	
	int complete;
	pid_t clientProc=fork();
	if(clientProc==0){
		chdir(folder);
		execv("./WTF", args);
	}else{
		waitpid(clientProc, &complete,0);
	}
}

int main( int argc, char** argv ){

	signal(SIGINT, exitSignalHandler);	
	
	//Connection setup
	serverProc = fork();
	if(serverProc==0){
		chdir("./server");
		execv("./WTFserver", (char*[]) {"./WTFserver", PORT, NULL} );
	}
	
	execWTF("./client1", (char*[]) {"./WTF", "configure", IP, PORT, NULL} );
	
	execWTF("./client2", (char*[]) {"./WTF", "configure", IP, PORT, NULL} );
	
	
	//Project create
	execWTF("./client1", (char*[]) {"./WTF", "create", "testproj", NULL} );		

	
	//Client1: adding file1, commit and push
	execWTF("./client1", (char*[]) {"./WTF", "add", "testproj", "file3.txt", NULL} );
	
	execWTF("./client1", (char*[]) {"./WTF", "commit", "testproj", NULL} );
	
	
	execWTF("./client1", (char*[]) {"./WTF", "push", "testproj", NULL} );	
	
	
	//Client2: checkout
	execWTF("./client2", (char*[]) {"./WTF", "checkout", "testproj", NULL} );
	
	
	//Client2: remove file, commitx2 and push
	execWTF("./client2", (char*[]) {"./WTF", "remove", "testproj", "file3.txt", NULL} );
	
	execWTF("./client2", (char*[]) {"./WTF", "commit", "testproj", NULL} );
	execWTF("./client2", (char*[]) {"./WTF", "commit", "testproj", NULL} );
	
	execWTF("./client2", (char*[]) {"./WTF", "push", "testproj", NULL} );	
	
	
	//Client1: update and upgrade
	execWTF("./client1", (char*[]) {"./WTF", "update", "testproj", NULL} );	
	
	execWTF("./client1", (char*[]) {"./WTF", "upgrade", "testproj", NULL} );
	
	
	//Client1: add multiple files in subdir, commit, push
	execWTF("./client1", (char*[]) {"./WTF", "add", "testproj", "folder1/file1.txt",NULL} );	
	execWTF("./client1", (char*[]) {"./WTF", "add", "testproj", "folder1/folder2/test.c",NULL} );
	execWTF("./client1", (char*[]) {"./WTF", "add", "testproj", "folder1/folder2/Makefile",NULL} );
	execWTF("./client1", (char*[]) {"./WTF", "commit", "testproj",NULL} );
	execWTF("./client1", (char*[]) {"./WTF", "push", "testproj",NULL} );
	
	
	//Client2: update, upgrade, modify file1.txt, commit, push
	execWTF("./client2", (char*[]) {"./WTF", "update", "testproj",NULL} );
	execWTF("./client2", (char*[]) {"./WTF", "upgrade", "testproj",NULL} );

	printf("./client2: modifying testproj/folder1/file1.txt contents\n");
	int fd = open("./client2/testproj/folder1/file1.txt", O_WRONLY | O_TRUNC );
	char* str = "this file's contents has been changed.";
	write(fd, str, strlen(str));
	close(fd);
	
	execWTF("./client2", (char*[]) {"./WTF", "commit", "testproj", NULL} );
	execWTF("./client2", (char*[]) {"./WTF", "push", "testproj",NULL} );
	
	
	//Client1: update, upgrade
	execWTF("./client1", (char*[]) {"./WTF", "update", "testproj", NULL} );
	//execWTF("./client1", (char*[]) {"./WTF", "upgrade", "testproj", NULL} );
		
	//Client1: rollback to version 2 (with just the file3.txt)
	printf("\nclient1: ./WTF rollback testproj 2\n");
	execWTF("./client1", (char*[]) {"./WTF", "rollback", "testproj", "2", NULL} );
	
	//Client1: currentversion
	printf("\nclient1: ./WTF currentversion testproj\n");
	execWTF("./client1", (char*[]) {"./WTF", "currentversion", "testproj", NULL} );
	
	//Client1: history
	printf("\nclient1: ./WTF history testproj\n");
	execWTF("./client1", (char*[]) {"./WTF", "history", "testproj", NULL} );

	//Client2: destroy 		
	printf("client1: ./WTF destroy testproj\n");
	execWTF("./client1", (char*[]) {"./WTF", "destroy", "testproj", NULL} );
	
	kill(serverProc, SIGINT);
    	return 0;	
}
