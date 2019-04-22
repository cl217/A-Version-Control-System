all: WTF

WTF: helper.o WTFserver.o WTFclient.o 
	gcc -g -std=c99 WTFclient.o helper.o -o WTF
	gcc -g -std=c99 WTFserver.o helper.o -o WTFserver
	
helper.o: helper.c WTFheader.h
	gcc -g -std=c99 -c helper.c
	
WTFserver.o: WTFserver.c WTFheader.h
	gcc -g -std=c99 -c WTFserver.c

WTFclient.o: WTFclient.c WTFheader.h
	gcc -g -std=c99 -c WTFclient.c

clean: 
	rm -rf WTF WTFserver WTFclient.o WTFserver.o helper.o
