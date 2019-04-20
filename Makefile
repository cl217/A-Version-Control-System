all: WTF

WTF: WTFclient.o WTFserver.o
	gcc -g -std=c99 WTFclient.o -o WTF
	gcc -g -std=c99 WTFserver.o -o WTFserver
	
	

WTFclient.o: WTFclient.c WTFheader.h
	gcc -g -std=c99 -c WTFclient.c
	
WTFserver.o: WTFserver.c WTFheader.h
	gcc -g -std=c99 -c WTFserver.c
	
clean: 
	rm -rf WTF WTFserver WTFclient.0 WTFserver.o
