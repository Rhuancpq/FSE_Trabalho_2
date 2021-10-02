#include "Types.hpp"

bool operator==(const Data& lhs, const Data& rhs){
    return lhs.tag == rhs.tag and lhs.type == rhs.type and lhs.value == rhs.value;
}