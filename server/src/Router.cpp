#include "Router.hpp"

Router::Router(cJSON * request) {
    this->request = request;
}

void Router::init() {
    DistServers dist_server;
    dist_server.ip = cJSON_GetObjectItem(request, "ip")->valuestring;
    dist_server.name = cJSON_GetObjectItem(request, "name")->valuestring;
    dist_server.port = cJSON_GetObjectItem(request, "port")->valueint;
    cJSON * in_data = cJSON_GetObjectItem(request, "inputs");
    cJSON * out_data = cJSON_GetObjectItem(request, "outputs");
    for(int i = 0; i < cJSON_GetArraySize(in_data); i++){
        cJSON * data = cJSON_GetArrayItem(in_data, i);
        Data d;
        d.type = cJSON_GetObjectItem(data, "type")->valuestring;
        d.tag = cJSON_GetObjectItem(data, "tag")->valuestring;
        d.value = 0;
        dist_server.in_data.push_back(d);
    }
    for(int i = 0; i < cJSON_GetArraySize(out_data); i++){
        cJSON * data = cJSON_GetArrayItem(out_data, i);
        Data d;
        d.type = cJSON_GetObjectItem(data, "type")->valuestring;
        d.tag = cJSON_GetObjectItem(data, "tag")->valuestring;
        d.value = 0;
        dist_server.out_data.push_back(d);
    }
    cout << "O servidor distribuído " << dist_server.name 
    << " foi inserido." << endl;
    Servers * server = Servers::getInstance();
    if(server->hasServer(dist_server.name)){
        cout << "O servidor distribuído com esse nome " << dist_server.name 
        << " já existe." << endl;
    } else {
        server->addServer(dist_server);
    }
}

void Router::data() {
    string server_name = cJSON_GetObjectItem(request, "server_name")->valuestring;
    cJSON * data = cJSON_GetObjectItem(request, "data");
    Servers * server = Servers::getInstance();
    Data req_data;
    req_data.type = cJSON_GetObjectItem(data, "type")->valuestring;
    req_data.tag = cJSON_GetObjectItem(data, "tag")->valuestring;
    
    if (req_data.type == "contagem") {
        req_data.value = cJSON_GetObjectItem(data, "value")->valueint;
    } else if (req_data.type == "temperature") {
        double temperature = cJSON_GetObjectItem(data, "temperature")->valuedouble;
        double humidity = cJSON_GetObjectItem(data, "humidity")->valuedouble;
        server->updateTemperature(server_name, temperature, humidity);
        return;
    } else {
        cJSON * state = cJSON_GetObjectItem(data, "state");
        req_data.value = cJSON_IsTrue(state) ? 1 : 0;
    }
    server->updateData(server_name, req_data);
}

void Router::leave() {
    string name = cJSON_GetObjectItem(request, "name")->valuestring;
    Servers * server = Servers::getInstance();
    server->removeServer(name);
    cout << "O servidor distribuído " <<  name << " foi removido." << endl;
}

void Router::filterRequest() {
    string type = cJSON_GetObjectItem(request, "type")->valuestring;
    if (VALID_TYPES.find(type) == VALID_TYPES.end()) {
        cerr << "Invalid request type: " << type << endl;
    }
    cout << "Request type: " << type << endl;
    if (type == "init") {
        init();
    } else if (type == "data") {
        data();
    } else if (type == "leave") {
        leave();
    }
}
