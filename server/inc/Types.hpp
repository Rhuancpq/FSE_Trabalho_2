#ifndef __TYPES_HPP__
#define __TYPES_HPP__

#include <string>
#include <vector>
using namespace std;

typedef struct Data {
    string type, tag;
    bool value;
} Data;

typedef struct DistServers {
    string ip, name;
    int port;
    vector<Data> in_data, out_data;
} DistServers;

#endif