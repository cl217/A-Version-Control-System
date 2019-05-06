#include "WTFheader.h"

/*
	**3.0 configure -done
	3.1 checkout - done
	**3.2 update -done
	**3.3 upgrade -done
	**3.4 commit -done
	**3.5 push -done
	**3.6 create -done
	3.7 destroy  -done
	**3.8 add -done
	3.9 remove -done
	3.10 currentversion -done
	3.11 history- done
	3.12 rollback -done 

*/



int isExit = 0;
int sockfd = -1;
struct filenode* filelist;


void exitSignalHandler( int sig_num ){
	close(sockfd);
	printf("Client disconnected from server.\n");
	exit(1);
}

void exitHandler(){
	if( sockfd > 0 ){
		close(sockfd);
		printf("Client disconnected from server\n");
	}
	exit(1);
}

int main( int argc, char** argv ){
 	//Ctrl-C handler, closes ports before ending program
	signal(SIGINT, exitSignalHandler);

	if( argc < 2 ){
		printf("Invalid arguments\n"); 
		printf("Usage: ./WTF <command>\n");
		return 0;
	}

	//  ./WTF randomargv2 for client-server connection test
	if(strcmp(argv[1], "configure")==0){ // ./WTF configure <IP> <port>
		if(argc != 4){
			printf("Invalid arguments\n"); 
			printf("Usage: ./WTF <command> <IP> <port>\n");
			return 0;
		}else{
			wtfconfigure(argv[2], argv[3]);
		}
	}else if(strcmp(argv[1], "checkout")==0){
		if(argc != 3){
			printf("Invalid arguments\n"); 
			printf("Usage: ./WTF <command> <projectname>\n");
			return 0;		
		}else{
			wtfcheckout(argv[2]);
		}
	}else if(strcmp(argv[1], "update")==0){
		if(argc != 3){
			printf("Invalid arguments\n"); 
			printf("Usage: ./WTF <command> <projectname>\n");
			return 0;		
		}else{
			wtfupdate(argv[2]);
		}
	}else if(strcmp(argv[1], "upgrade")==0){
		if(argc != 3){
			printf("Invalid arguments\n"); 
			printf("Usage: ./WTF <command> <projectname>\n");
			return 0;
		}else{
			wtfupgrade(argv[2]);
		}
	}else if(strcmp(argv[1], "commit")==0){
		if(argc != 3){
			printf("Invalid arguments\n"); 
			printf("Usage: ./WTF <command> <projectname>\n");
			return 0;
		}else{
			wtfcommit(argv[2]);
		}
	}else if(strcmp(argv[1], "push")==0){
		if(argc != 3){
			printf("Invalid arguments\n"); 
			printf("Usage: ./WTF <command> <projectname>\n");
			return 0;
		}else{
			wtfpush(argv[2]);
		}
	}else if(strcmp(argv[1], "create")==0){
		if(argc != 3){
			printf("Invalid arguments\n"); 
			printf("Usage: ./WTF <command> <projectname>\n");
			return 0;
		}else{
			wtfcreate(argv[2]);
		}
	}else if(strcmp(argv[1], "destroy")==0){
		if(argc != 3){
			printf("Invalid arguments\n"); 
			printf("Usage: ./WTF <command> <projectname>\n");
			return 0;
		}else{
			wtfdestroy(argv[2]);
		}
	}else if(strcmp(argv[1], "add")==0){
		if(argc != 4){
			printf("Invalid arguments\n"); 
			printf("Usage: ./WTF <command> <projectname> <file>\n");
			return 0;
		}else{
			wtfadd(argv[2], argv[3]);
		}
	}else if(strcmp(argv[1], "remove")==0){
		if(argc != 4){
			printf("Invalid arguments\n"); 
			printf("Usage: ./WTF <command> <projectname> <file>\n");
			return 0;
		}else{
			wtfremove(argv[2], argv[3]);
		}
	}else if(strcmp(argv[1], "currentversion")==0){
		if(argc != 3){
			printf("Invalid arguments\n"); 
			printf("Usage: ./WTF <command> <projectname>\n");
			return 0;
		}else{
			wtfcurrentversion(argv[2]);
		}
	}else if(strcmp(argv[1], "history")==0){
		if(argc != 3){
			printf("Invalid arguments\n"); 
			printf("Usage: ./WTF <command> <projectname>\n");
			return 0;
		}else{
			wtfhistory(argv[2]);
		}
	}else if(strcmp(argv[1], "rollback")==0){
		if(argc != 4){
			printf("Invalid arguments\n"); 
			printf("Usage: ./WTF <command> <projectname> <version>\n");
			return 0;
		}else{
			wtfrollback(argv[2], argv[3]);
		}
	}else{
		printf("Invalid command\n");
	}
	if(sockfd > 0 ){
		exitHandler();
	}
	return 0;
}

