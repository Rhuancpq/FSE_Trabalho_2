#include "Servers.hpp"

// this is a singleton class

Servers * Servers::instance; 

Servers::Servers(){
}

Servers::~Servers(){
    this->servers_mtx.~mutex();
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
    this->servers_mtx.lock();
    this->servers[server.name] = server;
    this->servers_mtx.unlock();
}

void Servers::removeServer(const string & name){
    this->servers_mtx.lock();
    this->servers.erase(name);
    this->servers_mtx.unlock();
}

DistServers Servers::getServer(const std::string & name){
    return this->servers[name];
}

vector<DistServers> Servers::getServers() {
    vector<DistServers> servers;
    for(auto it = this->servers.begin(); it != this->servers.end(); it++){
        servers.push_back(it->second);
    }
    return servers;
}

void Servers::updateData(const string & name, Data data){
    this->servers_mtx.lock();
    auto it_in = find_if(this->servers[name].in_data.begin(), 
    this->servers[name].in_data.end(),
    [&](const Data &d) { return d.tag == data.tag and d.type == data.type; });
    auto it_out = find_if(this->servers[name].out_data.begin(), 
    this->servers[name].out_data.end(),
    [&](const Data &d) { return d.tag == data.tag and d.type == data.type; });

    if(it_in != this->servers[name].in_data.end()){
        it_in->value = data.value;
    }
    if(it_out != this->servers[name].out_data.end()){
        it_out->value = data.value;
    }
    this->servers_mtx.unlock();
}

void Servers::updateTemperature(const string & name, 
const double & temperature, const double & humidity) {
    this->servers_mtx.lock();
    this->servers[name].temperature = temperature;
    this->servers[name].humidity = humidity;
    this->servers_mtx.unlock();
}