#include "json.h"

uint32_t value_count_upper_bound(const string string){
    // TODO: lower bound
    return string.length;
}

uint32_t key_pair_count_upper_bound(const string string){
    uint32_t count = 0;
    for (uint32_t i = 0; i < string.length; i++){
        if (string.chars[i] == ':'){
            count++;
        }
    }
    return count;
}

uint64_t hash(const string string) {
    // see: http://www.cs.yorku.ca/~oz/hash.html
    uint64_t hash = 5381;

    for (uint32_t i = 0; i < string.length; i++){
        hash = ((hash << 5) + hash) + string.chars[i];
    }

    return hash;
}

json_value * json_object_get(const json_object * object, const string string){
    uint64_t index = hash(string.chars, string.length) % JSON_OBJECT_BUCKETS_COUNT;
    json_object_bucket bucket = object->buckets[index];
    for (uint32_t i = 0; i < bucket.length; i++){
        json_object_key_pair key_pair = bucket.key_pairs[i];
        if (string_equals(string, key_pair.key)){
            return key_pair.value;
        }
    }
    return nullptr;
}
