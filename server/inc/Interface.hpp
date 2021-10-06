#ifndef __INTERFACE_HPP__
#define __INTERFACE_HPP__

#include <curses.h>
#include <menu.h>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <set>
#include <thread>
#include <chrono>
using namespace std;

#include "Servers.hpp"
#include "Types.hpp"
#include "EventQueue.hpp"

class Interface{
public:
    void operator()(bool & is_running);
};

#endif // __INTERFACE_HPP__