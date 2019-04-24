#include "WTFheader.h"

/*
	TODO: MUTEX
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

	signal(SIGINT, exitSignalHandler);
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
	
	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(port);

	if( bind(sockfd,(struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0 ){
		printf("Error: Can't bind.\n"); return 1;
	}
	
	listen(sockfd, 10); //max num?
	
	struct sockaddr_in clientAddress;
	socklen_t clientLen = sizeof(clientAddress);
	
	char buffer[255]; //temporary

	pid_t childpid;
	while(1){
		newsockfd = accept(sockfd, (struct sockaddr *) &clientAddress, &clientLen);
		if(newsockfd<0){
			printf("Error: Accept failed.\n"); return 1;
		}
		printf("new client accepted...\n\n");
		
		if((childpid = fork())==0){
			close(sockfd);
			while(1){
				
				struct node* dataList = recieveData(newsockfd);
				
				executeCommand(dataList);
			}
		}
	}
	return 0;	
}
 
void executeCommand(struct node* dataList){

	char* command = dataList->name;
	
	if(strcmp(command, "checkout")==0){
	
	}else if(strcmp(command, "update")==0){
	
	}else if(strcmp(command, "upgrade")==0){

	}else if(strcmp(command, "commit")==0){

	}else if(strcmp(command, "push")==0){

	}else if(strcmp(command, "create")==0){
		serverCreate(dataList);
	}else if(strcmp(command, "destroy")==0){

	}else if(strcmp(command, "currentversion")==0){

	}else if(strcmp(command, "history")==0){

	}else if(strcmp(command, "rollback")==0){

	}else{
		//should never happen
	}
}

/*
	<command><dataType><bytesPname><projectName>
			<numFile><bytesfName><fName><bytefContent><fContent>..
*/
void serverCreate(struct node* dataList){
	printf("serverCreate\n");
	
	//traverse(dataList);
	
	
	char* projectname = dataList->next->next->name;
	
	int exists = dirExists(projectname);
	if( exists == 1 ){
		printf("server120\n");
		char* data = appendData("create", "Error");
		sendData(newsockfd, data);
		return;
	}
	
	
	createDir(projectname);
	char* manifestPath = getPath(projectname, MANIFEST);
	int code = createFile(manifestPath);
	if(code==0){
		printf("Error: manifest creation\n");
	}else{
		printf("manifest created\n");
	}
	
	char* manifestData = writeToManifest(manifestPath,1, manifestPath, generateHash(""));
	printf("write manifest: %s\n", manifestData );
	
	//TODO: could prob move to a loop+function
	char* data = appendData("create", "ProjectFileContent"); //command, dataType
	data = appendData(data, int2str(strlen(projectname))); //bytesPname
	data = appendData(data, projectname); //projectName
	data = appendData(data, int2str(1)); //numFiles
	data = appendData(data, int2str(strlen(manifestPath))); //bytesfName
	data = appendData(data, manifestPath); //fName
	data = appendData(data, int2str(strlen(manifestData))); //bytefContent
	data = appendData(data, manifestData); //Content
	
	sendData(newsockfd, data);
	
}




