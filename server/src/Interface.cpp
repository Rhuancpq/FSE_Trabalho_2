#include "Interface.hpp"

void Interface::operator()(void) {
    while(true) {
        Servers * s = Servers::getInstance();
        vector<DistServers> all_states = s->getServers();
        for(auto & state : all_states) {
            cout << "Servidor Distribuido: " << state.name << endl;
            cout << "Estados: " << endl;
            for(auto & data : state.in_data) {
                cout << data.tag << " - " << data.value << endl;
            }
            cout << endl;
        }
        this_thread::sleep_for(2s);
    } 
}