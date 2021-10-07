#ifndef __EVENTQUEUE_HPP__
#define __EVENTQUEUE_HPP__

#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <mutex>
#include <deque>
using namespace std;

#include "Types.hpp"

// this is a singleton class
class EventQueue {
public:
    static EventQueue* getInstance();
    static void destroyInstance();
    void push(Event event);
    Event pop();
    int size();
private:
    EventQueue() = default;
    ~EventQueue() = default;
    static EventQueue* instance;
    mutex queue_mtx;
    deque<Event> queue;
};


#endif // __EVENTQUEUE_HPP__