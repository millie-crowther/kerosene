#ifndef JSON_H
#define JSON_H

#include "string.h"

typedef enum json_type {
    JSON_TYPE_NULL,
    JSON_TYPE_BOOLEAN,
    JSON_TYPE_NUMBER,
    JSON_TYPE_STRING,
    JSON_TYPE_LIST,
    JSON_TYPE_OBJECT
} json_type;

typedef struct json_array {
    json_value * values;
    uint32_t length;
} json_array;

typedef struct json_object_key_pair {
    string key;
    json_value * value;
} json_object_key_pair;

typedef struct json_object_bucket {
    json_object_key_pair * key_pairs;
    uint32_t length;
} json_object_bucket;

#define JSON_OBJECT_BUCKETS_COUNT 32

typedef struct json_object {
    json_object_bucket * buckets;
} json_object;

typedef struct json_value {
    json_type type;
    union {
        bool boolean;
        double number;
        string string;
        json_array array;
        json_object object;
    };
} json_value;

typedef struct json_document {
    json_value * values;
    json_object_key_pair * key_pairs;
    const char * string;
    json_object_bucket * bucket;

    void * data_pointer;
} json_document;

json_value * json_object_get(const json_object * object, const string string);
bool json_document_parse(const string string, json_document * document);
void json_document_destroy(json_document * document);

#endif
