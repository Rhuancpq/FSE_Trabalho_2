#include "Control.hpp"

bool Control::is_running = true;
bool Control::is_alarm_on = false;

int people_count = 0;

void Control::stop_all() {
    is_running = false;
}

void Control::update_alarm(bool is_alarm_on) {
    Control::is_alarm_on = is_alarm_on;
}

cJSON * mount_action (string tag, int value) {
    cJSON * request = cJSON_CreateObject();
    cJSON_AddStringToObject(request, "type", "action");
    cJSON * action = cJSON_CreateObject();
    cJSON_AddStringToObject(action, "tag", tag.c_str());
    cJSON_AddNumberToObject(action, "value", value);
    cJSON_AddItemToObject(request, "action", action);
    return request;
}

void Control::handle_event_smoke(Event event){
    // TODO: Handle event
    // Turn on fire sprinkler
    if(event.data.value) {
        Servers * servers = Servers::getInstance();
        vector<DistServers> dist_servers = servers->getServers();
        for(auto & dist_server : dist_servers) {
            auto it = find_if(dist_server.out_data.begin(), dist_server.out_data.end(), [](const Data & data) {
                return data.type == "aspersor";
            });

            if(it != dist_server.out_data.end()) {
                it->value = true;
                cJSON * request = mount_action(it->tag, 1);
                send_message_async(dist_server.ip, dist_server.port, request);
            }
        }
    }
}

void Control::handle_event_presence(Event event){
    if(Control::is_alarm_on) {
        // TODO: Run Alarm sound
    } else if(event.data.type == "presenca") {
        // TODO : Check value and schedule to turn off after 10s 
        Data data;
        for(auto d : event.origin.out_data) {
            if(d.type == "lampada" and d.tag.find("Corredor") != string::npos)
                data = d;
        }

        cJSON * request = mount_action(data.tag, event.data.value);
        send_message_async(event.origin.ip, event.origin.port, request);
    }
}

void Control::handle_event_count(Event event){
    if(event.data.tag.find("Entrando") != std::string::npos) {
        people_count++;
    } else if(event.data.tag.find("Saindo") != std::string::npos) {
        people_count > 0 and people_count--;
    }
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