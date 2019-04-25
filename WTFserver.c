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
		serverSendManifest(dataList);
		
	}else if(strcmp(command, "upgrade")==0){

	}else if(strcmp(command, "commit")==0){
		serverSendManifest(dataList);
		serverCommit(dataList->PROJECTNAME);
	}else if(strcmp(command, "push")==0){
		serverPush(dataList);
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

void serverCommit(char* projectname){
	struct node* dataList = recieveData(newsockfd); //gets the new commit data
	//count num of active commits in .commit folder
	
	char* commitFolderPath = getPath(projectname, COMMIT);
	int countFiles = 1;
	DIR* dir = opendir(commitFolderPath);
	struct dirent* entry;
	while((entry=readdir(dir)) != NULL ){
		if(entry->d_type == DT_REG){
			countFiles++;
		}
	}
	closedir(dir);
	
	int commitFD = createFile(getPath(commitFolderPath, int2str(countFiles)));
	char* writeout = dataList->FIRSTFILENODE->content;
	printf("%s\n", writeout);
	write(commitFD, writeout, strlen(writeout));
	close(commitFD);
}

void serverPush(struct node* dataList){
	printf("serverpush\n");
	printf("projectname: %s\n", dataList->PROJECTNAME);
	int exists = dirExists(dataList->PROJECTNAME);
	if( exists == 0 ){
		char* data = appendData("push", "Error");
		sendData(newsockfd, data);
		return;
	}
	
	char* commitFPath = getPath(dataList->PROJECTNAME, COMMIT);
	DIR* dir = opendir(commitFPath);
	struct dirent* entry;
	while((entry=readdir(dir)) != NULL ){
		if(entry->d_type == DT_REG){
			if( strcmp(dataList->FIRSTFILENODE->content, 
					readFileData(getPath(commitFPath, entry->d_name))) == 0 ){
				printf("commit matched\n");
				//expire all commits
			}
		}
	}
	closedir(dir);
	
	char* archivePath = getPath(dataList->PROJECTNAME, ARCHIVE);
	dir = opendir(archivePath);
	int countFiles = -1;
	while((entry=readdir(dir)) != NULL ){
		countFiles++;
	}
	closedir(dir);
	
	//duplicate of project, .project
	char* cpyPath = append(".", dataList->PROJECTNAME);
	char* syscommand = append("cp -r ", dataList->PROJECTNAME);
	syscommand = append(syscommand, " ");
	syscommand = append(syscommand, cpyPath);
	system(syscommand);
	
	//move to archive
	syscommand = append("mv ", cpyPath);
	syscommand = append(syscommand, " ");
	cpyPath = getPath(archivePath, int2str(countFiles));
	syscommand = append(syscommand, cpyPath);
	system(syscommand);
	
	//remove all commits
	dir = opendir(commitFPath);
	while((entry=readdir(dir)) != NULL ){
		if(entry->d_type == DT_REG){
			remove(getPath(commitFPath, entry->d_name));
		}
	}
	closedir(dir);
	
	//remove all deleted commits from list of commits
	struct manifestNode* cList = parseManifest(dataList->FIRSTFILENODE->content);
	
	struct manifestNode* cPtr = cList;
	while( cPtr != NULL ){
		//remove all deleted commits from list of commits
		if(strcmp(cPtr->code, "deleted")==0){
			remove(cPtr->path);		
		}
		cPtr = cPtr->next;
	}
	
	printf("193\n");
	//create/rewrite all the files sent
	struct node* ptr = dataList->FIRSTFILENODE;
	while( ptr != NULL ){
		printf("path: %s\n", ptr->name);
		printf("content %s\n", ptr->content);
		
		int fd = open( ptr->name, O_WRONLY|O_CREAT|O_TRUNC, 0666 );
		if( fd<0 ){
			printf("Error: server198push");
		}
		if( ptr->content != NULL ){
			write(fd, ptr->content, strlen(ptr->content));
		}
		close(fd);
		ptr=ptr->next;	
	}
	
	sendData(newsockfd, appendData("push", "Success"));
}

//sends the manifest for project to client
void serverSendManifest(struct node* dataList){
	char* projectname = dataList->PROJECTNAME;
	if( dirExists(projectname) == 0 ){
		char* data = appendData(dataList->name, "Error");
		sendData(newsockfd, data);
	}
	sendData(newsockfd,
				versionData(dataList->name,projectname,getPath(projectname,MANIFEST)));

}


//Create project and manifests, sends manifest to client
void serverCreate(struct node* dataList){
	char* projectname = dataList->PROJECTNAME;
	int exists = dirExists(projectname);
	if( exists == 1 ){
		char* data = appendData("create", "Error");
		sendData(newsockfd, data);
		return;
	}
	createDir(projectname);
	createDir(getPath(projectname, ".version"));
	createDir(getPath(projectname, ".commit"));
	char* manifestPath = getPath(projectname, MANIFEST);
	newVersionFile(1, manifestPath);
	
	writeToVersionFile(manifestPath, "uptodate", 1, manifestPath, generateHash(""));
	
	sendData(newsockfd, versionData("create",projectname, manifestPath));
}




