#ifndef __GPIOINMODULE_HPP__
#define __GPIOINMODULE_HPP__

#define IGNORE_CHANGE_USEC 10000
#define IGNORE_CHANGE_COUNT 200 // in milliseconds
#define IGNORE_CHANGE_ERROR 100 // in milliseconds

#include <wiringPi.h>
#include <thread>
#include <sys/time.h>
#include <iostream>
using namespace std;

#include "cJSON.h"
#include "Messager.hpp"

class GPIOInModule {
public:
    void operator()(int pin, const bool & is_end, string type, string tag);
};

#endif