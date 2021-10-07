#include "Servers.hpp"

Servers * Servers::instance; 

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
    lock_guard<mutex> lock(this->servers_mtx);
    this->servers[server.name] = server;
}

void Servers::removeServer(const string & name){
    lock_guard<mutex> lock(this->servers_mtx);
    this->servers.erase(name);
}

DistServers Servers::getServer(const std::string & name){
    return this->servers[name];
}

vector<DistServers> Servers::getServers() {
    vector<DistServers> res;
    if(!this->servers.empty()) {
        lock_guard<mutex> lock(this->servers_mtx);

        for(auto & server : this->servers){
            res.push_back(server.second);
        }
    }
    return res;
}

void Servers::updateData(const string & name, Data data){
    lock_guard<mutex> lock(this->servers_mtx);
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

    // TODO Handle priority
    Event e = {data, this->servers[name], 0};
    EventQueue * eq = EventQueue::getInstance();
    eq->push(e);
}

void Servers::updateTemperature(const string & name, 
const double & temperature, const double & humidity) {
    lock_guard<mutex> lock(this->servers_mtx);
    this->servers[name].temperature = temperature;
    this->servers[name].humidity = humidity;
}

bool Servers::hasServer(const string& name) {
    lock_guard<mutex> lock(this->servers_mtx);
    return this->servers.find(name) != this->servers.end();
}