#ifndef __LISTENER_HPP__
#define __LISTENER_HPP__

#include "cJSON.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <streambuf>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <signal.h>
#include <unordered_map>
#include <thread>
using namespace std;

#include "Messager.hpp"

class Listener {
public:
    void operator()(const bool & is_end, string ip, int port);
};

#endif // __LISTENER_HPP__