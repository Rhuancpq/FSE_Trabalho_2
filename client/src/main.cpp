#include "cJSON.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <streambuf>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <signal.h>
#include <unordered_map>
#include <thread>
using namespace std;

#include "DHTModule.hpp"
#include "GPIOInModule.hpp"
#include "GPIOOutModule.hpp"
#include "Messager.hpp"

int sockfd;
bool is_end = false;

typedef struct GPIO_Pin {
    int pin; // JSON gpio item
    string type = ""; // JSON type item
    string tag = ""; // JSON tag item
} GPIO_Pin;

vector<GPIO_Pin> get_gpio_info(cJSON * json, string item) {
    vector<GPIO_Pin> gpio_pins;
    cJSON * gpio_array = cJSON_GetObjectItem(json, item.c_str());
    if (cJSON_IsArray(gpio_array)) {
        int gpio_array_size = cJSON_GetArraySize(gpio_array);
        for (int i = 0; i < gpio_array_size; i++) {
            cJSON * gpio_item = cJSON_GetArrayItem(gpio_array, i);
            if (cJSON_IsObject(gpio_item)) {
                GPIO_Pin gpio_pin;
                gpio_pin.pin = cJSON_GetObjectItem(gpio_item, "gpio")->valueint;
                gpio_pin.type = cJSON_GetObjectItem(gpio_item, "type")->valuestring;
                gpio_pin.tag = cJSON_GetObjectItem(gpio_item, "tag")->valuestring;
                gpio_pins.push_back(gpio_pin);
            }
        }
    }
    return gpio_pins;
}

void terminate(int sig) {
    if (sig == SIGINT || sig == SIGTERM || sig == SIGQUIT) {
        is_end = true;
    }
    cout << "Encerrando programa..." << endl;
    sleep(1);
    cout << "Programa encerrado!" << endl;
    exit(0);
}

int main(int argc, char *argv[]){
    signal(SIGINT, terminate);
    signal(SIGTERM, terminate);
    signal(SIGQUIT, terminate);
    string config_file = argv[1];
    ifstream t(config_file);
    // if file doesn't exist, exit
    if (!t.good()) {
        cout << "File not found" << endl;
        return 1;
    }
    string config_str((istreambuf_iterator<char>(t)),
                istreambuf_iterator<char>());
    t.close();
    
    cJSON *config = cJSON_Parse(config_str.c_str());
    string ip = cJSON_GetObjectItem(config, "ip")->valuestring;
    int port = cJSON_GetObjectItem(config, "porta")->valueint;
    string name = cJSON_GetObjectItem(config, "nome")->valuestring;
    vector<GPIO_Pin> gpio_inputs = get_gpio_info(config, "inputs");
    vector<GPIO_Pin> gpio_outputs = get_gpio_info(config, "outputs");

    unordered_map<string, int> gpio_out_values;

    for (GPIO_Pin gpio_pin : gpio_outputs) {
        gpio_out_values[gpio_pin.tag] = 0;
    }
    
    cJSON_Delete(config);

    // // handle SIGINT
    // signal(SIGINT, [](int sig) {
    //     close(sockfd);
    //     exit(0);
    // });
    // struct sockaddr_in serv_addr;
    // if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    //     cout << "Error opening socket" << endl;
    //     return 1;
    // }
    // memset((char *) &serv_addr, 0, sizeof(serv_addr));
    // set_server_addr(&serv_addr, ip.c_str(), to_string(port).c_str());
    // cout << "Iniciando servidor distribuído no Ip: "
    // << inet_ntoa(serv_addr.sin_addr)
    // << " na porta: " << port << endl;
    // if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    //     cout << "Error on binding" << endl;
    //     return 1;
    // }

    // listen(sockfd, 5);

    DHTModule dht;

    thread dht_thread(dht, ref(is_end));
    
    dht_thread.detach();

    for (GPIO_Pin gpio_pin : gpio_inputs) {
        GPIOInModule gpio_in;
        thread gpio_in_thread(gpio_in, gpio_pin.pin, ref(is_end),
        gpio_pin.type, gpio_pin.tag);
        gpio_in_thread.detach();
    }

    for (GPIO_Pin gpio_pin : gpio_outputs) {
        GPIOOutModule gpio_out;
        thread gpio_out_thread(gpio_out, gpio_pin.pin, ref(is_end),
        ref(gpio_out_values[gpio_pin.tag]));
        gpio_out_thread.detach();
    }

    sleep(30);

    // close(sockfd);

    return 0;
}