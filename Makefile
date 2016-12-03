CC = gcc
#CFLAGS = -lsocket -lnsl
CFLAGS = 

all: tclient tserver


clean:
	@rm -rf *.o
	@rm -rf bin/client/tclient
	@rm -rf bin/server/tserver


tclient.o : src/tclient.c
	$(CC) $(CFLAGS) -c $<

tserver.o : src/tserver.c
	$(CC) $(CFLAGS) -c $<

common.o : src/common.c
	$(CC) $(CFLAGS) -c $<

tclient: common.o tclient.o	
	$(CC) $(CFLAGS)  -o bin/client/tclient  common.o tclient.o 

tserver: common.o tserver.o	
	$(CC) $(CFLAGS)  -lssl  -o bin/server/tserver common.o tserver.o
	
	

	
