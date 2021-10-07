#ifndef __ROUTER_HPP__
#define __ROUTER_HPP__

#include <string>
#include <unordered_set>
#include <iostream>
using namespace std;

#include "cJSON.h"
#include "Types.hpp"
#include "Servers.hpp"
#include "RouterSem.hpp"

static unordered_set<string> VALID_TYPES = {"init", "leave", "data"};

class Router {
public:
    Router(cJSON * request);
    ~Router() = default;
    void filterRequest();
private:
    cJSON * request;
    void init();
    void leave();
    void data();
};

#endif // __ROUTER_HPP__