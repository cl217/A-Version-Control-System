#include "WTFheader.h"


/*
	./WTF configure <IP> <port> 
	IP: 127.0.0.1 for localhost
	Port: 1024-9999
*/

int isExit = 0;
int sockfd;

void exitSignalHandler( int sig_num ){
	close(sockfd);	
	printf("Client socket closed\n");
	exit(0);
}


int main( int argc, char** argv ){
 	printf("wtfclient\n");
 	
 	//Ctrl-C handler, closes ports before ending program
	signal(SIGINT, exitSignalHandler); 
	

	
	/** Testing Purposes **/
	wtfconnect();
	char buffer[255];
	while(1){ //TODO: close on server shut down?
	
		printf("To Server: ");
		fgets(buffer, 255, stdin);
		write(sockfd, buffer, strlen(buffer));
		
		read(sockfd, buffer, 255);
		printf("Server: %s\n", buffer);
	}
	
	return 0;
}

/** Connect function for all commands that communicates with server**/
void wtfconnect(){
	//TODO: must fail if .configure file not found
	
	int port = atoi("2000"); //TODO: Port-change to get from .configure file
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd<0){
		printf("Error: can't open socket\n"); exit(1);
	}
	printf("Status: new client created\n");
		
	struct hostent* server = gethostbyname("127.0.0.1"); //TODO: IP-change to get from .configure file
	if(server==NULL){
		printf("Error: host not found\n"); exit(1);
	}
	printf("Status: host found\n");
	
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;

	char* tempstr = (char*)malloc(sizeof(&server->h_addr_list[0])+1);
	
	memcpy(&serverAddr.sin_addr.s_addr, server->h_addr_list[0], sizeof(server->h_addr_list[0]));

	serverAddr.sin_port = htons(port);

	if(connect(sockfd, (struct sockaddr*) &serverAddr, sizeof(serverAddr))<0){
		printf("Error: connection failed\n"); exit(1);
	}
	printf("Status: connected to server\n");
}

void wtfconfigure(int ip, int port){
	//TODO: write out ip and port to ./.configure file
}

void wtfcheckout(){}

void wtfupdate(){}

void wtfupgrade(){}

void wtfcommit(){}

void wtfpush(){}

void wtfcreate(){}

void wtfdestroy(){}

void wtfadd(){}

void wtfremove(){}

void wtfcurrentversion(){}

void wtfhistory(){}

void wtfrollback(){}

