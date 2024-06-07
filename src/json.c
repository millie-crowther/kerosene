#include "json.h"

size_t value_count(const char * string, size_t length){
    // TODO: lower bound
    return length;
}

size_t key_pair_count(const char * string, size_t length){
    size_t count = 0;
    for (size_t i = 0; i < length; i++){
        if (string[i] == ":"){
            count++;
        }
    }
    return count;
}
