#include "WTFheader.h"

/*
	TODO: MUTEX LOCK
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

				struct node* dataList = receiveData(newsockfd);

				executeCommand(dataList);
			}
		}
	}
	return 0;
}

void executeCommand(struct node* dataList){

	char* command = dataList->name;

	if(strcmp(command, "checkout")==0){
    		serverCheckout(dataList->PROJECTNAME);
	}else if(strcmp(command, "update")==0){
		serverSendManifest(dataList);
	}else if(strcmp(command, "upgrade")==0){
		serverUpgrade(dataList);
	}else if(strcmp(command, "commit")==0){
		serverCommit(dataList);
	}else if(strcmp(command, "push")==0){
		serverPush(dataList);
	}else if(strcmp(command, "create")==0){
		serverCreate(dataList);
	}else if(strcmp(command, "destroy")==0){
		serverDestroy(dataList->PROJECTNAME);
	}else if(strcmp(command, "currentversion")==0){

	}else if(strcmp(command, "history")==0){

	}else if(strcmp(command, "rollback")==0){

	}else{
		//should never happen
	}
}

void serverDestroy(char* projectname) {
	//TODO:Lock repository when called
	
	char* path = getPath(".", projectname);

	struct dirent *file;
	DIR *direc = opendir(path);
	if (direc == NULL){ //Fails - if project not on server
		sendData(newsockfd, makeMsg("destroy", "Error", "Project not on server"));
		return;
	}
	
	while ((file = readdir(direc)) != NULL) {
		printf("%s\n", file->d_name);
			char * filepath = getPath(path,file->d_name);
			printf("%s\n",filepath);
			if ( remove(filepath)==0) {
				printf("file removed\n");
			} else {
				printf("remove failed\n");
			}
	}
	closedir(direc);
	
	sendData(newsockfd, appendData("destroy", "Success"));
}

void serverCheckout(char* projectname) {
	char* projectPath = getPath(".", projectname);
	
	//Fails if project not on server
	if( dirExists(projectPath) == 0 ){
		sendData(newsockfd, makeMsg("checkout", "Error", "Project not on server"));
		return;
	}

	//Read in server's manifest data
	char * manPath = getPath(projectPath,MANIFEST);
	char * manData = readFileData(manPath);
	struct manifestNode * manList = parseManifest(manData);


	//make data to send to client
	struct manifestNode* ptr = manList->next;
	char * fileList = "";
	char * contentList = "";
	char* data = NULL;
	int count = 1;
	data = appendFileData(data, manPath); //adds manifest to data
	while( ptr != NULL ){
		data = appendFileData(data, ptr->path); //adds files to data
		ptr = ptr->next;
		count++;
	}
	data = appendData(dataHeader("checkout", "ProjectFileContent", projectname, count), data);
	
	sendData(newsockfd, data); //Sends data to client
}

void serverCommit(struct node* dataList){
	char* projectPath = getPath(".", dataList->PROJECTNAME);

	//sends manifest, fails if project or manifest not found
	if(serverSendManifest(dataList)==0){ 
		return; 
	}
	
	dataList = receiveData(newsockfd); //gets the new commit data
	//count num of active commits in .commit folder
	char* commitFolderPath = getPath(projectPath, COMMIT);
	int countFiles = 1;
	DIR* dir = opendir(commitFolderPath);
	struct dirent* entry;
	while((entry=readdir(dir)) != NULL ){
		if(entry->d_type == DT_REG){
			countFiles++;
		}
	}
	closedir(dir);
	
	//create a new commit in .commit folder
	int commitFD = createFile(getPath(commitFolderPath, int2str(countFiles)));
	char* writeout = dataList->FIRSTFILENODE->content;
	write(commitFD, writeout, strlen(writeout));
	close(commitFD);
	
	//sends success signal to client
	sendData(newsockfd,makeMsg("checkout", "Success", "success")); 
}

void serverPush(struct node* dataList){

	char* projectPath = getPath(".", dataList->PROJECTNAME);
	
	//Fails if project not on server
	if( dirExists(projectPath) == 0 ){
		sendData(newsockfd, makeMsg("push", "Error", "Project not on server"));
		return;
	}
	
	char* commitFPath = getPath(projectPath, COMMIT);
	
	//tries to find matching commit
	int commitFound = 0;
	DIR* dir = opendir(commitFPath);
	struct dirent* entry;
	while((entry=readdir(dir)) != NULL ){
		if(entry->d_type == DT_REG){
			if( strcmp(dataList->FIRSTFILENODE->content,
					readFileData(getPath(commitFPath, entry->d_name))) == 0 ){
				commitFound = 1;
			}
		}
	}
	closedir(dir);
	if( commitFound == 0 ){ //if matching commit not found
		sendData(newsockfd, makeMsg("push", "Error", "Commit before pushing"));
		return;
	}
	//remove all commits
	dir = opendir(commitFPath);
	while((entry=readdir(dir)) != NULL ){
		if(entry->d_type == DT_REG){
			remove(getPath(commitFPath, entry->d_name));
		}
	}
	closedir(dir);

	//read in manifest
	char* manPath = getPath(projectPath, MANIFEST);
	struct manifestNode* mList = parseManifest(readFileData(manPath));
	int versionNum = mList->version;
	
	//copy project to temporary ./.projectname on server
	char* tempPath = getPath(".", append(".", dataList->PROJECTNAME));
	createDir(tempPath);
	copydir(projectPath, tempPath);
	
	
	//copy tempCpy to .version in project dir
	char* copyPath = getPath(projectPath, getPath(ARCHIVE, int2str(versionNum)));
	createDir(copyPath);
	copydir(tempPath, copyPath);
	//TODO: RECURSIVE REMOVE
	
	//remove all deleted commits from list of commits
	struct manifestNode* cList = parseManifest(dataList->FIRSTFILENODE->content);
	struct manifestNode* cPtr = cList->next;
	while( cPtr != NULL ){
		//remove all deleted commits from list of commits
		if(strcmp(cPtr->code, "deleted")==0){
			struct manifestNode* mNode = findFile(cPtr->path ,mList);
			mNode->code = "deleted";
			remove(cPtr->path);
		}
		cPtr = cPtr->next;
	}

	//create/rewrite all the files sent
	struct node* ptr = dataList->FIRSTFILENODE->next;
	while( ptr != NULL ){
		int fd = open( ptr->name, O_WRONLY|O_CREAT|O_TRUNC, 0666 );
		if( fd<0 ){ //can't open, have to create dirs then retry
			printf("Creating: %s\n", ptr->name);
			char* tempPath = (char*)malloc((strlen(ptr->name)+1)*sizeof(char));
			strcpy(tempPath, ptr->name);
			createSubdir(tempPath);
			fd = open( ptr->name, O_WRONLY|O_CREAT|O_TRUNC, 0666 );
			if( fd<0){ //should never happen
				sendData(newsockfd,makeMsg("push", "Error", "Push failed"));
				return;
			}
		}
		
		struct manifestNode* cNode = findFile(ptr->name, cList);
		struct manifestNode* mNode = findFile(ptr->name, mList);
		if( mNode == NULL ){
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
//checks if project exists
//checks if file exists
int serverSendManifest(struct node* dataList){
	char* projectname = dataList->PROJECTNAME;
	char* projectPath = getPath(".", projectname);
	
	//Checks if project exists
	if( dirExists(projectPath) == 0 ){
		sendData(newsockfd, makeMsg(dataList->name, "Error", "Project not on server"));
		return 0; //unsuccessful
	}
	
	//Checks if manifest exists
	char* manPath = getPath(projectPath, MANIFEST);
	if( fileExists(manPath) == 0 ){
		sendData(newsockfd, makeMsg(dataList->name, "Error", "Manifest not found on server"));
		return 0;
	}
	
	//sends manifest
	char* data = versionData(dataList->name,projectname, manPath);
	sendData(newsockfd, data);
	return 1;
}


//Create project and manifests, sends manifest to client
void serverCreate(struct node* dataList){
	char* projectname = dataList->PROJECTNAME;
	char* projectpath = getPath(".", projectname);
	
	//fails if project already exists
	if( dirExists(projectpath)== 1 ){
		sendData(newsockfd, makeMsg("create", "Error", "Project already exists on server"));
		return;
	}
	
	//initializes project on server
	createDir(projectpath);
	createDir(getPath(projectpath, ARCHIVE));
	createDir(getPath(projectpath, COMMIT));
	char* manifestPath = getPath(projectpath, MANIFEST);
	newVersionFile(1, manifestPath);

	//Sends initialized manifest to client
	sendData(newsockfd, versionData("create",projectname, manifestPath));
}


void serverUpgrade(struct node* dataList){
	//fails if project doesnt exist
	if( dirExists(getPath(".", dataList->PROJECTNAME)) == 0 ){
		sendData(newsockfd, makeMsg("upgrade", "Error", "Project not on server"));
		return;
	}
	
	//read in .update sent from server
	struct manifestNode* uList = parseManifest(dataList->FIRSTFILENODE->content);
	char* data = NULL; int count = 0;
	uList = uList->next;
	
	//makes data to be sent of all files to be added/updated
	while( uList != NULL ){
		printf("path: %s\n", uList->path);
		if( uList->code != "D" ){
			data = appendFileData(data, uList->path);
			count++;
		}
		uList = uList->next;
	}
	//Sends the data to client
	data = appendData(dataHeader("upgrade", "ProjectFileContent", dataList->PROJECTNAME, count), data);
	sendData(newsockfd, data);
}
