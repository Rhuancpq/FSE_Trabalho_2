#ifndef __GPIO_HPP__
#define __GPIO_HPP__

#include <string>
using namespace std;

typedef struct GPIO_Pin {
    int pin; // JSON gpio item
    string type = ""; // JSON type item
    string tag = ""; // JSON tag item
} GPIO_Pin;

#endif // __GPIO_HPP__