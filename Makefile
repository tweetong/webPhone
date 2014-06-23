CC = gcc
CFLAGS = -Wall -g -lpthread 
LDLIBS =
SHARE_OBJS = util/die.o util/init.o util/dsp_set.o util/my_trans.o
SERVER_OBJS = server.o
CLIENT_OBJS = client.o

all:	server client

server:	$(SERVER_OBJS) $(SHARE_OBJS)
	$(CC) $(SERVER_OBJS) $(SHARE_OBJS) $(LDLIBS) $(CFLAGS) -o server.out

client:	$(CLIENT_OBJS) $(SHARE_OBJS)
	$(CC) $(CLIENT_OBJS) $(SHARE_OBJS) $(LDLIBS) $(CFLAGS) -o client.out

clean:
	rm -f *~ *.o *.out *flymake.h util/*~ util/*.o 
