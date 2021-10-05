#ifndef __MESSENGER_HPP__
#define __MESSENGER_HPP__

#include <string>
#include <thread>
#include <streambuf>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <iostream>
using namespace std;

#include <semaphore.h>

#include "cJSON.h"

void messager_sem_init();
void send_message_async(string ip, int port, cJSON * message);

#endif