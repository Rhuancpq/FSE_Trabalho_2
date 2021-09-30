#include "GPIOInModule.hpp"

cJSON * create_message(string type, string tag, int value) {
    // DATA SCHEMA
    // {
    //    "type": "GPIO_IN",
    //     "data": {
    //         "tag": "GPIO_IN_1",
    //         "value": true
    //     }
    // }
    cJSON * message = cJSON_CreateObject();
    cJSON_AddStringToObject(message, "type", "data");
    cJSON * data = cJSON_CreateObject();
    cJSON_AddStringToObject(data, "type", type.c_str());
    cJSON_AddStringToObject(data, "tag", tag.c_str());
    if (value == 0) {
        cJSON_AddFalseToObject(data, "state");
    } else {
        cJSON_AddTrueToObject(data, "state");
    }
    cJSON_AddItemToObject(message, "data", data);
    return message;
}

cJSON * create_quantity_message(string type, string tag, int value) {
    // DATA SCHEMA
    // {
    //     "type": "data",
    //     "data": {
    //         "type": "contagem",
    //         "tag": "sensor entrada",
    //         "value": 40
    //     }
    // }   
    cJSON * message = cJSON_CreateObject();
    cJSON_AddStringToObject(message, "type", "data");
    cJSON * data = cJSON_CreateObject();
    cJSON_AddStringToObject(data, "type", type.c_str());
    cJSON_AddStringToObject(data, "tag", tag.c_str());
    cJSON_AddNumberToObject(data, "value", value);
    cJSON_AddItemToObject(message, "data", data);
    return message;
}

void GPIOInModule::operator()(int pin, const bool & is_end, string  type, string  tag){
    auto callback = [](){};
    struct timeval now, last_change;
	unsigned long diff;

    if(wiringPiSetupGpio()  == -1){
        cerr << "Error: wiringPiSetupGpio()" << endl;
        return;
    }
    wiringPiISR(pin, INT_EDGE_BOTH, callback);
    if(type != "contagem"){
        while(!is_end){
            waitForInterrupt(pin, -1); // wait for interrupt infinitely
            gettimeofday(&now, NULL);
            diff = (now.tv_sec - last_change.tv_sec) * 1000000 + (now.tv_usec - last_change.tv_usec);
            if (diff > IGNORE_CHANGE_USEC) {
                // async send message
                cJSON * message = create_message(type, tag, digitalRead(pin));
                thread t(send_message, ref(message));
                t.detach();
            }
            last_change = now;
        }
    }else {
        while(!is_end){
            // TODO for the sake of god, this is not the best way to do this
            waitForInterrupt(pin, -1);
            bool is_counting = false;
            if(digitalRead(pin) == HIGH && !is_counting){
                gettimeofday(&now, NULL);
                last_change = now;
                is_counting = true;
            } else if (digitalRead(pin) == LOW && is_counting){ 
                gettimeofday(&now, NULL);
                diff = (now.tv_sec - last_change.tv_sec) * 1000000 + (now.tv_usec - last_change.tv_usec);
                // convert diff to milliseconds
                diff = diff / 1000;
                if (diff > (IGNORE_CHANGE_COUNT - IGNORE_CHANGE_ERROR) && 
                diff < IGNORE_CHANGE_COUNT + IGNORE_CHANGE_ERROR ) {
                    // async send message
                    cJSON * message = create_quantity_message(type, tag, 1);
                    thread t(send_message, ref(message));
                    t.detach();
                    is_counting = false;        
                }
            }
        }

    }
}