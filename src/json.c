#include "json.h"

#include <regex.h>

typedef enum json_token_type {
    JSON_TOKEN_TYPE_NULL,
    JSON_TOKEN_TYPE_FALSE,
    JSON_TOKEN_TYPE_TRUE,
    JSON_TOKEN_TYPE_NUMBER,
    JSON_TOKEN_TYPE_STRING,
    JSON_TOKEN_TYPE_OPEN_BRACE,
    JSON_TOKEN_TYPE_CLOSE_BRACE,
    JSON_TOKEN_TYPE_OPEN_BRACKET,
    JSON_TOKEN_TYPE_CLOSE_BRACKET,
    JSON_TOKEN_TYPE_WHITESPACE,
    JSON_TOKEN_TYPE_INVALID
} json_token_type;

typedef struct json_token {
    json_token_type type;
    const string string;
} json_token;

typedef struct json_token_regex {
    json_token_type type;
    const char * regex_string;
    regex_t regex
} json_token_regex;

json_token_regex regexes[JSON_TOKEN_TYPE_INVALID] = {
    { .type = JSON_TOKEN_TYPE_TRUE, .regex_string = "^true" },
    { .type = JSON_TOKEN_TYPE_INVALID }
};

bool json_compile_regular_expressions(){
    for (uint32_t i = 0; regexes[i].type != JSON_TOKEN_TYPE_INVALID; i++){
        int exit_code = regcomp(&regexes[i].regex, regexes[i].regex_string, 0);
        if (exit_code != 0){
            return false;
        }
    }

    return true;
}

bool tokenize(string string, json_token * tokens){

}

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
    uint64_t index = hash(string) % JSON_OBJECT_BUCKETS_COUNT;
    json_object_bucket bucket = object->buckets[index];
    for (uint32_t i = 0; i < bucket.length; i++){
        json_object_key_pair key_pair = bucket.key_pairs[i];
        if (string_equals(string, key_pair.key)){
            return key_pair.value;
        }
    }
    return nullptr;
}
