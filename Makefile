all: WTF

WTF: WTFserver.o WTFclient.o helper.o communicate.o readwrite.o
	gcc -g -std=c99 -lm -lssl -lcrypto WTFclient.o helper.o communicate.o readwrite.o -o WTF
	gcc -g -std=c99 -lm -lssl -lcrypto WTFserver.o helper.o communicate.o readwrite.o -o WTFserver
	
WTFserver.o: WTFserver.c WTFheader.h
	gcc -g -std=c99 -c WTFserver.c

WTFclient.o: WTFclient.c WTFheader.h
	gcc -g -std=c99 -c WTFclient.c	

helper.o: helper.c WTFheader.h
	gcc -g -std=c99  -lm -c  helper.c
	
communicate.o: communicate.c WTFheader.h
	gcc -g -std=c99 -c communicate.c

readwrite.o: readwrite.c WTFheader.h
	gcc -g -std=c99 -lssl -lcrypto -c readwrite.c

clean: 
	rm -rf WTF WTFserver WTFclient.o WTFserver.o helper.o
