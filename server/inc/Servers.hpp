#ifndef __SERVERS_HPP__
#define __SERVERS_HPP__

#include <unordered_map>
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
private:
    Servers();
    ~Servers();    
    static Servers* instance;
    unordered_map<string, DistServers> servers;
};


#endif