#ifndef __DHT22_H__
#define __DHT22_H__

// CONSTANTS 
#define MAX_TIMINGS	85
#define DEBUG 0
#define WAIT_TIME 2000

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// GLOBAL VARIABLES
extern uint8_t dht_pin;  // current using gpio pin 20 (wiringPi pin 28)

extern float temp_cels;
extern float temp_fahr;
extern float humidity;

// FUNCTION DECLARATIONS
int init();
int read_dht_data();

#endif // __DHT22_H__