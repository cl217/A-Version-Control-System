#include "WTFheader.h"

//helper to build the token while reading in
//append("dest", "src") returns "destsrc"
char* append( char* dest, char* src ){
	if( dest == NULL ){
		char* newString = (char*)malloc(2*sizeof(char));
		strcpy(newString, src);
		return newString;
	}
	char* newString = (char*)malloc(strlen(dest)*sizeof(char));
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

//TODO: splits data
/*	FORMAT: 
		-delimeter to split tokens is '\n'
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
	printf("splitData()\n");
	struct node* dataList = NULL;
	struct node* endPtr = NULL;
	
	int i = 0;
	
	//READ IN COMMAND NODE
	char * token = NULL;
	do{
		appendChar(token, data[i]);
		i++;
	}while(data[i]!='\0');
	struct node* addThis = (struct node*)malloc(1*sizeof(struct node));
	addThis->nodeType = "command";
	addThis->name = (char*)malloc((strlen(token)+1)*sizeof(char));
	strcpy(addThis->name, token);
	dataList = addThis; endPtr=addThis;
		
	//READ IN DATA TYPE NODE
	token = NULL;
	do{
		appendChar(token, data[i]);
		i++;
	}while(data[i]!='\0');
	addThis = (struct node*)malloc(1*sizeof(struct node));
	addThis->nodeType = "dataType";
	addThis->name = (char*)malloc((strlen(token)+1)*sizeof(char));
	strcpy(addThis->name, token);
	endPtr->next=addThis; endPtr = addThis;
	char* type = token;
	
	//READ IN PROJECT NODE
	//read in projectname Bytes
	token = NULL;
	do{
		appendChar(token, data[i]);
		i++;
	}while(data[i]!='\0');
	addThis = (struct node*)malloc(1*sizeof(struct node));
	addThis->nodeType = "project";
	int bytes = atoi(token);
	addThis->bytesName = bytes;
	//read in project name
	token = NULL;
	for( int k = 0; k < bytes; k++ ){
		appendChar(token, data[i]);
		i++;
	}
	i++; //skip delimeter
	addThis->name = (char*)malloc((strlen(token)+1)*sizeof(char));
	strcpy(addThis->name, token);
	endPtr->next = addThis; endPtr = addThis;	
	
	//if data contains files
	if( strcmp(type, "Project") != 0 ){
		//NUMFILE NODE
		token = NULL;
		do{
			appendChar(token, data[i]);
			i++;
		}while(data[i]!='\t');
		int numFile = atoi(token);
		addThis = (struct node*)malloc(1*sizeof(struct node));
		addThis->nodeType = "numFile";
		addThis->bytesName = numFile;
		endPtr->next=addThis; endPtr=addThis;

		//READ IN FILES		
		for( int k = 0; k < numFile; k++ ){
			addThis = (struct node*)malloc(1*sizeof(struct node));
			addThis->nodeType = (strstr(type, "Content")!= NULL)? "fileName":"fileContent";
			//read in bytes of file Name
			token = NULL;
			do{
				appendChar(token, data[i]);
				i++;
			}while(data[i]!='\t');
			int numByte = atoi(token);
			addThis->bytesName = numByte;
			
			//read in file name
			token = NULL;
			for( int m = 0; m < numByte; m++ ){
				appendChar(token, data[i]);
				i++;
			}
			i++; //skip delimeter
			addThis->name = (char*)malloc((strlen(token)+1)*sizeof(char));
			strcpy(addThis->name, token);
			
			//if data contains file content
			if( strstr(type,"Content") != NULL ){
				//read in bytes of file content
				token = NULL;
				do{
					appendChar(token, data[i]);
					i++;
				}while(data[i]!='\t');
				numByte = atoi(token);
				addThis->bytesContent = numByte;
				
				//read in file content
				for( int m = 0; m < numByte; m++ ){
					appendChar(token, data[i]);
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
	printf("traverse()\n");
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
			
		}else if(strcmp(nodeType, "fileNameContent")==0){
			printf("\tname(%s)\n", ptr->name);
			printf("\tbytesName(%d)\n", ptr->bytesName);
			printf("\tcontent(%s)\n", ptr->content);
			printf("\tbytesContent(%s)\n", ptr->bytesContent);
		}else{
			printf("\tinvalid node type\n");
		}
		
		printf("\n");
		ptr = ptr->next;	
	}
}




