#include "json.h"

#include <regex.h>

typedef enum json_token_type_t {
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
} json_token_type_t;

typedef struct json_token_t {
    json_token_type_t type;
    const char * string;
} json_token_t;

typedef struct json_token_regex_t {
    json_token_type_t type;
    const char * regex_string;
    regex_t regex
} json_token_regex_t;

json_token_regex_t regexes[JSON_TOKEN_TYPE_INVALID] = {
    { .type = JSON_TOKEN_TYPE_TRUE, .regex_string = "^true" },
    { .type = JSON_TOKEN_TYPE_INVALID }
};

bool json_compile_regular_expressions(){
    for (uint32_t i = 0; regexes[i].type != JSON_TOKEN_TYPE_INVALID; i++){
        int exit_code = regcomp(&regexes[i].regex, regexes[i].regex_string, REG_EXTENDED);
        if (exit_code != 0){
            return false;
        }
    }

    return true;
}

bool tokenize(const char * string, json_token_t * tokens){
    for (uint32_t i = 0; regexes[i].type != JSON_TOKEN_TYPE_INVALID; i++){
        regmatch_t match;
        int result = regexec(&regexes[i].regex, string, 1, &match, 0);
        if (result == 0){
            if (regexes[i].type != JSON_TOKEN_TYPE_WHITESPACE){
                *tokens = (json_token_t){
                    .type = regexes[i].type,
                    .string = string,
                };
            }
            string = string + match.rm_eo;
            i = 0;
        }
    }
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
