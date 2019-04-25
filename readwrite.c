#include "WTFheader.h"



//read file data
char* readFileData(char* filePath){
	int fileFD = open(filePath, O_RDONLY);
	if(fileFD < 0 ){
		printf("Error: file does not exist\n"); return NULL;
	}
	char c[1];
	char* data = NULL; 
	while( read(fileFD, &c[0], 1) > 0 ){
		data = appendChar(data, c[0]);
	}
	
	if(data==NULL){
		data = "";
	}
	
	//printf("data: %s\n", data);
	close(fileFD);
	return data;
}


//builds list from manifest data
//first node will be the version number
struct manifestNode* parseManifest(char* manifestData){
	//add node to beginning, most recent versions at the front
	struct manifestNode* manifestList = NULL;
	int i = 0;
	char* token = NULL;
	while( manifestData[i] != '\n' ){
		token = appendChar(token, manifestData[i++]);
	}
	int manVersion = atoi(token); i++; //skips newline
	
	while( i < strlen(manifestData) ){
		struct manifestNode* addThis = 
					(struct manifestNode*)malloc(1*sizeof(struct manifestNode));
		token = NULL;	

		//read in code
		while( manifestData[i] !='\t'){
			token = appendChar(token, manifestData[i++]);
		}
		addThis->code = (char*)malloc((strlen(token)+1)*sizeof(char));
		strcpy(addThis->code, token);
		//printf("code: %s\n", token);
		
		//read in version
		token = NULL; i++;
		while(manifestData[i] != '\t'){
			token = appendChar(token, manifestData[i++]);
		}
		addThis->version = atoi(token);
		//printf("version: %s\n", token);
		
		//read in path
		token = NULL; i++;
		while( manifestData[i] != '\t'){
			token = appendChar(token, manifestData[i++]);
		} 
		addThis->path = (char*)malloc((strlen(token)+1)*sizeof(char));
		strcpy(addThis->path, token);
		//printf("path: %s\n", token);
				
		//read hash code		
		token = NULL; i++;
		while( i < strlen(manifestData) && manifestData[i] != '\n' ){
			token = appendChar(token, manifestData[i++]);		
		}
		addThis->hash = (char*)malloc((strlen(token)+1)*sizeof(char));
		strcpy(addThis->hash, token);
		//printf("hash: %s\n", token);
		
		if(manifestList == NULL ){
			manifestList=addThis;
		}else{
			addThis->next = manifestList;
			manifestList = addThis;
		}
	}
	struct manifestNode* addThis = (struct manifestNode*)malloc(1*sizeof(struct manifestNode));
	addThis->version = manVersion;
	addThis->next = manifestList;
	manifestList = addThis;
	
	return manifestList;
}


/*WRITE MANIFEST 
	code - new, modified, deleted, uptodate
	<manifestversion>
	<\n><code><version of file><\t><filepath><\t><hashcode>
	<\n>...
*/
char* writeToManifest(char* manifestPath, char* code, int curVersion, char* filepath, char* hash){

	int manifestFD = open(manifestPath, O_WRONLY|O_APPEND);
	if(manifestFD<0){
		printf("error: opening\n"); return NULL;
	}
	char* version = int2str(curVersion);
	
	char* mData = append("\n", code);
	mData = appendData(mData, version);
	mData = appendData(mData, filepath);
	mData = appendData(mData, hash);
	write(manifestFD, mData, strlen(mData));
	
	close(manifestFD);
	return mData;
}

void newManifest(int newVersion, char* manifestPath){
	int manifestFD = open(manifestPath, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	if(manifestFD<0){
		printf("error: creating new manifest\n"); return;
	}
	char * versionStr = int2str(newVersion);
	write(manifestFD, versionStr, strlen(versionStr)); 
	close(manifestFD);
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

void writeToUpdate(int fd, char* code, struct manifestNode* node){
	char* str = appendData( code, int2str(node->version));
	str = appendData(str, node->path);
	str = appendData(str, node->hash);
	write(fd, str, strlen(str));
	write(fd, "\n", 1);
}


