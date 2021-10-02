#include "Servers.hpp"

// this is a singleton class

Servers * Servers::instance; 

Servers::Servers(){
}

Servers::~Servers(){
}

Servers* Servers::getInstance(){
    if(Servers::instance == nullptr){
        Servers::instance = new Servers();
    }
    return Servers::instance;
}


void Servers::destroyInstance(){
    if(Servers::instance != nullptr){
        delete Servers::instance;
        Servers::instance = nullptr;
    }
}

void Servers::addServer(const DistServers & server){
    this->servers[server.name] = server;
}

void Servers::removeServer(const string & name){
    this->servers.erase(name);
}

DistServers Servers::getServer(const std::string & name){
    return this->servers[name];
}