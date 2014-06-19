#ifndef __INCLUDE_MY_CLIENT_H__
#define __INCLUDE_MY_CLIENT_H__

void init_my_client(char *argv);
void my_client_send(char buf[],unsigned int size);
int my_client_recv(char buf[]);
void my_client_close();

#endif
