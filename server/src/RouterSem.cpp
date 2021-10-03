#include "RouterSem.hpp"

sem_t RouterSem::sem;

void RouterSem::init() {
    sem_init(&sem, 0, MAX_SEM_COUNT);
}

void RouterSem::acquire() {
    sem_wait(&sem);
}

void RouterSem::release() {
    sem_post(&sem);
}