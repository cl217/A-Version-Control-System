all: WTF
test: TEST

WTF: WTFserver.o WTFclient.o helper.o communicate.o readwrite.o
	gcc -g -std=c99 -lm -lssl -lcrypto WTFclient.o helper.o communicate.o readwrite.o -o WTF
	gcc -g -std=c99 -lm -lssl -lcrypto WTFserver.o helper.o communicate.o readwrite.o -o WTFserver
	
TEST: WTFtest.o helper.o readwrite.o
	mkdir server
	mkdir client1
	mkdir client1/testproj
	mkdir client1/testproj/folder1
	echo randomtext > client1/testproj/folder1/file1.txt
	touch client1/testproj/folder1/file2.c
	mkdir client2
	cp WTFserver server
	cp WTF client1
	cp WTF client2
	gcc -g -std=c99 -lm -lssl -lcrypto WTFtest.o helper.o readwrite.o -o WTFtest
		
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

WTFtest.o: WTFtest.c WTFheader.h
	gcc -g -std=c99 -c WTFtest.c

clean: 
	rm -rf WTF WTFclient.o WTFserver.o helper.o readwrite.o communicate.o WTFtest.o WTFserver WTFclient WTFtest server client1 client2
	
	

