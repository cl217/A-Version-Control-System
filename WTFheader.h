#ifndef _WTFheader_h_
#define _WTFheader_h_
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <math.h>
#include <dirent.h>
#include <openssl/sha.h>
#include <libgen.h>
#include <sys/wait.h>
#include <pthread.h>
#include "zlib.h"

/*
#include <arpa/inet.h>
#include <ctype.h>
*/

#define MANIFEST ".manifest"
#define UPDATE ".update"
#define COMMIT ".commit"
#define ARCHIVE ".archive"
#define HISTORY ".history"

#define PROJECTNAME next->next->name
#define NUMFILENODE next->next->next
#define FIRSTFILENODE next->next->next->next



/** structures **/

//for parsing data sent between client/server
struct node{
	char* nodeType; //command, dataType, project, numFile, fileName, fileContent
	char* name; //also used for numFile
	char* content;
	struct node* next;
};

//for parsing manifest/commit/update file entries
struct manifestNode{
	char* code; //upload, modify, deleted, uptodate
	int version;
	char* path;
	char* hash;
	struct manifestNode* next;
};

//each project has a mutex
struct mutexNode{
	char* projectname;
	pthread_mutex_t mutex;
	struct mutexNode* next;
};

//each client connection has a thread
struct threadNode{
	pthread_t thread;
	int sockfd;
	struct threadNode* next;
};


/** helper.c **/
struct mutexNode* getMutex( char*, struct mutexNode*);
char* append(char*, char*);
char* appendChar(char*, char);
char* appendData(char*, char*);
char* appendFileData(char*, char*);
char* int2str(int);
char* getPath(char* current, char* entry);
int createDir(char*);
int createFile(char*);
int dirExists(char*);
void createSubdir(char*);
int fileExists(char*);
int compareVersion( char*, char*, struct manifestNode* );
int neg(int);
struct manifestNode* findFile(char*, struct manifestNode*);
char* makeMsg(char*, char*, char*);
void compressDir(char*, char*);
void decompressDir(char*, char*);


/** communicate.c **/
int sendData(int, char*);
struct node* receiveData(int);
void sendConfirmation(int, int);
int receiveConfirmation(int);
struct node* splitData(char*);
void sendCommandProject( int, char*, char*);


/** readwrite.c **/
void copydir(char*, char*);
char* generateHash(char*);
char* writeToVersionFile(char*,char*, int, char*, char*);
struct manifestNode* parseManifest(char*); //can be used with commit too
char* readFileData(char*);
void newVersionFile(int, char*); //can be used with new commit file too
void writeToUpdate(int, char*, struct manifestNode*);
char* versionData( char*, char*, char*);
char* dataHeader( char* command, char* type, char* projectname, int numFile );
void destroyRecursive(char*);

void compressProject(char* projectname, char* outFilePath);
void getAllFiles(char* inDirPath);
void decompressDir(char* inFilePath, char* outDirPath);


/** wtfserver.c **/
void executeCommand(struct node*, int);
void serverCreate(struct node*, int);
int serverSendManifest(struct node*, int);
void serverCommit(struct node*, int);
void serverPush(struct node*, int);
void serverUpgrade(struct node*, int);
void serverCheckout(char*, int);
void serverDestroy(char *, int);
void serverCurrentVersion( char *, int);
void serverHistory(struct node *, int);
void serverRollback(struct node *, int);
void writeHistory(struct manifestNode*, int, char*);


/** wtfclient.c **/
void exitSignalHandler(int sig_num);
void exitHandler();
void wtfconnect();
void wtfconfigure(char*, char*);
void wtfcreate(char*);
void wtfdestroy(char*);
void wtfcheckout(char*);
void wtfadd(char*, char*);
void wtfremove(char*, char*);
void wtfcommit(char*);
void wtfpush(char*);
void wtfcurrentversion(char*);
void wtfhistory(char*);
void wtfrollback(char* , char*);
void wtfupdate(char*);
void wtfupgrade(char*);
void traverseDir(char*);


#endif
