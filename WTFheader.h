#ifndef _WTFheader_h_
#define _WTFheader_h_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> 
#include <netdb.h>
#include <string.h> 
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

/*
#include <arpa/inet.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctype.h>
#include <libgen.h>
*/
/** structures **/
struct node{
	char* nodeType; //command, dataType, project, numFile, fileName, fileNameContent
	int bytesName; //also used for numFile
	char* name;
	int bytesContent;
	char* content;	
	struct node* next;
};

/** helper.c **/
char* append(char*, char*);
char* appendChar(char*, char);
char* appendData(char*, char*);
struct node* splitData(char*);

/** wtfserver.c **/
void executeCommand(struct node*);

/** wtfclient.c **/
void exitSignalHandler(int sig_num);
void exitHandler();
int getFromServer();
int sendToServer(char* data);

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



/** testing purposes **/
void traverse(struct node*);



#endif
