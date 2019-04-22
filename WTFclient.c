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

void exitHandler(){
	close(sockfd);
	printf("Client socket closed\n");
	exit(0);
}


int main( int argc, char** argv ){
 	//Ctrl-C handler, closes ports before ending program
	signal(SIGINT, exitSignalHandler); 
	
	if( argc < 2 ){
		printf("invalid args\n"); return 0;
	}
	
	//  ./WTF randomargv2 for client-server connection test
	if(strcmp(argv[1], "configure")==0){ // ./WTF configure <IP> <port>
		if(argc != 4){
			printf("invalid args\n"); return 0;
		}else{
			wtfconfigure(argv[2], argv[3]);
		}
	}else if(strcmp(argv[1], "checkout")==0){
		if(argc != 3){
			printf("invalid args\n"); return 0;
		}else{
			wtfcheckout(argv[2]);
		}
	}else if(strcmp(argv[1], "update")==0){
		if(argc != 3){
			printf("invalid args\n"); return 0;
		}else{
			wtfupdate(argv[2]);
		}
	}else if(strcmp(argv[1], "upgrade")==0){
		if(argc != 3){
			printf("invalid args\n"); return 0;
		}else{
			wtfupdate(argv[2]);
		}
	}else if(strcmp(argv[1], "commit")==0){
		if(argc != 3){
			printf("invalid args\n"); return 0;
		}else{
			wtfcommit(argv[2]);
		}
	}else if(strcmp(argv[1], "push")==0){
		if(argc != 3){
			printf("invalid args\n"); return 0;
		}else{
			wtfpush(argv[2]);
		}
	}else if(strcmp(argv[1], "create")==0){
		if(argc != 3){
			printf("invalid args\n"); return 0;
		}else{
			wtfcreate(argv[2]);
		}
	}else if(strcmp(argv[1], "destroy")==0){
		if(argc != 3){
			printf("invalid args\n"); return 0;
		}else{
			wtfdestroy(argv[2]);
		}
	}else if(strcmp(argv[1], "add")==0){
		if(argc != 4){
			printf("invalid args\n"); return 0;
		}else{
			wtfadd(argv[2], argv[3]);
		}
	}else if(strcmp(argv[1], "remove")==0){
		if(argc != 4){
			printf("invalid args\n"); return 0;
		}else{
			wtfremove(argv[2], argv[3]);
		}
	}else if(strcmp(argv[1], "currentversion")==0){
		if(argc != 3){
			printf("invalid args\n"); return 0;
		}else{
			wtfcurrentversion(argv[2]);
		}
	}else if(strcmp(argv[1], "history")==0){
		if(argc != 3){
			printf("invalid args\n"); return 0;
		}else{
			wtfhistory(argv[2]);
		}
	}else if(strcmp(argv[1], "rollback")==0){
		if(argc != 4){
			printf("invalid args\n"); return 0;
		}else{
			wtfrollback(argv[2], argv[3]);
		}
	}else{
		//printf("Error: invalid argv[1]\n");
		
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
	}
	
	atexit(exitHandler);
}

/** Connect function for all commands that communicates with server**/
void wtfconnect(){
	//open to read from .configure file
	int configFile = open("./.configure", O_RDONLY);
	if( configFile == -1 ){
		printf("Error: no .configure found\n"); exit(1);
	}
	
	char* c = (char*)malloc(2*sizeof(char));
	char* word = NULL;
	int length = 0;
	//READ IN IP
	while( read(configFile, c, 1) > 0 && c[0] != '\n' ){
		word = append(word, c, length);
		length++;
	}
	struct hostent* server = gethostbyname(word);
	//READ IN PORT
	length = 0;
	word = NULL;
	while( read(configFile, c, 1) > 0 ){
		word = append(word, c, length);
		length++;
	}
	int port = atoi(word);
	
	//CREATE CLIENT
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd<0){
		printf("Error: can't open socket\n"); exit(1);
	}
	printf("Status: new client created\n");
	
	//FIND SERVER	
	if(server==NULL){
		printf("Error: host not found\n"); atexit(exitHandler);
	}
	printf("Status: host found\n");
	
	//CONNECT TO SERVER
	//TODO: LOOP TRY TO CONNECT EVERY 3 SECONDS UNTIL CONNECTED OR KILLED
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	char* tempstr = (char*)malloc(sizeof(&server->h_addr_list[0])+1);
	memcpy(&serverAddr.sin_addr.s_addr, server->h_addr_list[0], sizeof(server->h_addr_list[0]));
	serverAddr.sin_port = htons(port);
	if(connect(sockfd, (struct sockaddr*) &serverAddr, sizeof(serverAddr))<0){
		printf("Error: connection failed\n"); atexit(exitHandler);
	}
	printf("Status: connected to server\n");
}

void wtfconfigure( char* ip, char* port){
	int file = open( "./.configure", O_CREAT | O_WRONLY | O_TRUNC, 0777 );
	if( file < 0 ){
		printf("Error: .configure file creation failed\n"); return;
	}
	write(file, ip, strlen(ip));
	write(file, "\n", 1);
	write(file, port, strlen(port));
	close(file);
}

/*	1.1	*/
void wtfcreate( char* projectname ){
	wtfconnect();
}

/*	1.2	*/
void wtfdestroy( char* projectname ){}

/*	1.2	*/
void wtfcheckout( char* projectname ){}

/*	2.1	*/
void wtfadd( char* projectname, char* filename ){}

/*	2.2	*/
void wtfremove( char* projectname, char* filename ){}

/*	3.1	*/
void wtfcommit( char* projectname ){}

/*	3.2	*/
void wtfpush( char* projectname ){}

/* 	4.1	*/
void wtfcurrentversion( char* projectname ){}

/*	4.1	*/
void wtfhistory( char* history ){}

/*	4.1	*/
void wtfrollback( char* projectname, char* version ){}

/*	4.1	*/
void wtfupdate( char* projectname ){}

/*	4.2-After update()	*/
void wtfupgrade( char* projectname ){}
