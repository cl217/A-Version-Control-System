#include "WTFheader.h"

/*
	**3.0 configure -done
	3.1 checkout
	**3.2 update -done
	**3.3 upgrade -done
	**3.4 commit -done
	**3.5 push -done
	**3.6 create -done
	3.7 destroy
	**3.8 add -done
	3.9 remove
	3.10 currentversion
	3.11 history
	3.12 rollback
*/



int isExit = 0;
int sockfd = -1;
struct filenode* filelist;


void exitSignalHandler( int sig_num ){
	close(sockfd);
	printf("Client socket closed\n");
	exit(1);
}

void exitHandler(){
	if( sockfd > 0 ){
		close(sockfd);
		printf("Client socket closed\n");
	}
	exit(1);
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
			wtfupgrade(argv[2]);
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
	}
	if(sockfd > 0 ){
		exitHandler();
	}
	return 0;
}

/** Connect function for all commands that communicates with server**/
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
	printf("Status: new client created\n");

	//FIND SERVER
	if(server==NULL){
		printf("Error: host not found\n"); exitHandler();
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
		printf("Error: connection failed\n");
		exitHandler();
	}else{
		printf("Status: connected to server\n");
	}
}

//1.0**
void wtfconfigure( char* ip, char* port){
	int file = open( "./.configure", O_CREAT | O_WRONLY | O_TRUNC, 0777 );
	if( file < 0 ){
		printf("Error: .configure file creation failed\n"); exitHandler();
	}
	write(file, ip, strlen(ip));
	write(file, "\n", 1);
	write(file, port, strlen(port));
	close(file);
}

/*
	<command><dataType><bytesPname><projectName>
			<numFile><bytesfName><fName><bytefContent><fContent>..
*/

//	1.1**
void wtfcreate( char* projectname ){
	wtfconnect();

	sendCommandProject(sockfd, "create", projectname);

	struct node* dataList = recieveData(sockfd);

	if( strcmp(dataList->next->name, "Error")==0 ){ //project already exists
		printf("Error: Project already exists on server.\n");
		exitHandler();
	}

	//TODO: could move to a function
	struct node* projectNode = dataList->next->next;
	char* dirPath = getPath(".", projectNode->name);
	createDir(dirPath);
	struct node* fileNode = projectNode->next->next;

	int manifestFD = open(fileNode->name, O_WRONLY|O_CREAT|O_APPEND, 0666);
	if(manifestFD<0){
		printf("error: opening\n");
	}
	write(manifestFD, fileNode->content, strlen(fileNode->content));
	close(manifestFD);
}

//	1.2
void wtfdestroy( char* projectname ){
	printf("wtfdestroy()\n");

	wtfconnect(); //shuts down program if cant connect

	sendCommandProject(sockfd, "destroy", projectname);
}

