#include "WTFheader.h"


/*
	./WTF configure <IP> <port> 
	IP: 127.0.0.1 for localhost
	Port: 1024-9999
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
		/*
		wtfconnect();
		char buffer[255];
		while(1){ //TODO: close on server shut down?
	
			printf("To Server: ");
			fgets(buffer, 255, stdin);
			write(sockfd, buffer, strlen(buffer));
		
			read(sockfd, buffer, 255);
			printf("Server: %s\n", buffer);
		}
		*/
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
	write(manifestFD, fileNode->content, fileNode->bytesContent);
	close(manifestFD);
}

//	1.2
void wtfdestroy( char* projectname ){}

//	1.2
void wtfcheckout( char* projectname ){}

//	2.1**
//client adds file to own manifest
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
		newManifest(mList->version, manifestPath);
		mList=mList->next;
		int curVer = 0;
		while(mList!=NULL){
			if(strcmp(mList->path, filePath)!=0){
				writeToManifest(manifestPath, mList->code, mList->version, mList->path, mList->hash);
			}else{
				curVer = mList->version;
			}	
			mList=mList->next;
		} 
		writeToManifest(manifestPath, "upload", curVer, filePath, generateHash(fileText));	
		printf("Warning: file already exists, has been overwritten\n");	
	}else{		
		writeToManifest(manifestPath, "upload", 1, filePath, generateHash(fileText));
		printf("Success: added\n");
	}	
}

//	2.2 - remove the file from manifest (tag it as removed?)
void wtfremove( char* projectname, char* filename ){}

//	before commit** 
//TODO: Awaiting clarifications on some things
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
	int updateFD = createFile(updatePath);
	
	char* manPath = getPath(projectname, MANIFEST);
	
	struct manifestNode* sMan = parseManifest(dataList->MANCONTENTNODE);
	struct manifestNode* cMan = parseManifest(readFileData(manPath));

	struct manifestNode* ptr = cMan->next;
	while( ptr!=NULL ){
		struct manifestNode* sNode = findFile(ptr->path, sMan);
		int upload = 0;
		if( sNode == NULL ){
			printf("U: %s\n", ptr->path);
			upload = 1;
		}else if( sNode->version < ptr->version ){
			printf("U: %s\n", ptr->path);		
			upload = 1;
		}
		if( sNode != NULL && sNode->version > ptr->version ){
			printf("M: %s\n", ptr->path);
			if( upload == 1 ){
				printf("Conflict: %s\n", ptr->path);
			}else{
				writeToUpdate(updateFD, "M", ptr);
			}
		}
		if( sNode == NULL ){
			printf("D: %s\n", ptr->path);
			if( upload == 1 ){
				printf("Conflict: %s\n", ptr->path);
			}else{
				writeToUpdate(updateFD, "D", ptr);
			}	
		}
		ptr=ptr->next;
	}
	//A-check sMan
	ptr = sMan->next;
	while( ptr!=NULL ){
		struct manifestNode* cNode = findFile(ptr->path, cMan);
		if( cNode == NULL ){
			printf("A: %s\n", ptr->path);
			writeToUpdate(updateFD, "A", ptr);
		}
		ptr=ptr->next;
	}
	close(updateFD);
}

//	after update**
void wtfupgrade( char* projectname ){
	printf("wtfugrade()\n");
	if(dirExists(projectname)==0){
		printf("Error: Project does not exist\n"); exitHandler();
	}
	wtfconnect(); //shuts down program if cant connect
	
}

//	3.1**
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
	
	char* commitPath = getPath(projectname, COMMIT);
	int commitFD = createFile(commitPath);
	
	struct manifestNode* sMan = parseManifest(dataList-> MANCONTENTNODE);
	struct manifestNode* cMan 
				= parseManifest(readFileData(getPath(projectname, MANIFEST)));
	
	struct manifestNode* ptr=cMan->next;
	while( ptr != NULL ){
		char* newHash = generateHash(readFileData(ptr->path));
		struct manifestNode* sNode = findFile(ptr->path, sMan);
		if( sNode == NULL || strcmp(sNode->hash, newHash)!=0 ){
			ptr->version;
			ptr->hash = newHash;
			char* str = appendData(int2str(++(ptr->version)), ptr->path);
			str = appendData(str, newHash);
			str = appendData(str, "\n");
			write(commitFD, str, strlen(str));		
		}
		ptr=ptr->next;
	}
	
	ptr=sMan->next;
	while( ptr!=NULL ){
		struct manifestNode* cNode = findFile(ptr->path, cMan);
		//if difference- file in both but version sMan>cMan
		if( cNode != NULL && strcmp(cNode->hash, ptr->hash)!=0 
							&& ptr->version > cNode->version){
			printf("Error: Must update before commit\n");
			remove(commitPath);
			close(commitFD);
			exitHandler();
		}
		ptr=ptr->next;
	}
	
	//send commit to server
	char* data = appendData("commit", "ProjectFileContent");
	data = appendData(data, int2str(strlen(projectname)));
	data = appendData(data, projectname);
	data = appendData(data, int2str(1));
	data = appendFileData(data, commitPath);
	sendData(sockfd, data);
	printf("commit sucessfully saved\n");
	close(commitFD);
}

//	3.2**
void wtfpush( char* projectname ){}

//	4.1
void wtfcurrentversion( char* projectname ){}

//	4.1
void wtfhistory( char* history ){}

//	4.1	
void wtfrollback( char* projectname, char* version ){}
