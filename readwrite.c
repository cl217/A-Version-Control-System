#include "WTFheader.h"

char* allFileData;
int fileCount = 0;
void compressProject(char* projectname, char* outFilePath){

	allFileData = NULL;
	fileCount = 0;
	
	getAllFiles(getPath(".", projectname));
	
	printf("fileCount: %d\n", fileCount);
	
	allFileData = appendData( dataHeader("compress", "ProjectFileContent", projectname, fileCount), allFileData);
	
	//printf("allFileData: %s\n", allFileData);
	
	int dataLen = strlen(allFileData);
	
	printf("outPath: %s\n", outFilePath);
	
	int fd = createFile(outFilePath);
	close(fd);
	
	printf("rw25\n");
	
	gzFile fi = (gzFile)gzopen(outFilePath, "wb");
	printf("rw28\n");
	gzwrite(fi, (void*)allFileData, dataLen);
	printf("rw30\n");
	gzclose(fi);
	printf("rw32\n");
	
}

//use zlib to compress entire directory to a single file
void getAllFiles(char* inDirPath){
	DIR * dir = opendir(inDirPath);
	struct dirent * entry;
	if(dir == NULL){
		return;
	}
	while( (entry=readdir(dir)) != NULL ){ //loop everything in directory
		char* entrypath = getPath(inDirPath, entry->d_name);
		if( entry->d_type==DT_DIR
				&& strcmp(".", entry->d_name)!=0 && strcmp("..", entry->d_name)!=0 ){
			getAllFiles(entrypath); //go in subdirectory-recursive traversal
		}else if(entry->d_type == DT_REG){ //is file
			printf("file: %s\n", entrypath);
			fileCount++;
			allFileData = appendFileData(allFileData, entrypath);				
		}
	}
	closedir(dir);
}


void decompressDir(char* inFilePath, char* outDirPath){

	int gigabyte = 1073741824;
	//char buf[5000000];
	char* buf = (char*)malloc(gigabyte);
	
	
	gzFile fi = (gzFile)gzopen(inFilePath, "rb");
	gzrewind(fi);
	int len;
	if(!gzeof(fi)){
		//len = gzread(fi, buf, sizeof(buf));
		len = gzread(fi, buf, gigabyte);
	}	
	gzclose(fi);
	
	char* decompressed = (char*)malloc((len+1)*sizeof(char));
	memcpy(decompressed, &buf[0], len);
	decompressed[len] = '\0';
	
	//write out all the files
	struct node* fileList = splitData(decompressed);
	
	struct node* ptr = fileList->FIRSTFILENODE;
	while(ptr!=NULL){
		char* cpyPath = (char*)malloc((strlen(ptr->name)+1)*sizeof(char));
		memcpy(cpyPath, &(ptr->name[2]), strlen(ptr->name)-2+1); //copies \0
		cpyPath = getPath(outDirPath, cpyPath);
		
		int fd = open( cpyPath, O_WRONLY|O_CREAT|O_TRUNC, 0666 );
		if( fd<0 ){ //can't open, have to create dirs then retry
			char* tempPath = (char*)malloc((strlen(cpyPath)+1)*sizeof(char));
			strcpy(tempPath, cpyPath);
			createSubdir(tempPath);
			fd = open( cpyPath, O_WRONLY|O_CREAT|O_TRUNC, 0666 );
		}
		if( ptr->content != NULL ){
			write(fd, ptr->content, strlen(ptr->content));
		}
		close(fd);
		ptr = ptr->next;
	}
	
	free(buf);
}


void destroyRecursive(char* path) {
	struct dirent *file;
	DIR *direc = opendir(path);
	if (direc == NULL){
		return;
	}

	 while ((file = readdir(direc)) != NULL) {
	 	char * filepath = getPath(path,file->d_name);
	 	if (dirExists(filepath)) {
	 		if ((strcmp(file->d_name,".") != 0) && (strcmp(file->d_name, "..") != 0)) {
	 			destroyRecursive(filepath);
			}
		}
		remove(filepath);
	}
	closedir(direc);

	remove(path);
}

