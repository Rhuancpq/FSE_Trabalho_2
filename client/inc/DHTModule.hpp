#ifndef __DHTMODULE_HPP__
#define __DHTMODULE_HPP__

#include <iostream>
#include <thread>
using namespace std;

extern "C" {
    #include "dht22.h"
}

#include "Messager.hpp"
#include "cJSON.h"

class DHTModule {
public:
    void operator()(bool & is_end); 
};


#endif // __DHTMODULE_HPP__

