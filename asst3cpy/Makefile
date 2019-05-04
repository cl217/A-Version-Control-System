all: WTF
test: clean WTF TEST

WTF: WTFclient.o helper.o communicate.o readwrite.o
	gcc -g -std=c99 -lm -lssl -lcrypto -lz WTFclient.o helper.o communicate.o readwrite.o -o WTF
	
WTFclient.o: WTFclient.c WTFheader.h
	gcc -g -std=c99 -c WTFclient.c	

helper.o: helper.c WTFheader.h
	gcc -g -std=c99  -lm -c  helper.c
	
communicate.o: communicate.c WTFheader.h
	gcc -g -std=c99 -c communicate.c

readwrite.o: readwrite.c WTFheader.h
	gcc -g -std=c99 -lssl -lcrypto -lz -c readwrite.c

WTFtest.o: WTFtest.c WTFheader.h
	gcc -g -std=c99 -c WTFtest.c

clean: 
	rm -rf WTF WTFclient.o WTFserver.o helper.o readwrite.o communicate.o WTFtest.o WTFserver WTFclient WTFtest server client1 client2
	
	

