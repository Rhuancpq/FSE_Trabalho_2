#ifndef __TYPES_HPP__
#define __TYPES_HPP__

#include <string>
#include <vector>
using namespace std;

typedef struct Data {
    string type, tag;
    bool value;
} Data;

bool operator==(const Data& lhs, const Data& rhs);

typedef struct DistServers {
    string ip, name;
    int port;
    double temperature, humidity;
    vector<Data> in_data, out_data;
} DistServers;

typedef struct Event {
    Data data;
    DistServers origin;
    int priority;
} Event;

#endif