/** Connect function for all commands that communicates with server
	Shuts program down if can't connect
**/
void wtfconnect(){
	//open to read from .configure file
	int configFile = open("./.configure", O_RDONLY);
	if( configFile == -1 ){
		printf("Error: no .configure found\n"); exitHandler();
	}

	char* c = (char*)malloc(2*sizeof(char));
	char* word = NULL;
	//READ IN IP
	while( read(configFile, c, 1) > 0 && c[0] != '\n' ){
		c[1] = '\0';
		word = append(word, c);
	}
	struct hostent* server = gethostbyname(word);
	//READ IN PORT
	word = NULL;
	while( read(configFile, c, 1) > 0 ){
		c[1] = '\0';
		word = append(word, c);
	}
	close(configFile);

	int port = atoi(word);

	//CREATE CLIENT
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd<0){
		printf("Error: can't open socket\n"); exitHandler();
	}

	//FIND HOST
	if(server==NULL){
		printf("Error: IP not found\n"); exitHandler();
	}

	//Connection setup
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	char* tempstr = (char*)malloc(sizeof(&server->h_addr_list[0])+1);
	memcpy(&serverAddr.sin_addr.s_addr, server->h_addr_list[0], sizeof(server->h_addr_list[0]));
	serverAddr.sin_port = htons(port);

	//CONNECT TO SERVER
	int connected = 0;
	while( connected == 0 ){
		if(connect(sockfd, (struct sockaddr*) &serverAddr, sizeof(serverAddr))>=0){
			printf("Status: Connected to server\n");
			connected = 1;
		}else{
			sleep(3);
			printf("Status: Attempting to connect to server...\n");
		}
	}
}


/**
	Saves ip and port to .configure file
**/
void wtfconfigure( char* ip, char* port){
	int file = open( "./.configure", O_CREAT | O_WRONLY | O_TRUNC, 0777 );
	if( file < 0 ){
		printf("Error: .configure file creation failed\n"); exitHandler();
	}
	write(file, ip, strlen(ip));
	write(file, "\n", 1);
	write(file, port, strlen(port));
	close(file);
	printf("Status: configure successful\n");
}


/**
	Creates project+intiates manifest on server, copies manifest to client
	Creates project folder on client if doesnt already exist
	Fails if:
		-Project already on server
**/
void wtfcreate( char* projectname ){

	wtfconnect(); //connects to server

	sendCommandProject(sockfd, "create", projectname); //sends data to server
	struct node* dataList = receiveData(sockfd); //receives data from server

	if( strcmp(dataList->next->name, "Error")==0 ){ //project already exists
		printf("Error: %s\n", dataList->next->content);
		exitHandler();
	}

	//creates project folder if doesn't already exist
	char* projectPath = getPath(".", dataList->PROJECTNAME);
	if( dirExists(projectPath) == 0 ){
		createDir(projectPath);
	}

	//copies manifest to client
	struct node* manNode = dataList->FIRSTFILENODE; //get node of manifest data
	int manifestFD = open(manNode->name, O_WRONLY|O_CREAT|O_APPEND, 0666);
	if(manifestFD<0){
		printf("Error: creating manifest\n"); exitHandler();
	}
	write(manifestFD, manNode->content, strlen(manNode->content));
	close(manifestFD);

	printf("Status: create successful\n");
}


