#ifndef __GPIOINMODULE_HPP__
#define __GPIOINMODULE_HPP__

#include <wiringPi.h>
#include <thread>
#include <iostream>
using namespace std;

class GPIOInModule {
public:
    void operator()(int pin, const bool & is_end);
};

#endif