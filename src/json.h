#ifndef JSON_H
#define JSON_H

typedef enum json_type {
    JSON_TYPE_NULL,
    JSON_TYPE_BOOLEAN,
    JSON_TYPE_NUMBER,
    JSON_TYPE_STRING,
    JSON_TYPE_LIST,
    JSON_TYPE_OBJECT
} json_type;

typedef struct json_string {
    const char * string;
    size_t length;
} json_string;

typedef struct json_array {
    json_value * values;
    size_t length;
} json_array;

typedef struct json_object_key_pair {
    json_string key;
    json_value * value;
} json_object_key_pair;

typedef struct json_object_bucket {
    json_object_key_pair * key_pairs;
    size_t count;
} json_object_bucket;

#define JSON_OBJECT_BUCKETS_COUNT 32

typedef struct json_object {
    json_object_bucket buckets[JSON_OBJECT_BUCKETS_COUNT];
    json_value * values;
    size_t count;
} json_object;

typedef struct json_value {
    json_type type;
    union {
        bool boolean;
        json_string string;
        json_array array;
    };
} json_value;

size_t json_value_count(const json_string string);
bool json_parse(const json_string string, json_value * output);

#endif
