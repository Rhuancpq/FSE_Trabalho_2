#include "Control.hpp"

bool Control::is_running = true;
bool Control::is_alarm_on = false;

void Control::stop_all() {
    is_running = false;
}

void Control::update_alarm(bool is_alarm_on) {
    Control::is_alarm_on = is_alarm_on;
}

void Control::handle_event_smoke(Event event){
    // TODO: Handle event
    // Turn on fire sprinkler
}

void Control::handle_event_presence(Event event){
    if(Control::is_alarm_on) {
        // TODO: Run Alarm sound
    } else if(event.data.type == "presenca") {
        // TODO: TURN ON LIGHT and schedule to turn off after 10s
        
    }
}

void Control::handle_event_count(Event event){
    // TODO: Handle event
}


void Control::init_control() {
    while(is_running) {
        EventQueue * event_queue = EventQueue::getInstance();
        while(event_queue->size() > 0) {
            Event event = event_queue->pop();
            cout << "Event: " << event.data.type << endl;
            if(event.data.type == "fumaca"){
                this->handle_event_smoke(event);
            }else if(event.data.type == "porta" or
            event.data.type == "janela" or
            event.data.type == "presenca"){
                this->handle_event_presence(event);
            }else if(event.data.type == "contagem") {
                this->handle_event_count(event);
            }
        }

        this_thread::sleep_for(10ms);
    }
}