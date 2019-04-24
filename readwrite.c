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
	printf("data: %s\n", data);
	close(fileFD);
	return data;
}



//reads from the manifest file
struct manifestNode* readManifest(char* manifestPath){
	//add node to beginning, most recent versions at the front
	struct manifestNode* manifestList = NULL;
	
	int manifestFD = open(manifestPath, O_RDONLY);
	if(manifestFD < 0 ){
		printf("Error: .manifest does not exist\n"); return NULL; 
	}
	char c[1];
	int firstIteration = 1;
	while( read(manifestFD, &c[0], 1) > 0 ){
		struct manifestNode* addThis = 
					(struct manifestNode*)malloc(1*sizeof(struct manifestNode));
		char* token = NULL;	
		//read in version
		
		if( firstIteration == 1 ){
			read(manifestFD, &c[0], 1); //reads char after newline
			firstIteration = 0;
		}
		do{
			token = appendChar(token, c[0]);
		}while(read(manifestFD, &c[0], 1) && c[0] != '\t');
		addThis->version = atoi(token);
		
		//read in path
		token = NULL;
		while(read(manifestFD, &c[0], 1)>0 && c[0] != '\t'){
			token = appendChar(token, c[0]);
		} 
		addThis->path = (char*)malloc((strlen(token)+1)*sizeof(char));
		strcpy(addThis->path, token);
				
		//read hash code		
		token = NULL;
		while(read(manifestFD, &c[0], 1)>0 && c[0] != '\n'){
			token = appendChar(token, c[0]);		
		}
		addThis->hash = (char*)malloc((strlen(token)+1)*sizeof(char));
		strcpy(addThis->hash, token);
		
		if(manifestList == NULL ){
			manifestList=addThis;
		}else{
			addThis->next = manifestList;
			manifestList = addThis;
		}
	}
	close(manifestFD);
	return manifestList;
}


/*WRITE MANIFEST 
	<\n><version of project><\t><filepath><\t><hashcode>
	<\n>...
*/
char* writeToManifest(char* manifestPath, int curVersion, char* filepath, char* hash){

	int manifestFD = open(manifestPath, O_WRONLY|O_APPEND);
	if(manifestFD<0){
		printf("error: opening\n");
	}
	char* version = int2str(curVersion);
	
	char* mData = append("\n", version);
	mData = appendData(mData, filepath);
	mData = appendData(mData, hash);
		
	write(manifestFD, mData, strlen(mData));
	//write(manifestFD, "h", 1);
	
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