//	1.2
void wtfcheckout( char* projectname ){
	printf("wtfcheckout()\n");
	if(dirExists(projectname)!=0){
		printf("Error: Project already exists locally\n"); exitHandler();
	}
	wtfconnect(); //shuts down program if cant connect

	sendCommandProject(sockfd, "checkout", projectname);

	struct node* dataList = recieveData(sockfd);
	if( strcmp(dataList->next->name, "Error")==0 ){
		printf("Error: Project does not exist on server\n");
		exitHandler();
	}


	//create project directory
	char* projectpath =  getPath(".", projectname);
	createDir(projectpath);
	char* manifestPath = getPath(projectpath, MANIFEST);
	char * manName = getPath(projectname, MANIFEST);


	//get manifest node data - will be the first file node in the dataList
	//printf("firstnode: %s\n", dataList->FIRSTFILENODE->name);
	char* mData = dataList->FIRSTFILENODE->content;

	//write manifest
	int manFD = open(manName, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	if(manFD<0){
		printf("error1: opening\n");
		return;
	}
	write(manFD, mData, strlen(mData));
	close(manFD);

	//write each file
	struct node * fptr = dataList->FIRSTFILENODE->next;
	while (fptr != NULL) {
		printf("creating: %s\n", fptr->name);
		
		int fd = open( fptr->name, O_WRONLY|O_CREAT|O_TRUNC, 0666 );
		if( fd<0 ){ //can't open, have to create dirs then retry
			printf("Creating: %s\n", fptr->name);
			char* tempPath = (char*)malloc((strlen(fptr->name)+1)*sizeof(char));
			strcpy(tempPath, fptr->name);
			createSubdir(tempPath);
			fd = open( fptr->name, O_WRONLY|O_CREAT|O_TRUNC, 0666 );
			if( fd<0){
				printf("Error creating: %s\n", fptr->name);
				return;
			}else{
				printf("Created: %s\n", fptr->name);
			}
		}
		
		if( fptr->content != NULL ){
			write(fd, fptr->content, strlen(fptr->content));
		}
		close(fd);
		fptr = fptr->next;
	}
}

//	2.1**
//client adds file to own manifest -DONE
void wtfadd( char* projectname, char* filename ){
	printf("wtfadd()\n");
	if( dirExists(projectname) == 0 ){
		printf("Error: Project does not exist\n"); exitHandler();
	}
	char* filePath = getPath(projectname, filename);
	char* fileText = readFileData(filePath);
	if( fileText == NULL ){
		exitHandler();
	}
	char* manifestPath = getPath(projectname, MANIFEST);
	struct manifestNode* mList = parseManifest(readFileData(manifestPath));
	if( mList == NULL ){
		exitHandler();
	}
	struct manifestNode* mNode = findFile(filePath, mList);
	if( mNode != NULL ){
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
	}else{
		writeToVersionFile(manifestPath, "upload", 0, filePath, generateHash(fileText));
		printf("Success: added\n");
	}
}

//	2.2 - remove the file from manifest (tag it as removed?)
void wtfremove( char* projectname, char* filename ){}

//	before commit** -DONE
void wtfupdate( char* projectname ){
	printf("wtfupdate()\n");
	if(dirExists(projectname)==0){
		printf("Error: Project does not exist\n"); exitHandler();
	}
	wtfconnect(); //shuts down program if cant connect

	sendCommandProject(sockfd, "update", projectname);

	struct node* dataList = recieveData(sockfd);
	if( strcmp(dataList->next->name, "Error")==0 ){
		printf("Error: Project does not exist on server\n");
		exitHandler();
	}

	char* updatePath = getPath(projectname, UPDATE);
	char* manPath = getPath(projectname, MANIFEST);

	struct manifestNode* sMan = parseManifest(dataList->FIRSTFILENODE->content);
	struct manifestNode* cMan = parseManifest(readFileData(manPath));

	int sManVer = sMan->version;
	int cManVer = cMan->version;

	newVersionFile(sMan->version, updatePath);

	struct manifestNode* sPtr = sMan->next;
	struct manifestNode* cPtr = cMan->next;

	int existConflict = 0;
	int isEmpty = 1;

	//in cMan
	while( cPtr != NULL ){
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
	while( sPtr != NULL ){
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
	}
	if( isEmpty == 1 ){
		printf("Project is up to date\n");
		close(createFile(updatePath));
	}

}

//	after update**-DONE
void wtfupgrade( char* projectname ){
	printf("wtfupgrade()\n");
	if(dirExists(projectname)==0){
		printf("Error: Project does not exist\n"); exitHandler();
	}

	char* updatePath = getPath(projectname, UPDATE);
	char* manPath = getPath(projectname, MANIFEST);
	if( fileExists(updatePath) == 0 ){
		printf("Error: No update file found. Update before upgrading.\n");
		exitHandler();
	}
	struct stat updateStat;
	if(stat(getPath(projectname, UPDATE), &updateStat) > 0 && updateStat.st_size == 0){
		printf("Project up to date\n"); exitHandler();
	}

	wtfconnect(); //shuts down program if cant connect

	//sends the update list to server
	sendData(sockfd, versionData("upgrade", projectname, updatePath));

	struct node* dataList = recieveData(sockfd);
	if( strcmp(dataList->next->name, "Error")==0 ){
		printf("Error: Project does not exist on server\n");
		exitHandler();
	}

	struct manifestNode* mList = parseManifest(readFileData(manPath));
	struct manifestNode* uList = parseManifest(readFileData(updatePath));

	printf("client363\n");

	//delete
	struct manifestNode* uPtr = uList->next;
	while( uPtr != NULL ){
		if( strcmp(uPtr->code, "D") == 0 ){
			printf("deleted: %s\n", uPtr->path);
			//also remove from mList
			struct manifestNode * mNode = findFile( uPtr->path, mList );
			mNode->code = "deleted";
			remove(uPtr->path);
		}
		uPtr = uPtr->next;
	}

	printf("client379\n");

	//create/rewrite all the file sent
	struct node* ptr = dataList->FIRSTFILENODE;
	printf("client385\n");

	while( ptr != NULL ){
		//printf("209content %s\n", ptr->content);

		int fd = open( ptr->name, O_WRONLY|O_CREAT|O_TRUNC, 0666 );
		if( fd<0 ){ //can't open, have to create dirs then retry
			printf("Creating: %s\n", ptr->name);
			char* tempPath = (char*)malloc((strlen(ptr->name)+1)*sizeof(char));
			strcpy(tempPath, ptr->name);
			createSubdir(tempPath);
			fd = open( ptr->name, O_WRONLY|O_CREAT|O_TRUNC, 0666 );
			if( fd<0){
				printf("Error creating: %s\n", ptr->name);
				return;
			}else{
				printf("Created: %s\n", ptr->name);
			}
		}

		struct manifestNode* uNode = findFile(ptr->name, uList);
		struct manifestNode* mNode = findFile(ptr->name, mList);
		if( mNode == NULL ){
			struct manifestNode* addThis = (struct manifestNode*)malloc(1*sizeof(struct manifestNode));
			addThis->code = "uptodate";
			addThis->version = uNode->version;
			addThis->path = uNode->path;
			addThis->hash = uNode->hash;
			addThis->next = mList->next;
			mList->next = addThis;
		}else{
			mNode->code = "uptodate";
			mNode->version = uNode->version;
			mNode->hash = uNode->hash;
		}

		if( ptr->content != NULL ){
			write(fd, ptr->content, strlen(ptr->content));
		}
		close(fd);
		ptr=ptr->next;
	}

	newVersionFile( uList->version, manPath);
	mList = mList->next;
	while( mList != NULL ){
		if( strcmp(mList->code, "deleted") != 0 ){
			writeToVersionFile(manPath, mList->code, mList->version, mList->path, mList->hash);

		}
		mList = mList->next;
	}
	remove(getPath(projectname, UPDATE));
}

//	3.1** -DONE
void wtfcommit( char* projectname ){
	printf("wtfcommit()\n");
	if( dirExists(projectname) == 0 ){
		printf("Error: Project does not exist\n"); exitHandler();
	}
	wtfconnect();

	//check .update
	struct stat updateStat;
	if(stat(getPath(projectname, UPDATE), &updateStat) > 0 && updateStat.st_size>1){
		printf("Error: Must update before commiting\n"); exitHandler();
	}

	sendCommandProject(sockfd, "commit", projectname);

	struct node* dataList = recieveData(sockfd);
	printf("commit: manifest recieved\n");
	if( strcmp(dataList->next->name, "Error")==0 ){
		printf("Error: Project does not exist on server\n");
		exitHandler();
	}

	struct manifestNode* sMan = parseManifest(dataList->FIRSTFILENODE->content);
	struct manifestNode* cMan
				= parseManifest(readFileData(getPath(projectname, MANIFEST)));

	//check if manifest versions match
	if( cMan->version != sMan->version ){
		printf("Error: local repo not up to date, update before commiting\n");
		exitHandler();
	}

	char* commitPath = getPath(projectname, COMMIT);
	newVersionFile(1, commitPath);

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
	printf("commit sucessfully saved\n");
}

//	3.2** -DONE
void wtfpush( char* projectname ){
	printf("wtfcommit()\n");
	if( dirExists(projectname) == 0 ){
		printf("Error: Project does not exist\n"); exitHandler();
	}
	char* commitPath = getPath(projectname, COMMIT);
	int commitFD = open( commitPath, O_RDONLY );
	if(commitFD < 0 ){
		printf("Error: no commits to be pushed");
		exitHandler();
	}
	close(commitFD);
	struct stat updateStat;
	if(stat(getPath(projectname, UPDATE), &updateStat) > 0 && updateStat.st_size>1){
		//if there are any M codes
		printf("Error: Must upgrade before pushing\n"); exitHandler();
	}

	wtfconnect();

	struct manifestNode* cList = parseManifest(readFileData(commitPath));
	cList = cList->next;

	char* data = NULL; int count = 1;
	data = appendFileData(data, commitPath);
	while( cList != NULL ){
		printf("605: %s\n", cList->path);
		data = appendFileData(data, cList->path);
		printf("607: %s\n", data);
		cList = cList->next;
		count++;
	}
	printf("611\n");

	data = appendData(dataHeader("push", "ProjectFileContent", projectname, count), data);

	//send .commit to server
	sendData(sockfd, data);
	printf("617\n");

	struct node* dataList = recieveData(sockfd);
	if( strcmp(dataList->next->name, "Error")==0 ){
		printf("Error: Project does not exist on server\n");
		exitHandler();
	}

	//update manifest
	int manFD = open(getPath(projectname, MANIFEST), O_WRONLY|O_TRUNC|O_CREAT, 0666);
	if( manFD < 0 ){
		printf("449: Error updating manifest\n");
	}
	write(manFD, dataList->FIRSTFILENODE->content, strlen(dataList->FIRSTFILENODE->content));
	close(manFD);

	remove(commitPath);

	printf("push sucessful\n");
}

//	4.1
void wtfcurrentversion( char* projectname ){}

//	4.1
void wtfhistory( char* history ){}

//	4.1
void wtfrollback( char* projectname, char* version ){}
