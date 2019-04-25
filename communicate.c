#include "WTFheader.h"



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
	
	//recieve data
	char data[dataSize+1];
	read(fd, &data, dataSize);
	data[dataSize]='\0';
	sendConfirmation(fd, 1);
	
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
	
	//printf("splitdata: %s\n", data);
	
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
	while(data[i]!='\t' && i < strlen(data)){
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
	if(strcmp(token, "Error")==0 || strcmp(token, "Success")==0){
		return dataList;
	}
	
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
	//addThis->bytesName = bytes;
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
		//addThis->bytesName = numFile;
		addThis->name = token;
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
			//addThis->bytesName = numByte;
			
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
				//printf("286\n");
				//read in bytes of file content
				token = NULL;
				while(data[i]!='\t'){
					token = appendChar(token, data[i]);
					i++;
				}
				i++;
				numByte = atoi(token);
				//addThis->bytesContent = numByte;
				//printf("numByte: %d\n", addThis->bytesContent);
				
				//read in file content
				if( numByte == 0 ){
					addThis->content = NULL;
				}else{
					token = NULL;
					for( int m = 0; m < numByte; m++ ){
						token = appendChar(token, data[i]);
						i++;
					}
					addThis->content = (char*)malloc((strlen(token)+1)*sizeof(char));
					strcpy(addThis->content, token);
				}
				i++; //skip delimeter
			}
			endPtr->next=addThis; endPtr=addThis;

		}
	}
	//traverse(dataList);
	return dataList; //return head of list
}

void sendCommandProject( int sockfd, char* command, char* projectname ){
	char * data = appendData(command, "Project"); //command, dataType
	data = appendData(data, int2str(strlen(projectname))); //bytesPname
	data = appendData(data, projectname); //projectName
	printf("Sending to server: %s\n", data);
	sendData(sockfd, data);	
}
