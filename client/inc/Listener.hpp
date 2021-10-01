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

#define DEFAULT_IP "0.0.0.0"

#include "Messager.hpp"
#include "Worker.hpp"
#include "GPIO.hpp"

class Listener {
public:
    void operator()(const bool & is_end, string name, int port,
    unordered_map<string, int> & gpio_out_values,
    const vector<GPIO_Pin> & in_gpios, const vector<GPIO_Pin> & out_gpios);
};

#endif // __LISTENER_HPP__