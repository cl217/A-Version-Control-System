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
	
	printf("data: %s\n", data);
	close(fileFD);
	return data;
}


//builds list from manifest data
struct manifestNode* parseManifest(char* manifestData){
	//add node to beginning, most recent versions at the front
	struct manifestNode* manifestList = NULL;
	
	int firstIteration = 1;
	int i = 1;
	while( i < strlen(manifestData) ){
		struct manifestNode* addThis = 
					(struct manifestNode*)malloc(1*sizeof(struct manifestNode));
		char* token = NULL;	

		//read in code
		while( manifestData[i] !='\t'){
			token = appendChar(token, manifestData[i++]);
		}
		addThis->code = (char*)malloc((strlen(token)+1)*sizeof(char));
		strcpy(addThis->code, token);
		printf("code: %s\n", token);
		
		//read in version
		token = NULL; i++;
		while(manifestData[i] != '\t'){
			token = appendChar(token, manifestData[i++]);
		}
		addThis->version = atoi(token);
		printf("version: %s\n", token);
		
		//read in path
		token = NULL; i++;
		while( manifestData[i] != '\t'){
			token = appendChar(token, manifestData[i++]);
		} 
		addThis->path = (char*)malloc((strlen(token)+1)*sizeof(char));
		strcpy(addThis->path, token);
		printf("path: %s\n", token);
				
		//read hash code		
		token = NULL; i++;
		while( i < strlen(manifestData) && manifestData[i] != '\n' ){
			token = appendChar(token, manifestData[i++]);		
		}
		addThis->hash = (char*)malloc((strlen(token)+1)*sizeof(char));
		strcpy(addThis->hash, token);
		printf("hash: %s\n", token);
		
		if(manifestList == NULL ){
			manifestList=addThis;
		}else{
			addThis->next = manifestList;
			manifestList = addThis;
		}
	}
	printf("rw76\n");
	return manifestList;
}


/*WRITE MANIFEST 
	code - new, modified, deleted, uptodate
	<\n><code><version of project><\t><filepath><\t><hashcode>
	<\n>...
*/
char* writeToManifest(char* manifestPath, char* code, int curVersion, char* filepath, char* hash){

	int manifestFD = open(manifestPath, O_WRONLY|O_APPEND);
	if(manifestFD<0){
		printf("error: opening\n");
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

