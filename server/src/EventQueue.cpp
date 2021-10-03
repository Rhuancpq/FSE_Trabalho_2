#include "EventQueue.hpp"

// this is a singleton class

EventQueue * EventQueue::instance; 

EventQueue::EventQueue(){
}

EventQueue::~EventQueue(){
    this->queue_mtx.~mutex();
}

EventQueue* EventQueue::getInstance(){
    if(EventQueue::instance == nullptr){
        EventQueue::instance = new EventQueue();
    }
    return EventQueue::instance;
}


void EventQueue::destroyInstance(){
    if(EventQueue::instance != nullptr){
        delete EventQueue::instance;
        EventQueue::instance = nullptr;
    }
}

void EventQueue::push(Event event){
    lock_guard<mutex> lock(this->queue_mtx);
    // TODO handle priority
    this->queue.push_back(event);
}

Event EventQueue::pop(){
    Event event;
    lock_guard<mutex> lock(this->queue_mtx);
    event = this->queue[0];
    this->queue.pop_front();
    return event;
}

int EventQueue::size(){
    lock_guard<mutex> lock(this->queue_mtx);
    return this->queue.size();
}
