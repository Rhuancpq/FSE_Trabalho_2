#ifndef __INTERFACE_HPP__
#define __INTERFACE_HPP__

#include <thread>
#include <iostream>
using namespace std;

#include "Servers.hpp"
#include "Types.hpp"

class Interface{
public:
    void operator()(void);
};

#endif // __INTERFACE_HPP__