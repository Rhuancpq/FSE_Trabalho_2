#include "DHTModule.hpp"

void DHTModule::operator()(bool & is_end) {
    init();

    int done = 0;

    while (!is_end) {

        while(!done) {
            done = !read_dht_data();
            delay(WAIT_TIME);
        }

        cout << "Temperatura: " << temp_cels << " C" << endl;
        cout << "Umidade: " << humidity << "%" << endl;
        this_thread::sleep_for(chrono::seconds(1s));

    }
}