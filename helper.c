#include "WTFheader.h"


void* writeToFile(int fd, char* data){

}

char* int2str(int num){

	int digits = floor( log10( num ) ) + 1;
	char* str = (char*)malloc((digits+1)*sizeof(char));
	sprintf(str, "%d", num);
	str[digits]='\0';
	return str;
}

//append("dest", "src") returns "destsrc"
char* append( char* dest, char* src ){
	if( dest == NULL ){
		char* newString = (char*)malloc((strlen(src)+1)*sizeof(char));
		strcpy(newString, src);
		return newString;
	}
	char* newString = (char*)malloc((strlen(dest)+strlen(src)+1)*sizeof(char));
	strcpy(newString, dest); //strcpy will copy null terminator
	strcat(newString, src); //will overwrite /0 of dest, concat src
	return newString;
}

//appendChar- appends char to end of string
char* appendChar( char* dest, char src ){
	if( dest == NULL ){
		char* newString = (char*)malloc(2*sizeof(char));
		newString[0] = src;
		newString[1] = '\0';
		return newString;
	}
	char* newString = (char*)malloc((strlen(dest)+2)*sizeof(char));
	strcpy(newString, dest); //strcpy will copy null terminator
	char* srcStr = (char*)malloc(2*sizeof(char));
	srcStr[0] = src;
	srcStr[1] = '\0';
	strcat(newString, srcStr); //will overwrite /0 of dest, concat src
	return newString;
}

//appendData("dest", "src") returns dest+\t+src = "dest\tsrc"
char* appendData( char* dest, char* src ){
	char* newString = (char*)malloc((strlen(dest)+strlen(src)+2)*sizeof(char));
	strcpy(newString, dest);
	strcat(newString, "\t");
	strcat(newString, src);
	return newString;
}

//concats: "current"+"/"+"entry"="current/entry"
char* getPath( char* current, char* entry ){
	char* path = (char*)malloc((strlen(current)+strlen(entry)+2)*sizeof(char));
	strcpy(path, current);
	strcat(path, "/");
	strcat(path, entry);
	return path;
}


int sendData( int fd, char* data ){
	int size = strlen(data);
	write(fd, &size, sizeof(int)); //send size of data
	int code = recieveConfirmation(fd); //get confirmation
	if(code != 0 ){
		write(fd, data, size); //send data
		recieveConfirmation(fd); //get confirmation
	}
	return code; //0 if error
}

struct node* recieveData( int fd ){
	//recieve data size
	int dataSize;
	read(fd, &dataSize, sizeof(int));
	sendConfirmation(fd, 1);
	
	printf("recieveData81\n");
	
	//recieve data
	char data[dataSize+1];
	read(fd, &data, dataSize);
	data[dataSize]='\0';
	sendConfirmation(fd, 1);
	
	printf("recieveData87\n");
	
	return splitData(data);	
}

void sendConfirmation(int fd, int code){ //1-success, 0-failure
	int sendcode = code;
	write(fd, &sendcode, sizeof(int));
}

int recieveConfirmation( int fd ){
	int code;
	read(fd, &code, sizeof(int));
	return code;
}

/*WRITE MANIFEST 
	<version of project><\t><filepath><\t><hashcode><\n>
*/
char* writeToManifest(int curVersion, char* filepath, char* fileData){

	int manifestFD = open(filepath, O_WRONLY|O_APPEND);
	if(manifestFD<0){
		printf("error: opening\n");
	}
	char* hash = generateHash(fileData);
	char* version = int2str(++curVersion);
	
	char* mData = appendData(version, filepath);
	mData = appendData(mData, hash);
	
	write(manifestFD, mData, strlen(mData));
	
	close(manifestFD);
	return mData;
}


char* generateHash( char* fileData ){
	
	unsigned char temp[SHA_DIGEST_LENGTH];
	char* buf = (char*)malloc((SHA_DIGEST_LENGTH*2+1)*sizeof(char));
	memset(temp, 0x0, SHA_DIGEST_LENGTH);
	SHA1((unsigned char*)fileData, strlen(fileData), temp);
	for(int i = 0; i < SHA_DIGEST_LENGTH; i++){
		sprintf((char*)&buf[i*2], "%02x", temp[i]);
		
	}
	buf[SHA_DIGEST_LENGTH*2] = '\0';
	return buf;	
}


int createDir(char* dirPath){
	printf("dirPath: %s\n", dirPath);
	DIR* dir = opendir(dirPath);
	if( dir != NULL ){
		return 0; //dir already exists
	}
	int code = mkdir(dirPath, 0700);
	if( code == -1 ){
		printf("error making dir\n");
	}
	return 1; //sucess
}

int createFile(char* filePath){
	printf("filePath: %s\n", filePath);
	int fileFD = open(filePath, O_CREAT|O_WRONLY|O_TRUNC, 0666);
	if( fileFD < 0 ){
		printf("error creating file\n"); return 0;
	}
	close(fileFD);
	return 1;
}

//TODO: splits data
/*	FORMAT: 
		-delimeter to split tokens is '\t'
		-first token will always be the command
		-2nd token (if any) will be type of data:
			-Project: sending project name only (create, etc)
			-ProjectFile: sending projectName with fileName, no content (remove,destroy,etc)
			-ProjectFileContent: sending projectName with fileNames and content 
			-ProjectVersion: ProjectName and version
		-3rd token 
			-if files are sent- num of files being sent
		
		-For ProjectFileContent
			<command><dataType><bytesPname><projectName>
				<numFile><bytesfName><fName><bytefContent><fContent>...	
			
*/

