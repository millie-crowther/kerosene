#include "json.h"

uint32_t value_count_upper_bound(const char * string, uint32_t length){
    // TODO: lower bound
    return length;
}

uint32_t key_pair_count_upper_bound(const char * string, uint32_t length){
    uint32_t count = 0;
    for (uint32_t i = 0; i < length; i++){
        if (string[i] == ':'){
            count++;
        }
    }
    return count;
}

uint64_t hash(const char * string, uint32_t length) {
    // see: http://www.cs.yorku.ca/~oz/hash.html
    uint64_t hash = 5381;

    for (uint32_t i = 0; i < length; i++){
        hash = ((hash << 5) + hash) + string[i];
    }

    return hash;
}
