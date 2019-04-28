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

	listen(sockfd, 10); //TODO: max num?

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
    serverSendManifest(dataList);
    serverCheckout(dataList->PROJECTNAME);
	}else if(strcmp(command, "update")==0){
		serverSendManifest(dataList);
	}else if(strcmp(command, "upgrade")==0){
		serverUpgrade(dataList);
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

void serverCheckout(char* projectname) {
  //printf("\nx\n");
  char * manPath = getPath(projectname,MANIFEST);
  char * manData = readFileData(manPath);
  struct manifestNode * manList = parseManifest(manData);
  printf("manData: %s\n",manData);

  struct manifestNode* ptr = manList->next;
  char * fileList = "";
  char * contentList = "";
  printf("%s\n",ptr->hash);

  //create strings: one of file list and one of content of all files
  int fileCount = 0;
	while( ptr != NULL ) {
    fileList = append(fileList, ptr->path);
    contentList = append(contentList,readFileData(ptr->path));
    fileCount++;
		ptr = ptr->next;
	}
  //printf("\nfileList: %s\ncontList: %s\n", fileList,contentList);
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
	write(commitFD, writeout, strlen(writeout));
	close(commitFD);
}

void serverPush(struct node* dataList){
	//TODO: Revert on failure
	printf("serverpush\n");
	printf("projectname: %s\n", dataList->PROJECTNAME);
	int exists = dirExists(dataList->PROJECTNAME);
	if( exists == 0 ){
		char* data = appendData("push", "Error");
		sendData(newsockfd, data);
		return;
	}

	char* commitFPath = getPath(dataList->PROJECTNAME, COMMIT);
	//next->next->next->next
	printf("commit: %s\n", dataList->next->next->next->next->name);


	DIR* dir = opendir(commitFPath);
	struct dirent* entry;
	printf("server141\n");
	while((entry=readdir(dir)) != NULL ){
		if(entry->d_type == DT_REG){
			printf("entry: %s\n", entry->d_name);
			if( strcmp(dataList->FIRSTFILENODE->content,
					readFileData(getPath(commitFPath, entry->d_name))) == 0 ){
				printf("commit matched\n");
				//expire all commits
			}else{
				printf("commit not matched\n");
			}
		}
	}
	closedir(dir);

	printf("152\n");

	char* archivePath = getPath(dataList->PROJECTNAME, ARCHIVE);
	dir = opendir(archivePath);
	int countFiles = -1;
	while((entry=readdir(dir)) != NULL ){
		countFiles++;
	}
	closedir(dir);

	printf("server167\n");

	//duplicate of project, .project - TODO: do this in a less lazy way
	char* cpyPath = append(".", dataList->PROJECTNAME);
	char* syscommand = append("cp -r ", dataList->PROJECTNAME);
	syscommand = append(syscommand, " ");
	syscommand = append(syscommand, cpyPath);
	system(syscommand);

	printf("server176\n");

	//move to archive - TODO: do this in a less lazy way
	syscommand = append("mv ", cpyPath);
	syscommand = append(syscommand, " ");
	cpyPath = getPath(archivePath, int2str(countFiles));
	syscommand = append(syscommand, cpyPath);
	system(syscommand);

	printf("server185\n");

	//remove all commits
	dir = opendir(commitFPath);
	while((entry=readdir(dir)) != NULL ){
		if(entry->d_type == DT_REG){
			remove(getPath(commitFPath, entry->d_name));
		}
	}
	closedir(dir);

	printf("server196\n");

	char* manPath = getPath(dataList->PROJECTNAME, MANIFEST);
	struct manifestNode* mList = parseManifest(readFileData(manPath));

	printf("server201\n");

	//remove all deleted commits from list of commits
	struct manifestNode* cList = parseManifest(dataList->FIRSTFILENODE->content);
	printf("server205\n");
	struct manifestNode* cPtr = cList->next;
	printf("server207\n");
	while( cPtr != NULL ){
		printf("server209\n");
		printf("cPtr->code: %s\n", cPtr->code);
		//remove all deleted commits from list of commits
		if(strcmp(cPtr->code, "deleted")==0){
			printf("server212");
			struct manifestNode* mNode = findFile(cPtr->path ,mList);
			mNode->code = "deleted";
			remove(cPtr->path);
		}
		cPtr = cPtr->next;
	}

	printf("server216\n");

	//create/rewrite all the files sent
	struct node* ptr = dataList->FIRSTFILENODE->next;
	while( ptr != NULL ){
		printf("212path: %s\n", ptr->name);
		//printf("209content %s\n", ptr->content);

		int fd = open( ptr->name, O_WRONLY|O_CREAT|O_TRUNC, 0666 );
		if( fd<0 ){
			printf("Error: server198push");
		}

		struct manifestNode* cNode = findFile(ptr->name, cList);
		struct manifestNode* mNode = findFile(ptr->name, mList);
		if( mNode == NULL ){
			printf("222added: %s\n", cNode->path);
			struct manifestNode* addThis = (struct manifestNode*)malloc(1*sizeof(struct manifestNode));
			addThis->code = "uptodate";
			addThis->version = cNode->version;
			addThis->path = cNode->path;
			addThis->hash = cNode->hash;
			addThis->next = mList->next;
			mList->next = addThis;
		}else{
			mNode->code = "uptodate";
			mNode->version = cNode->version;
			mNode->hash = cNode->hash;
		}

		if( ptr->content != NULL ){
			write(fd, ptr->content, strlen(ptr->content));
		}
		close(fd);
		ptr=ptr->next;
	}

	//printf("curVersion: %d\n", mList->version);
	//remove(manPath);
	newVersionFile( ++(mList->version), manPath);
	mList = mList->next;
	while( mList != NULL ){
		if( strcmp(mList->code, "deleted") != 0 ){
			writeToVersionFile(manPath, mList->code, mList->version, mList->path, mList->hash);
			mList = mList->next;
		}
	}

	sendData(newsockfd, versionData("push", dataList->PROJECTNAME, manPath));
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
	char* projectpath =  getPath(".", projectname);
	createDir(projectpath);
	createDir(getPath(projectpath, ARCHIVE));
	createDir(getPath(projectpath, COMMIT));
	char* manifestPath = getPath(projectpath, MANIFEST);
	newVersionFile(1, manifestPath);

	//writeToVersionFile(manifestPath, "uptodate", 1, manifestPath, generateHash(""));

	sendData(newsockfd, versionData("create",projectname, manifestPath));
}

void serverUpgrade(struct node* dataList){
	int exists = dirExists(dataList->PROJECTNAME);
	if( exists == 0 ){
		char* data = appendData("upgrade", "Error");
		sendData(newsockfd, data);
		return;
	}

	struct manifestNode* uList = parseManifest(dataList->FIRSTFILENODE->content);
	printf("server301\n");

	char* data = NULL; int count = 0;
	uList = uList->next;
	while( uList != NULL ){
		printf("path: %s\n", uList->path);
		if( uList->code != "D" ){
			data = appendFileData(data, uList->path);
			count++;
		}
		uList = uList->next;
	}
	printf("filecount: %d\n", count);
	data = appendData(dataHeader("upgrade", "ProjectFileContent", dataList->PROJECTNAME, count), data);
	sendData(newsockfd, data);
}