/*
	Destroys project folder + all subdirectories and files on server
	Keeps everything on client
	Fails if:
		Project not on server
*/
void wtfdestroy( char* projectname ){

	wtfconnect(); //shuts down program if cant connect

	sendCommandProject(sockfd, "destroy", projectname);

	//Error check, project not on server
	struct node* dataList = receiveData(sockfd);
	if( strcmp(dataList->next->name, "Error")==0 ){
		printf("Error: %s\n", dataList->next->content);
		exitHandler();
	}

	printf("Status:project succesfully destroyed on server\n");
}

/*
	Downloads latest copy of project onto client
	Fails if:
		-Not on server
		-Already on client
*/
void wtfcheckout( char* projectname ){

	char* projectPath = getPath(".", projectname);

	//Error check- project already on client
	if(dirExists(projectPath)!=0){
		printf("Error: Project already exists locally\n"); exitHandler();
	}
	wtfconnect(); //shuts down program if cant connect

	//Sends and receives data from server
	sendCommandProject(sockfd, "checkout", projectname);
	struct node* dataList = receiveData(sockfd);

	//Errorcheck - project not on server
	if( strcmp(dataList->next->name, "Error")==0 ){
		printf("Error: %s\n", dataList->next->content);
		exitHandler();
	}

	//create project directory
	createDir(projectPath);
	char* manPath = getPath(projectPath, MANIFEST);

	//get manifest node data from server - will be the first file node in the dataList
	char* mData = dataList->FIRSTFILENODE->content;

	//write client's manifest
	int manFD = open(manPath, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	if(manFD<0){
		printf("Status: Error opening manifest\n");
		return;
	}
	write(manFD, mData, strlen(mData));
	close(manFD);

	//write each file to client's project
	struct node * fptr = dataList->FIRSTFILENODE->next;
	while (fptr != NULL) {
		int fd = open( fptr->name, O_WRONLY|O_CREAT|O_TRUNC, 0666 );
		//can't open, have to create nonexisting subdirs then retry
		if( fd<0 ){
			char* tempPath = (char*)malloc((strlen(fptr->name)+1)*sizeof(char));
			strcpy(tempPath, fptr->name);
			createSubdir(tempPath);
			fd = open( fptr->name, O_WRONLY|O_CREAT|O_TRUNC, 0666 ); //retry opening file
			if( fd<0){ //should not happen
				printf("Error creating: %s\n", fptr->name);
				return;
			}
		}
		if( fptr->content != NULL ){
			write(fd, fptr->content, strlen(fptr->content));
		}
		close(fd);
		fptr = fptr->next;
	}

	printf("Status: checkout successful\n");
}

/**
	Adds file to client manifest
**/
void wtfadd( char* projectname, char* filename ){
	char* projectPath =  getPath(".", projectname);
	//Error checks
	if( dirExists(projectPath) == 0 ){ //project does not exist
		printf("Error: Project does not exist\n"); exitHandler();
	}
	char* filePath = getPath(projectPath, filename);
	char* fileText = readFileData(filePath);
	if( fileText == NULL ){ //file does not exist
		exitHandler();
	}

	//Get manifest
	char* manifestPath = getPath(projectPath, MANIFEST);
	struct manifestNode* mList = parseManifest(readFileData(manifestPath));
	if( mList == NULL ){ //error check- manifest does not exist
		exitHandler();
	}

	struct manifestNode* mNode = findFile(filePath, mList);
	if( mNode != NULL ){ //If file already in manifest
		//rewrite manifest, updating hash of file
		newVersionFile(mList->version, manifestPath);
		mList=mList->next;
		int curVer = 0;
		while(mList!=NULL){
			if(strcmp(mList->path, filePath)!=0){
				writeToVersionFile(manifestPath, mList->code, mList->version, mList->path, mList->hash);
			}else{
				curVer = mList->version;
			}
			mList=mList->next;
		}
		writeToVersionFile(manifestPath, "upload", curVer, filePath, generateHash(fileText));
		printf("Warning: file already exists, hash has been updated\n");
	}else{ //File not already in manifest
		//add file to end of manifest
		writeToVersionFile(manifestPath, "upload", 0, filePath, generateHash(fileText));
		printf("Status: add successful\n");
	}
}



/*
	Removes targeted file from the client side manifest
*/
void wtfremove(char * projectname, char * filename) {

	char* projectPath = getPath(".", projectname);
	char * manPath = getPath(projectPath, MANIFEST);
	char * filePath = getPath(projectPath,filename);
	if(dirExists(projectPath)==0){
		printf("Error: Project does not exist\n"); exitHandler();
	}

	char * manData = readFileData(manPath);
	struct manifestNode * manList = parseManifest(manData);

	struct manifestNode * prev = manList;
	struct manifestNode * ptr = manList->next;

	//remove manifest and make a new manifest
	remove(manPath);
	newVersionFile(manList->version, manPath);

	//write to new manifest, skip file to be removed

	int found = 0;
	while (ptr != NULL) {
		if (strcmp(ptr->path,filePath) != 0 ) {
			writeToVersionFile(manPath,ptr->code,ptr->version,ptr->path, ptr->hash);
		}else{
			found = 1;
		}
		ptr = ptr->next;
	}

	if( found == 0 ){
		printf("Error: file is not on manifest\n");
	}else{
		printf("Status: file succesfully removed from manifest\n");
	}
}


/**
	Compares server and client's manifest
	Documents file changes that the server is ahead on
**/
void wtfupdate( char* projectname ){

	char* projectPath = getPath(".", projectname);

	//Errorcheck-project not on client
	if(dirExists(projectPath)==0){
		printf("Error: Project does not exist locally\n"); exitHandler();
	}

	wtfconnect(); //connects, shuts down if can't

	//send and receive data
	sendCommandProject(sockfd, "update", projectname);
	struct node* dataList = receiveData(sockfd);

	//Errorcheck- project not on server
	if( strcmp(dataList->next->name, "Error")==0 ){
		printf("Error: %s\n", dataList->next->content);
		exitHandler();
	}

	char* updatePath = getPath(projectPath, UPDATE);
	char* manPath = getPath(projectPath, MANIFEST);

	//read in server and client's manifiest
	struct manifestNode* sMan = parseManifest(dataList->FIRSTFILENODE->content);
	struct manifestNode* cMan = parseManifest(readFileData(manPath));
	int sManVer = sMan->version;
	int cManVer = cMan->version;

	//creates .updatefile
	newVersionFile(sMan->version, updatePath);


	struct manifestNode* sPtr = sMan->next; //server's manifest
	struct manifestNode* cPtr = cMan->next; //client's manifest
	int existConflict = 0;
	int isEmpty = 1;

	while( cPtr != NULL ){ //traverses all files in client's manifest
		char* liveHash =  generateHash(readFileData(cPtr->path));
		struct manifestNode* sNode = findFile(cPtr->path,sMan);
		if( sManVer==cManVer &&
					( sNode==NULL || strcmp(sNode->hash,liveHash)!=0 ) ){
			//U-Not in sMan | in both but sMan hash != cMan live hash | sManVer = cManVer
			printf("U %s\n", cPtr->path);
		}else if( sNode!=NULL && sNode->version!=cPtr->version
					&& sManVer!=cManVer && strcmp(cPtr->hash,liveHash)==0 ){
			//M-in both sMan+cMan, s's man+file version!=c's | c's live hash=cMan hash
			printf("M %s\n", cPtr->path);
			writeToVersionFile(updatePath, "M", sNode->version, sNode->path, sNode->hash);
			isEmpty = 0;
		}else if( sNode==NULL && sManVer!=cManVer ){
			//D- Not in sMan, sManVer != cManVer
			printf("D %s\n", cPtr->path);
			writeToVersionFile(updatePath, "D", cPtr->version, cPtr->path, cPtr->hash);
			isEmpty = 0;
		}else if( sNode!= NULL && cManVer != sManVer && sNode->version!=cPtr->version
			&& strcmp(cPtr->hash,sNode->hash)!=0 && strcmp(liveHash,sNode->hash)!=0 ){
			//Conflict
			existConflict = 1;
			printf("CONFLICT %s\n", cPtr->path);
		}
		cPtr = cPtr->next;
	}

	//A - in sMan and not cMan, sMan version != cMan version
	while( sPtr != NULL ){ //traverses all files in server's manifest
		struct manifestNode* cNode = findFile(sPtr->path, cMan);
		if( cNode == NULL && sManVer != cManVer ){
			printf("A %s\n", sPtr->path);
			writeToVersionFile(updatePath, "A", sPtr->version, sPtr->path, sPtr->hash);
			isEmpty = 0;
		}
		sPtr = sPtr->next;
	}

	if( existConflict == 1 ){
		printf("Error: can't update, fix conflicts and try again\n");
		remove(updatePath);
		return;
	}
	if( isEmpty == 1 ){
		printf("Error: Project is already up to date\n");
		close(createFile(updatePath));
		return;
	}

	printf("Status: update successful\n");
}


/**
	Applies changes in .update file to
	bring client version of project up to date with servers
**/
void wtfupgrade( char* projectname ){

	char* projectPath = getPath(".", projectname);

	//Errorcheck-project not on client
	if(dirExists(projectPath)==0){
		printf("Error: Project does not exist\n"); exitHandler();
	}

	//get paths of .update and .manifest
	char* updatePath = getPath(projectPath, UPDATE);
	char* manPath = getPath(projectPath, MANIFEST);

	//Errorcheck- no .update file
	if( fileExists(updatePath) == 0 ){
		printf("Error: No update file found. Update before upgrading.\n");
		exitHandler();
	}
	//Errorcheck- empty .update file
	struct stat updateStat;
	stat(getPath(projectPath, UPDATE), &updateStat);
	if(updateStat.st_size == 0){
		printf("Status: Project already up to date\n");
		remove(getPath(projectPath, UPDATE));
		exitHandler();
	}

	wtfconnect(); //connects, shuts down if can't

	//sends the update list to server
	sendData(sockfd, versionData("upgrade", projectname, updatePath));
	struct node* dataList = receiveData(sockfd); //receives data from server

	//Errorcheck-project not on server
	if( strcmp(dataList->next->name, "Error")==0 ){
		printf("Error: %s\n", dataList->next->content);
		exitHandler();
	}
	//read in client's manifest and update file
	struct manifestNode* mList = parseManifest(readFileData(manPath));
	struct manifestNode* uList = parseManifest(readFileData(updatePath));
	//delete all files labeled as deleted on .update
	struct manifestNode* uPtr = uList->next;
	while( uPtr != NULL ){
		if( strcmp(uPtr->code, "D") == 0 ){
			//also remove from mList
			struct manifestNode * mNode = findFile( uPtr->path, mList );
			mNode->code = "deleted";
			remove(uPtr->path);
		}
		uPtr = uPtr->next;
	}
	if( strcmp(dataList->next->name, "Success")!=0 ){ //if not already done
		//create/rewrite all files received from server
		struct node* ptr = dataList->FIRSTFILENODE;
		while( ptr != NULL ){
			int fd = open( ptr->name, O_WRONLY|O_CREAT|O_TRUNC, 0666 );
			if( fd<0 ){ //can't open, file in subdirectories that havent been created
				char* tempPath = (char*)malloc((strlen(ptr->name)+1)*sizeof(char));
				strcpy(tempPath, ptr->name);
				createSubdir(tempPath); //create subdirectories
				fd = open( ptr->name, O_WRONLY|O_CREAT|O_TRUNC, 0666 ); //retry opening file
				if( fd<0){ //should not happen
					printf("Error creating: %s\n", ptr->name);
					return;
				}
			}
			//update manifest
			struct manifestNode* uNode = findFile(ptr->name, uList);
			struct manifestNode* mNode = findFile(ptr->name, mList);
			if( mNode == NULL ){ //if new file to be created
				struct manifestNode* addThis = (struct manifestNode*)malloc(1*sizeof(struct manifestNode));
				addThis->code = "uptodate";
				addThis->version = uNode->version;
				addThis->path = uNode->path;
				addThis->hash = uNode->hash;
				addThis->next = mList->next;
				mList->next = addThis;
			}else{ //if existing file to be updates
				mNode->code = "uptodate";
				mNode->version = uNode->version;
				mNode->hash = uNode->hash;
			}

			if( ptr->content != NULL ){ //write file contents if not empty
				write(fd, ptr->content, strlen(ptr->content));
			}
			close(fd);
			ptr=ptr->next;
		}
	}
	//write out updates manifest
	newVersionFile( uList->version, manPath);
	mList = mList->next;
	while( mList != NULL ){
		if( strcmp(mList->code, "deleted") != 0 ){
			writeToVersionFile(manPath, mList->code, mList->version, mList->path, mList->hash);
		}
		mList = mList->next;
	}
	remove(getPath(projectPath, UPDATE)); //removes .update
	printf("Status: upgrade successful\n");
}

/**
	Compares client and server's manifest
	documents file changes that the client is ahead on
**/
void wtfcommit( char* projectname ){

	char* projectPath = getPath(".", projectname);

	//Error check - project not on client
	if( dirExists(projectPath) == 0 ){
		printf("Error: Project does not exist\n"); exitHandler();
	}

	wtfconnect(); //Connects to server, shut down if can't

	//Error check - .update not empty
	struct stat updateStat;
	if(stat(getPath(projectPath, UPDATE), &updateStat) > 0 && updateStat.st_size>1){
		printf("Error: Must update before commiting\n"); exitHandler();
	}

	//Sends and receives data from server
	sendCommandProject(sockfd, "commit", projectname);
	struct node* dataList = receiveData(sockfd);

	//Error check - Project not on server or no matching commit
	if( strcmp(dataList->next->name, "Error")==0 ){
		printf("Error: %s\n", dataList->next->content);
		exitHandler();
	}

	//Read in server's and client's manifest
	struct manifestNode* sMan = parseManifest(dataList->FIRSTFILENODE->content);
	struct manifestNode* cMan
				= parseManifest(readFileData(getPath(projectPath, MANIFEST)));

	//Error check- client project not up to date, must upgrade first
	if( cMan->version != sMan->version ){
		printf("Error: local repo not up to date, upgrade before commiting\n");
		exitHandler();
	}

	//creates .commit file
	char* commitPath = getPath(projectPath, COMMIT);
	newVersionFile(1, commitPath);

	//traverses all files in client's manifest
	//writes to commit all files that the client is ahead on
	struct manifestNode* ptr=cMan->next;
	while( ptr != NULL ){
		char* newHash = generateHash(readFileData(ptr->path));
		struct manifestNode* sNode = findFile(ptr->path, sMan);
		if( sNode == NULL || strcmp(sNode->hash, newHash)!=0 ){
			writeToVersionFile(commitPath,"upload",++(ptr->version),ptr->path,newHash);
			ptr->hash = newHash;
		}
		ptr=ptr->next;
	}

	//traverses all files in servers manifest
	//if in server but not client's manifest- writes to .commit that the file is to be deleted
	ptr=sMan->next;
	while( ptr!=NULL ){
		struct manifestNode* cNode = findFile(ptr->path, cMan);
		//if difference- file in both but version sMan>cMan
		if(cNode==NULL){
			writeToVersionFile(commitPath,"deleted",++(ptr->version),ptr->path,ptr->hash);
		}else if(strcmp(cNode->hash, ptr->hash)!=0 && ptr->version > cNode->version){
			printf("Error: Must update before commit\n");
			remove(commitPath);
			exitHandler();
		}
		ptr=ptr->next;
	}
	//send commit to server
	sendData(sockfd, versionData("commit", projectname, commitPath));
	dataList = receiveData(sockfd);
	if( strcmp(dataList->next->name, "Success")==0 ){
		printf("Status: %s\n", dataList->next->content);
	}
}

/**
	Applies all changes in .commit to server's version of project
**/
void wtfpush( char* projectname ){

	char* projectPath = getPath(".", projectname);

	//Errorcheck- project not on client
	if( dirExists(projectPath) == 0 ){
		printf("Error: Project does not exist\n"); exitHandler();
	}
	//Error check -.commit file does not exist, must commit first
	char* commitPath = getPath(projectPath, COMMIT);
	int commitFD = open( commitPath, O_RDONLY );
	if(commitFD < 0 ){
		printf("Error: No commits exist, commit before pushing\n");
		exitHandler();
	}
	close(commitFD);
	//Error check - .update file exists and there are M codes
	struct stat updateStat;
	if(stat(getPath(projectPath, UPDATE), &updateStat) > 0 && updateStat.st_size>1){
		struct manifestNode* uList = parseManifest(readFileData(getPath(projectPath, UPDATE)));
		uList = uList->next;
		while( uList != NULL ){
			if(strcmp(uList->code, "M")==0){
				printf("Error: Modified files exist. Upgrade before pushing"); exitHandler();
			}
		}
	}

	wtfconnect(); //connects to server, shuts down if can't

	//makes data to be send to server
	struct manifestNode* cList = parseManifest(readFileData(commitPath));
	cList = cList->next;
	char* data = NULL; int count = 1;
	data = appendFileData(data, commitPath); //adds .commit to data
	while( cList != NULL ){ //adds all files to be updates to data
		data = appendFileData(data, cList->path);
		cList = cList->next;
		count++;
	}
	data = appendData(dataHeader("push", "ProjectFileContent", projectname, count), data);
	sendData(sockfd, data); //send the data to server
	struct node* dataList = receiveData(sockfd); //receives data from server

	//Error check- error signal received bc project not on server
	if( strcmp(dataList->next->name, "Error")==0 ){
		printf("Error: %s\n", dataList->next->content);
		exitHandler();
	}

	//update client's manifest
	int manFD = open(getPath(projectPath, MANIFEST), O_WRONLY|O_TRUNC|O_CREAT, 0666);
	if( manFD < 0 ){ //should never happen
		printf("Status: Error updating manifest\n"); exitHandler();
	}
	write(manFD, dataList->FIRSTFILENODE->content, strlen(dataList->FIRSTFILENODE->content));
	close(manFD);

	remove(commitPath);

	printf("Status: push successful\n");
}

//	4.1
void wtfcurrentversion( char* projectname ){
	char* projectPath = getPath(".", projectname);

	wtfconnect(); //connects, shuts down if can't
	char * manPath = getPath(projectPath,MANIFEST);

	//send and receive data
	sendCommandProject(sockfd, "currentversion", projectname);
	struct node* data = receiveData(sockfd);

	if( strcmp(data->next->name, "Error")==0 ){
		printf("Error: %s\n", data->next->content);
		exitHandler();
	}

	struct manifestNode * manList = parseManifest(data->FIRSTFILENODE->content);
	struct manifestNode * ptr = manList->next;

	while (ptr!=NULL) {
		char * file = append("File: ",ptr->path);
		char * version = append("\tVersion: ", int2str(ptr->version));
		char * line = append(file,version);
		printf("%s\n", line);
		ptr = ptr->next;
	}
}

//	4.1
void wtfhistory( char* projectname ){
	wtfconnect(); //shuts down program if cant connect

	sendCommandProject(sockfd, "history", projectname);
	struct node* dataList = receiveData(sockfd);

	if( strcmp(dataList->next->name, "Error")==0 ){
		printf("Error: %s\n", dataList->next->content);
		exitHandler();
	}
	char * projectpath = getPath(".",projectname);
	char * historyPath = getPath(projectpath, HISTORY);

	printf("History:\n%s\n", dataList->FIRSTFILENODE->content);

}

//	4.1
/*
	Find the version tar file in ./projectname/archive
	Use system("tar xvzf <thetarfile>") to extract tar
	Will be extracted to .projectname directory
	Delete current projectname directory
	Use copydir() to rename .projectname to projectname

*/
void wtfrollback( char* projectname, char* version ){
	wtfconnect(); //shuts down program if cant connect

	char * projectData = appendData(projectname,version); //append version to projectdata

	//send rollback command
	sendCommandProject(sockfd, "rollback", projectData); //1

	struct node* dataList = receiveData(sockfd); //2

	if( strcmp(dataList->next->name, "Error")==0 ){
		printf("Error: %s\n", dataList->next->content);
		exitHandler();
	}
	
	printf("Status: project on server has been rolled back to version %s successfully\n", version);
	//pretty sure only server gets reverted
	/*
	destory local project and checkout servers up to date version
	printf("path: %s\n", getPath(".",projectname));
	destroyRecursive(getPath(".",projectname)); 
	wtfcheckout(projectname);
	*/

}
