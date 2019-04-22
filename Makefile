all: WTF

WTF: WTFclient.o WTFserver.o helper.o
	gcc -g -std=c99 WTFclient.o helper.o -o WTF
	gcc -g -std=c99 WTFserver.o -o WTFserver
	
WTFclient.o: WTFclient.c WTFheader.h
	gcc -g -std=c99 -c WTFclient.c
	
WTFserver.o: WTFserver.c WTFheader.h
	gcc -g -std=c99 -c WTFserver.c
	
helper.o: helper.c WTFheader.h
	gcc -g -std=c99 -c helper.c
	
clean: 
	rm -rf WTF WTFserver WTFclient.o WTFserver.o helper.o
