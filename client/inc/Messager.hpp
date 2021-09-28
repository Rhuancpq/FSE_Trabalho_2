#ifndef __MESSENGER_HPP__
#define __MESSENGER_HPP__

#include "cJSON.h"
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
using namespace std;

int send_message(cJSON *message);
extern string server_addr;
extern int server_port;
void set_server_addr(struct sockaddr_in * serv_addr, const char * addr, const char * port);


#endif