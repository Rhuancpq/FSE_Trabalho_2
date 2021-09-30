#include "GPIOInModule.hpp"

cJSON * create_message(string type, string tag, int value) {
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
            waitForInterrupt(pin, -1); // wait for interrupt infinitely
            int count = 0;
            if(digitalRead(pin) == HIGH){
            start:
                this_thread::sleep_for(200ms);
                if(digitalRead(pin) == HIGH){
                    this_thread::sleep_for(100ms);
                    if(digitalRead(pin) == LOW){
                        count++;
                    }else {
                        count++;
                        goto start;
                    }
                } else {
                    count++;
                }
                // async send message
                cJSON * message = create_quantity_message(type, tag, count);
                thread t(send_message, ref(message));
                t.detach();
            }
        }

    }
}