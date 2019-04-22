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

/** wtfclient.c **/
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


/** helper.c **/
char* append(char*, char*, int);

#endif
