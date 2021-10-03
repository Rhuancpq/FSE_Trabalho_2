#ifndef __ROUTER_SEM_HPP__
#define __ROUTER_SEM_HPP__

#include <semaphore.h>
#include <iostream>
using namespace std;

#define MAX_SEM_COUNT 5

class RouterSem {
private:
    static sem_t sem;
public:
    static void init();
    static void acquire();
    static void release();
};

#endif  