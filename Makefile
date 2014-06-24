CC = gcc
CFLAGS = -Wall -g
LDLIBS = -lpthread -lm -lglut -lGLU -lGL -lm -lpng
SHARE_OBJS = util/die.o util/init.o util/dsp_set.o util/my_trans.o util/fft.o gui/gui_trans.o gui/gui_util.o gui/gui_send.o gui/gui_recv.o
SERVER_OBJS = server.o
CLIENT_OBJS = client.o
GUI_C_OBJS = test_client_gui.o
GUI_S_OBJS = test_server_gui.o 

all:	server client

server:	$(SERVER_OBJS) $(SHARE_OBJS)
	$(CC) $(SERVER_OBJS) $(SHARE_OBJS) $(LDLIBS) $(CFLAGS) -o server.out

client:	$(CLIENT_OBJS) $(SHARE_OBJS)
	$(CC) $(CLIENT_OBJS) $(SHARE_OBJS) $(LDLIBS) $(CFLAGS) -o client.out

gui:	gui_client gui_server

gui_client:	$(GUI_C_OBJS) $(SHARE_OBJS)
	$(CC) $(GUI_C_OBJS) $(SHARE_OBJS) $(LDLIBS) $(CFLAGS) -o gui_client.out

gui_server:	$(GUI_S_OBJS) $(SHARE_OBJS)
	$(CC) $(GUI_S_OBJS) $(SHARE_OBJS) $(LDLIBS) $(CFLAGS) -o gui_server.out

clean:
	rm -f *~ *.o *.out *flymake.h util/*~ util/*.o gui/*~ gui/*.o 
