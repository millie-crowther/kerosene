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

bool string_equals(const json_string a, const json_string b){
    return a.length == b.length && strncmp(a.string, b.string, a.length) == 0;
}

json_value * json_object_get(const json_object * object, const json_string string){
    uint64_t index = hash(string.string, string.length) % JSON_OBJECT_BUCKETS_COUNT;
    json_object_bucket bucket = object->buckets[index];
    for (uint32_t i = 0; i < bucket.length; i++){
        json_object_key_pair key_pair = bucket.key_pairs[i];
        if (string_equals(&string, key_pair.key)){
            return key_pair.value;
        }
    }
    return nullptr;
}
