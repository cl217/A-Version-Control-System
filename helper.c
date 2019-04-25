#include "WTFheader.h"

//finds the lastest version of file in manifestList
//returns the ptr to that node
struct manifestNode* findFile( char* filepath, struct manifestNode* manifestList){
	struct manifestNode* ptr = manifestList->next;
	while(ptr!=NULL){
		if(strcmp(ptr->path, filepath)==0){
			return ptr;
		}
		ptr = ptr->next;
	}
	return NULL; //file not in manifest
	
}

//compares file against last version of file in manifest
int compareVersion( char* filePath, char* hash, struct manifestNode* list ){
	struct manifestNode* ptr = list;
	while(ptr!=NULL){
		if( strcmp(filePath, list->path) == 0 ){
			if( strcmp(hash, list->hash) == 0 ){
				return 0; //same		
			}
			return 1; //different (exists)
		}
		ptr=ptr->next;	
	}
	return 2; //different (new file)

}

//turns postive int to negative int
int neg(int num){
	return -1*num;
}

//1(true), 0(false)
int dirExists(char* dirname){
	DIR* dir = opendir(dirname);
	if(dir==NULL){
		closedir(dir);
		return 0; //does not exist
	}
	closedir(dir);
	return 1; //exists
}

int fileExists(char* fileName){
	int fileFD =  open(fileName,O_RDONLY);
	if(fileFD<0){
		return 0; //does not exist
	}
	close(fileFD);
	return 1; //exists
}

void* writeToFile(int fd, char* data){

}


char* int2str(int num){
	int isNeg = 0;
	if( num < 0 ){
		isNeg = 1;
		num = -1*num;
	}
	int digits = floor( log10( num ) ) + 1;
	char* str;
	
	if( isNeg == 0 ){
		str = (char*)malloc((digits+1)*sizeof(char));
		sprintf(str, "%d", num);
		str[digits]='\0';
	}else{
		str = (char*)malloc((digits+2)*sizeof(char));
		sprintf(str, "%d", neg(num));
		str[digits+1]='\0';
	}
	
	//printf( "returnnum: %s\n", str);
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

char* appendFileData(char* data, char* filePath){
	printf("path: %s\n", filePath);
	data = appendData(data, int2str(strlen(filePath)));
	data = appendData(data, filePath);
	char* fileData = readFileData(filePath);
	data = appendData(data, int2str(strlen(fileData)));
	data = appendData(data, fileData);
	return data;
}

//concats: "current"+"/"+"entry"="current/entry"
char* getPath( char* current, char* entry ){
	char* path = (char*)malloc((strlen(current)+strlen(entry)+2)*sizeof(char));
	strcpy(path, current);
	strcat(path, "/");
	strcat(path, entry);
	return path;
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
	closedir(dir);
	return 1; //success
}

//make sure to close file
int createFile(char* filePath){
	printf("filePath: %s\n", filePath);
	int fileFD = open(filePath, O_CREAT|O_WRONLY|O_TRUNC, 0666);
	if( fileFD < 0 ){
		printf("error creating file\n"); return 0;
	}
	return fileFD;
}

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
			//printf("\tbytesName(%d)\n", ptr->bytesName);
			
		}else if(strcmp(nodeType, "numFile")==0){
			printf("\tname(%s)\n", ptr->name);
			
		}else if(strcmp(nodeType, "fileName")==0){
			printf("\tname(%s)\n", ptr->name);
			//printf("\tbytesName(%d)\n", ptr->bytesName);
			
		}else if(strcmp(nodeType, "fileContent")==0){
			printf("\tname(%s)\n", ptr->name);
			//printf("\tbytesName(%d)\n", ptr->bytesName);
			printf("\tcontent(%s)\n", ptr->content);
			//printf("\tbytesContent(%d)\n", ptr->bytesContent);
		}else{
			printf("\tinvalid node type\n");
		}
		
		printf("\n");
		ptr = ptr->next;	
	}
}




