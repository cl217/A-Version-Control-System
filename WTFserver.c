#include "WTFheader.h"


/*
	./WTFserver <port>
	./WTFserver
	Port: 1024-9999
*/	

int sockfd;
int newsockfd;

void exitSignalHandler( int sig_num ){
	close(newsockfd);
	close(sockfd);	
	printf("Server socket closed\n");
	exit(0);
}

int main( int argc, char** argv ){

	printf("wtfserver\n");
	
	int port = -1;	
	//read in argument <port>
	if( argc != 2 ){
		printf("Error: Invalid arguments\n");
		printf("Usage: ./WTFserver <port>\n");
		return 1; 
	}else{
		port = atoi(argv[1]); //string to integer
	}
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("Error: Can't open socket\n"); return 1;
	}
	printf("server: running...\n");
	
	//struct sockaddr_in serverAddress = (struct sockaddr_in)malloc(sizeof(struct sockaddr_in));
	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(port);

	//bind	
	if( bind(sockfd,(struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0 ){
		printf("Error: Can't bind.\n"); return 1;
	}
	
	listen(sockfd, 10); //max num?
	
	struct sockaddr_in clientAddress;
	socklen_t clientLen = sizeof(clientAddress);
	
	newsockfd = accept(sockfd, (struct sockaddr *) &clientAddress, &clientLen);
	if(newsockfd<0){
		printf("Error: Accept failed.\n"); return 1;
	}
	
	printf("new client accepted...\n");
	
	
	char buffer[255];
	signal(SIGINT, exitSignalHandler);
	while(1){
		
		int code = read(newsockfd, buffer, 255);	

		printf("From Client: %s\n", buffer);
		
		
		printf("To Client: ");
		fgets(buffer, 255, stdin);
		code = write(newsockfd, buffer, strlen(buffer));

		
	}
	
	
	return 0;	
}