struct node* splitData(char* data){
	//printf("splitData()\n");
	struct node* dataList = NULL;
	struct node* endPtr = NULL;
	
	int i = 0;
	
	//READ IN COMMAND NODE
	char * token = NULL;
	while(data[i]!='\t'){
		token = appendChar(token, data[i]);
		i++;
	}
	i++; //skips delim
	struct node* addThis = (struct node*)malloc(1*sizeof(struct node));
	addThis->nodeType = "command";
	addThis->name = (char*)malloc((strlen(token)+1)*sizeof(char));
	strcpy(addThis->name, token);
	dataList = addThis; endPtr=addThis;
	//printf("command: %s\n", token);
		
	//READ IN DATA TYPE NODE
	token = NULL;
	while(data[i]!='\t'){
		token = appendChar(token, data[i]);
		i++;
	}
	i++; //skips delim
	addThis = (struct node*)malloc(1*sizeof(struct node));
	addThis->nodeType = "dataType";
	addThis->name = (char*)malloc((strlen(token)+1)*sizeof(char));
	strcpy(addThis->name, token);
	endPtr->next=addThis; endPtr = addThis;
	char* type = token;
	//printf("dataType: %s\n", token);
	
	//READ IN PROJECT NODE
	//read in projectname Bytes
	token = NULL;
	while(data[i]!='\t'){
		token = appendChar(token, data[i]);
		i++;
	}
	i++; //skips delim
	addThis = (struct node*)malloc(1*sizeof(struct node));
	addThis->nodeType = "project";
	int bytes = atoi(token);
	addThis->bytesName = bytes;
	//read in project name
	token = NULL;
	for( int k = 0; k < bytes; k++ ){
		token = appendChar(token, data[i]);
		i++;
	}
	i++; //skip delimeter
	addThis->name = (char*)malloc((strlen(token)+1)*sizeof(char));
	strcpy(addThis->name, token);
	endPtr->next = addThis; endPtr = addThis;	
	//printf("Projectname: %s\n", token);
	
	//if data contains files
	if( strcmp(type, "Project") != 0 ){
		//NUMFILE NODE
		token = NULL;
		while(data[i]!='\t'){
			token = appendChar(token, data[i]);
			i++;
		}
		i++;
		int numFile = atoi(token);
		addThis = (struct node*)malloc(1*sizeof(struct node));
		addThis->nodeType = "numFile";
		addThis->bytesName = numFile;
		endPtr->next=addThis; endPtr=addThis;

		//READ IN FILES		
		for( int k = 0; k < numFile; k++ ){
			addThis = (struct node*)malloc(1*sizeof(struct node));
			addThis->nodeType = (strstr(type, "Content")== NULL)? "fileName":"fileContent";
			
			//read in bytes of file Name
			token = NULL;
			while(data[i]!='\t'){
				token = appendChar(token, data[i]);
				i++;
			}
			i++;
			int numByte = atoi(token);
			addThis->bytesName = numByte;
			
			//read in file name
			token = NULL;
			for( int m = 0; m < numByte; m++ ){
				token = appendChar(token, data[i]);
				i++;
			}
			i++; //skip delimeter
			addThis->name = (char*)malloc((strlen(token)+1)*sizeof(char));
			strcpy(addThis->name, token);
			
			//if data contains file content
			if( strstr(type,"Content") != NULL ){
				printf("286\n");
				//read in bytes of file content
				token = NULL;
				while(data[i]!='\t'){
					token = appendChar(token, data[i]);
					i++;
				}
				i++;
				numByte = atoi(token);
				addThis->bytesContent = numByte;
				//printf("numByte: %d\n", addThis->bytesContent);
				
				//read in file content
				for( int m = 0; m < numByte; m++ ){
					token = appendChar(token, data[i]);
					i++;
				}
				addThis->content = (char*)malloc((strlen(token)+1)*sizeof(char));
				strcpy(addThis->content, token);
				i++; //skip delimeter
			}
			endPtr->next=addThis; endPtr=addThis;

		}
	}
	traverse(dataList);
	return dataList; //return head of list
}

//temporary traverse function for testing
void traverse(struct node* list){
	printf("\ntraverse()\n\n");
	struct node* ptr = list;
	while( ptr != NULL ){
		char* nodeType = ptr->nodeType;
		
		printf("type(%s)\n", ptr->nodeType);		
		
		if(strcmp(nodeType, "command")==0){
			printf("\tname(%s)\n", ptr->name);
			
		}else if(strcmp(nodeType, "dataType")==0){
			printf("\tname(%s)\n", ptr->name);
			
		}else if(strcmp(nodeType, "project")==0){
			printf("\tname(%s)\n", ptr->name);
			printf("\tbytesName(%d)\n", ptr->bytesName);
			
		}else if(strcmp(nodeType, "numFile")==0){
			printf("\tbytesName(%d)\n", ptr->bytesName);
			
		}else if(strcmp(nodeType, "fileName")==0){
			printf("\tname(%s)\n", ptr->name);
			printf("\tbytesName(%d)\n", ptr->bytesName);
			
		}else if(strcmp(nodeType, "fileContent")==0){
			printf("\tname(%s)\n", ptr->name);
			printf("\tbytesName(%d)\n", ptr->bytesName);
			printf("\tcontent(%s)\n", ptr->content);
			printf("\tbytesContent(%d)\n", ptr->bytesContent);
		}else{
			printf("\tinvalid node type\n");
		}
		
		printf("\n");
		ptr = ptr->next;	
	}
}




