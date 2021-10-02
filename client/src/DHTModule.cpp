#include "DHTModule.hpp"

void DHTModule::operator()(bool & is_end) {
    init();

    int done = 0;

    while (!is_end) {

        while(!done) {
            done = !read_dht_data();
            delay(WAIT_TIME);
        }

        done = 0;

        // TODO: send data to server
        cJSON * json = cJSON_CreateObject();
        // type data 
        cJSON_AddStringToObject(json, "type", "data");
        cJSON * data = cJSON_CreateObject();
        cJSON_AddStringToObject(data, "type", "temperature");
        cJSON_AddStringToObject(data, "tag", "DHT - Sensor");
        cJSON_AddNumberToObject(data, "temperature", temp_cels);
        cJSON_AddNumberToObject(data, "humidity", humidity);
        cJSON_AddItemToObject(json, "data", data);
        if(send_message(json) < 0){
            cerr << "Error sending data to server" << endl;
        }

        this_thread::sleep_for(900ms);

    }
}