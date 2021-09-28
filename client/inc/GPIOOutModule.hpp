#ifndef __GPIOOutModule_hpp__
#define __GPIOOutModule_hpp__

#include <wiringPi.h>
#include <thread>
#include <iostream>
using namespace std;

class GPIOOutModule {
public:
    void operator()(int pin, const bool & is_end, const int & out_value);
};

#endif // __GPIOOutModule_hpp__