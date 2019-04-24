all: WTF

WTF: helper.o WTFserver.o WTFclient.o 
	gcc -g -std=c99 -lssl -lcrypto -lm WTFclient.o helper.o -o WTF
	gcc -g -std=c99 -lssl -lcrypto -lm WTFserver.o helper.o -o WTFserver
	
helper.o: helper.c WTFheader.h
	gcc -g -std=c99 -lssl -lcrypto -lm -c  helper.c
	
WTFserver.o: WTFserver.c WTFheader.h
	gcc -g -std=c99 -lssl -lcrypto -lm -c WTFserver.c

WTFclient.o: WTFclient.c WTFheader.h
	gcc -g -std=c99 -lssl -lcrypto -lm -c WTFclient.c

clean: 
	rm -rf WTF WTFserver WTFclient.o WTFserver.o helper.o
