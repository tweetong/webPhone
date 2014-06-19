#ifndef __INCLUDE_MY_SERVER_H__
#define __INCLUDE_MY_SERVER_H__

void init_my_server();
void my_server_send(char buf[],unsigned int size);
int my_server_recv(char buf[]);
void my_server_close();

#endif
