#include "cJSON.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <streambuf>
#include <signal.h>
#include <unordered_map>
#include <thread>
using namespace std;

#include "DHTModule.hpp"
#include "GPIOInModule.hpp"
#include "GPIOOutModule.hpp"
#include "Messager.hpp"
#include "Listener.hpp"
#include "GPIO.hpp"

int sockfd;
bool is_end = false;

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
    int server_port = cJSON_GetObjectItem(config, "porta_servidor")->valueint;
    string name = cJSON_GetObjectItem(config, "nome")->valuestring;
    vector<GPIO_Pin> gpio_inputs = get_gpio_info(config, "inputs");
    vector<GPIO_Pin> gpio_outputs = get_gpio_info(config, "outputs");

    unordered_map<string, int> gpio_out_values;

    for (GPIO_Pin gpio_pin : gpio_outputs) {
        gpio_out_values[gpio_pin.tag] = 0;
    }
    
    cJSON_Delete(config);

    server_addr = ip;
    ::server_port = server_port;

    Listener listener;

    thread listener_thread(listener, cref(is_end), name, port,
    ref(gpio_out_values), cref(gpio_inputs), cref(gpio_outputs));

    listener_thread.detach();

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

    return 0;
}