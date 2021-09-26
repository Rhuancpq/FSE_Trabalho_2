#include "GPIOInModule.hpp"

void GPIOInModule::operator()(int pin, const bool & is_end){
    auto callback = [](){};
    wiringPiISR(pin, INT_EDGE_BOTH, callback);
    while(!is_end){
        waitForInterrupt(pin, -1); // wait for interrupt infinitely
        cout << "Interrupted Pin: " << pin << endl;
        cout << "Current value: " << digitalRead(pin) << endl; 
    }
}