void copydir(char* srcPath, char*destPath){
	DIR * dir = opendir(srcPath);
	struct dirent * entry;
	if(dir == NULL){
		return;
	}
	while( (entry=readdir(dir)) != NULL ){ //loop everything in directory
		char* entrypath = getPath(srcPath, entry->d_name);
		char* cpypath = getPath(destPath, entry->d_name);
		if( entry->d_type==DT_DIR
				&& strcmp(".", entry->d_name)!=0 && strcmp("..", entry->d_name)!=0 ){
			//is directory
			createDir(cpypath);
			copydir(entrypath, cpypath); //go in subdirectory-recursive traversal
		}else if(entry->d_type == DT_REG){ //is file
			int fd = createFile(cpypath);
			char* data = readFileData(entrypath);
			if(data != NULL){
				write(fd, data, strlen(data));
			}
			close(fd);
		}
	}
}

//read file data
char* readFileData(char* filePath){
	int fileFD = open(filePath, O_RDONLY);
	if(fileFD < 0 ){
		printf("Error: %s does not exist\n", filePath); return NULL;
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

char* dataHeader( char* command, char* type, char* projectname, int numFile ){
	char* data = appendData(command, type); //command, dataType
	data = appendData(data, int2str(strlen(projectname))); //bytesPname
	data = appendData(data, projectname); //projectName
	data = appendData(data, int2str(numFile)); //numFiles
	return data;
}

//to send file (manifest or commit)
char* versionData( char* command, char* projectname, char* file){
	char* data = dataHeader(command, "ProjectFileContent", projectname, 1);
	data = appendFileData(data, file);
	return data;
}


//builds list from version data
//first node will be the version number
struct manifestNode* parseManifest(char* manifestData){
	//add node to beginning, most recent versions at the front
	struct manifestNode* manifestList = NULL;
	int i = 0;
	char* token = NULL;
	while( i < strlen(manifestData) && manifestData[i] != '\n' ){
		token = appendChar(token, manifestData[i++]);
	}
	int manVersion = atoi(token);

	while( i < strlen(manifestData) ){
		struct manifestNode* addThis =
					(struct manifestNode*)malloc(1*sizeof(struct manifestNode));
		printf("rw: newnode\n");
		token = NULL;

		//read in code
		i++; //skips newline
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
		//printf("version: %s\n", token);

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
		//printf("hash: %s\n", token);

		addThis->next = manifestList;
		manifestList = addThis;
		
		printf("newnode added\n");
		struct manifestNode* tempPtr = manifestList;
		while(tempPtr!=NULL){
			printf("tempPtr\n");
			tempPtr=tempPtr->next;	
		}	
		
	}
	struct manifestNode* addThis = (struct manifestNode*)malloc(1*sizeof(struct manifestNode));
	addThis->version = manVersion;
	addThis->next = manifestList;
	manifestList = addThis;
	
	printf("newnode added\n");
	struct manifestNode* tempPtr = manifestList;
	while(tempPtr!=NULL){
		printf("tempPtr\n");
		tempPtr=tempPtr->next;	
	}
	return manifestList;
}


/*WRITE MANIFEST/Commit
	code - new, modified, deleted, uptodate
	<manifestversion>
	<\n><code><version of file><\t><filepath><\t><hashcode>
	<\n>...
*/
char* writeToVersionFile(char* versionPath, char* code, int curVersion, char*path, char* hash){

	int versionFD = open(versionPath, O_WRONLY|O_APPEND);
	if(versionFD<0){
		printf("error: opening\n"); return NULL;
	}
	char* version = int2str(curVersion);

	char* mData = append("\n", code);
	mData = appendData(mData, version);
	mData = appendData(mData, path);
	mData = appendData(mData, hash);
	write(versionFD, mData, strlen(mData));

	close(versionFD);
	return mData;
}

//manifest or commit
void newVersionFile(int newVersion, char* filePath){
	int fileFD = open(filePath, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	if(fileFD<0){
		printf("error2: creating file\n");return;
	}
	char * versionStr = int2str(newVersion);
	write(fileFD, versionStr, strlen(versionStr));
	close(fileFD);
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
