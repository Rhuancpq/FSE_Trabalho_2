#include "GPIOOutModule.hpp"

/**
* @param pin: GPIO pin number
*/
void GPIOOutModule::operator()(int pin, const bool & is_end, const int & out_value){
    if(wiringPiSetupGpio() == -1){
        cerr << "Error: wiringPiSetupGpio()" << endl;
        return;
    }

    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    int prev_value = out_value;
    while(!is_end){
        if(prev_value != out_value){
            digitalWrite(pin, out_value ? HIGH : LOW);
            prev_value = out_value;
        }

        this_thread::sleep_for(10ms);
    }
    digitalWrite(pin, LOW);
}