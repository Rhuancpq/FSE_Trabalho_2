#ifndef __SERVERS_HPP__
#define __SERVERS_HPP__

#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <mutex>
using namespace std;

#include "Types.hpp"
#include "EventQueue.hpp"

// this is a singleton class
class Servers {
public:
    static Servers* getInstance();
    static void destroyInstance();
    void addServer(const DistServers & server);
    void removeServer(const string & name);
    DistServers getServer(const string& name);
    vector<DistServers> getServers();
    bool hasServer(const string& name);
    void updateTemperature(const string & name, 
    const double & temperature, const double & humidity);
    void updateData(const string& name, Data data);
private:
    Servers() = default;
    ~Servers();    
    static Servers* instance;
    mutex servers_mtx;
    unordered_map<string, DistServers> servers;
};


#endif