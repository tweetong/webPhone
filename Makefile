CC = gcc
CFLAGS = -Wall -g
LDLIBS =
SHARE_OBJS = util/die.o util/init.o util/dsp_set.o 
SERVER_OBJS = server.o util/my_server.o
CLIENT_OBJS = client.o util/my_client.o

all:	server client

server:	$(SERVER_OBJS) $(SHARE_OBJS)
	$(CC) $(SERVER_OBJS) $(SHARE_OBJS) $(LDLIBS) -o server.out

client:	$(CLIENT_OBJS) $(SHARE_OBJS)
	$(CC) $(CLIENT_OBJS) $(SHARE_OBJS) $(LDLIBS) -o client.out

clean:
	rm -f *~ *.o *.out *flymake.h 
