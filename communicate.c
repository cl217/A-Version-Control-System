#include "WTFheader.h"


int sendData( int fd, char* data ){
	
	//printf("Sending: %s\n", data);
	
	//EC- use zlib to compress data
	int avail_comp = 3*strlen(data);
	char compressedData[avail_comp];
	z_stream defstream;
	
	defstream.zalloc = Z_NULL;
	defstream.zfree = Z_NULL;
	defstream.opaque = Z_NULL;
	
	defstream.avail_in = (uInt)strlen(data)+1;
	defstream.next_in = (Bytef*) data;
	defstream.avail_out = (uInt)avail_comp;
	defstream.next_out = (Bytef*)compressedData;
	
	deflateInit(&defstream, Z_BEST_COMPRESSION);
	deflate(&defstream, Z_FINISH);
	deflateEnd(&defstream);
	

	int sizeCompressed = defstream.total_out; //size of compressed data
	write(fd, &sizeCompressed, sizeof(int)); //send size of compressed data
	int code = receiveConfirmation(fd); //get confirmation
	
	int size = strlen(data)+1;
	write(fd, &size, sizeof(int)); //send size of regular data
	code = receiveConfirmation(fd); //get confirmation
	
	if(code != 0 ){
		write(fd, compressedData, size); //send data
		receiveConfirmation(fd); //get confirmation
	}
	return code; //0 if error
}

struct node* receiveData( int fd ){

	//receive compressed data size
	int compressedSize;
	read(fd, &compressedSize, sizeof(int));
	sendConfirmation(fd, 1);
	
	//recieve regular data size
	int dataSize;
	read(fd, &dataSize, sizeof(int));
	sendConfirmation(fd, 1);
	
	//receive compressed data
	char data[compressedSize];
	read(fd, &data, compressedSize);
	sendConfirmation(fd, 1);

	//EC- decompress data and pass into splitData
	char* decompressedData = (char*)malloc((dataSize)*sizeof(char));

	z_stream infstream;
	infstream.zalloc = Z_NULL;
	infstream.zfree = Z_NULL;
	infstream.opaque = Z_NULL;
	
	infstream.avail_in = (uInt)compressedSize;
	infstream.next_in = (Bytef*) data;
	infstream.avail_out = (uInt) dataSize;
	infstream.next_out = (Bytef*) decompressedData;
	
	inflateInit(&infstream);
	inflate(&infstream, Z_NO_FLUSH);
	inflateEnd(&infstream);
	
	//printf("Decompressed: %s\n", decompressedData);
	
	return splitData(decompressedData);
}

void sendConfirmation(int fd, int code){ //1-success, 0-failure
	int sendcode = code;
	write(fd, &sendcode, sizeof(int));
}

int receiveConfirmation( int fd ){
	int code;
	read(fd, &code, sizeof(int));
	return code;
}

struct node* splitData(char* data){

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

	//IF Datatype is ERROR/SUCCESS SIGNAL
	if(strcmp(token, "Error")==0 || strcmp(token, "Success")==0){
		token = NULL;
		while(data[i]!='\t'){
			token = appendChar(token, data[i]);
			i++;
		}
		i++; //skips delim
		int bytes = atoi(token);
		token = NULL;
		for( int k = 0; k < bytes; k++ ){
			token = appendChar(token, data[i]);
			i++;
		}
		addThis->content = (char*)malloc((strlen(token)+1)*sizeof(char));
		strcpy(addThis->content, token);
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
	//printf("Sending to server: %s\n", data);
	sendData(sockfd, data);
}
