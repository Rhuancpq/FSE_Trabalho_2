#include "GPIOInModule.hpp"

void GPIOInModule::operator()(int pin, const bool & is_end, string  type, string  tag){
    auto callback = [](){};
    if(wiringPiSetupGpio()  == -1){
        cerr << "Error: wiringPiSetupGpio()" << endl;
        return;
    }
    wiringPiISR(pin, INT_EDGE_BOTH, callback);
    while(!is_end){
        waitForInterrupt(pin, -1); // wait for interrupt infinitely
        cout << "Interrupted Pin: " << pin << endl;
        cout << "Current value: " << digitalRead(pin) << endl; 
    }
}