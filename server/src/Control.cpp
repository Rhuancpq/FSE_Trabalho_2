#include "Control.hpp"

bool Control::is_running = true;
bool Control::is_alarm_on = false;

int people_count = 0;

void Control::stop_all() {
    is_running = false;
}

void Control::update_alarm(bool new_alarm_value) {
    Control::is_alarm_on = new_alarm_value;
}

bool Control::get_alarm_status() {
    return is_alarm_on;
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
            for(auto & out_data : dist_server.out_data) {
                if(out_data.type == "aspersor") {
                    cJSON * request = mount_action(out_data.tag, 1);
                    servers->updateData(dist_server.name, {out_data.type, out_data.tag, true});
                    send_message_async(dist_server.ip, dist_server.port, request);
                }
            }

        }
    }
}

// receives type as "action" 
void Control::handle_event_action(Event event) {
    cJSON * request = mount_action(event.data.tag, event.data.value);
    Servers * servers = Servers::getInstance();
    for(auto & data : event.origin.out_data) {
        if(data.tag == event.data.tag) {
            servers->updateData(event.origin.name, {data.type, data.tag, event.data.value});
        }
    }
    send_message_async(event.origin.ip, event.origin.port, request);
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
        Servers * servers = Servers::getInstance();
        servers->updateData(event.origin.name, {data.type, data.tag, event.data.value});
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
            if(event.data.type == "fumaca"){
                this->handle_event_smoke(event);
            }else if(event.data.type == "porta" or
            event.data.type == "janela" or
            event.data.type == "presenca"){
                this->handle_event_presence(event);
            }else if(event.data.type == "contagem") {
                this->handle_event_count(event);
            }else if(event.data.type == "action") {
                this->handle_event_action(event);
            }
        }

        this_thread::sleep_for(10ms);
    }
}