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
	
	int port = -1;
	if(argc<3){
		printf("Error: invalid args\n"); return 1;
	}else{
		port = atoi(argv[2]);
	}
	

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd<0){
		printf("Error: can't open socket\n"); return 1;
	}
	printf("Status: new client created\n");
		
	struct hostent* server = gethostbyname(argv[1]);
	if(server==NULL){
		printf("Error: host not found\n"); return 1;
	}
	printf("Status: host found\n");
	
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;

	char* tempstr = (char*)malloc(sizeof(&server->h_addr_list[0])+1);
	
	memcpy(&serverAddr.sin_addr.s_addr, server->h_addr_list[0], sizeof(server->h_addr_list[0]));

	serverAddr.sin_port = htons(port);

	if(connect(sockfd, (struct sockaddr*) &serverAddr, sizeof(serverAddr))<0){
		printf("Error: connection failed\n"); return 1;
	}
	printf("Status: connected to server\n");
	
	
	signal(SIGINT, exitSignalHandler);
	char buffer[255];
	while(1){ //TODO: close on server shut down
	
		printf("To Server: ");
		fgets(buffer, 255, stdin);
		int code=write(sockfd, buffer, strlen(buffer));

		code = read(sockfd, buffer, 255);

		printf("From Server: %s\n", buffer);
	}
	

	return 0;
}
