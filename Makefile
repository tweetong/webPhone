CC = gcc
CFLAGS = -Wall -g -lpthread 
LDLIBS =
SHARE_OBJS = util/die.o util/init.o util/dsp_set.o util/my_trans.o
SERVER_OBJS = server.o
CLIENT_OBJS = client.o
TEST_FFT_OBJS = FFT.o

all:	server client fft

server:	$(SERVER_OBJS) $(SHARE_OBJS)
	$(CC) $(SERVER_OBJS) $(SHARE_OBJS) $(LDLIBS) $(CFLAGS) -o server.out

client:	$(CLIENT_OBJS) $(SHARE_OBJS)
	$(CC) $(CLIENT_OBJS) $(SHARE_OBJS) $(LDLIBS) $(CFLAGS) -o client.out

fft:	$(TEST_FFT_OBJS) $(SHARE_OBJS)
	$(CC) $(TEST_FFT_OBJS) $(SHARE_OBJS) $(LDLIBS) -o fft.out

clean:
	rm -f *~ *.o *.out *flymake.h util/*~ util/*.o 
