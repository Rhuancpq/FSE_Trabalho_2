#ifndef __CONTROL_HPP__
#define __CONTROL_HPP__

#include "EventQueue.hpp"
#include "Types.hpp"
#include "Servers.hpp"
#include "Messager.hpp"

#include <thread>
#include <iostream>
using namespace std;

extern int people_count;

class Control {
private:
    static bool is_running;
    static bool is_alarm_on;
    void handle_event_smoke(Event event);
    void handle_event_presence(Event event);
    void handle_event_count(Event event);
    void handle_event_action(Event event);
public:
    Control(){};
    ~Control(){};
    void static stop_all();
    void init_control();
    void static update_alarm(bool is_alarm_on);
    bool static get_alarm_status();
};

#endif // __CONTROL_HPP__
