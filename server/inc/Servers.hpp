#ifndef __SERVERS_HPP__
#define __SERVERS_HPP__

#include <unordered_map>
#include <algorithm>
using namespace std;

#include "Types.hpp"

// this is a singleton class
class Servers {
public:
    static Servers* getInstance();
    static void destroyInstance();
    void addServer(const DistServers & server);
    void removeServer(const string & name);
    DistServers getServer(const string& name);
    vector<DistServers> getServers();
    void updateTemperature(const string & name, 
    const double & temperature, const double & humidity);
    void updateData(const string& name, Data data);
private:
    Servers();
    ~Servers();    
    static Servers* instance;
    // not thread safe, TODO use mutex
    unordered_map<string, DistServers> servers;
};


#endif