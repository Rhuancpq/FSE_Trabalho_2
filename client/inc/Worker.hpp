#ifndef __WORKER_HPP__
#define __WORKER_HPP__

#define BUFFER_SIZE 1024

#include <unordered_map>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>
using namespace std;

#include "cJSON.h"

class Worker {
public:
    void operator()(int socket, sockaddr_in & client_addr,
    unordered_map<string, int> & gpio_out_values);
};

#endif