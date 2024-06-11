#ifndef JSON_H
#define JSON_H

#include <stdint.h>

typedef enum json_type_t {
    JSON_TYPE_NULL,
    JSON_TYPE_BOOLEAN,
    JSON_TYPE_NUMBER,
    JSON_TYPE_STRING,
    JSON_TYPE_ARRAY,
    JSON_TYPE_OBJECT
} json_type_t;

struct json_value_t;

typedef struct json_array_t {
    struct json_value_t * elements;
    uint32_t length;
} json_array_t;

typedef struct json_bucket_t {
    const json_value_t * keys;
    const json_value_t * values;
    uint32_t length;
} json_bucket_t;

#define JSON_OBJECT_BUCKETS_COUNT 32

typedef struct json_object_t {
    json_bucket_t buckets[JSON_OBJECT_BUCKETS_COUNT];
} json_object_t;

typedef struct json_value_t {
    json_type_t type;
    union {
        bool boolean;
        double number;
        const char * string;
        json_array_t array;
        json_object_t * object;
    };
} json_value_t;

typedef struct json_document_t {
    void * data_pointer;
    json_value_t * root;
} json_document_t;

bool json_compile_regular_expressions();
json_value_t * json_object_get(const json_object_t * object, const char * string);
bool json_document_parse(const char * string, json_document_t * document);
void json_document_destroy(json_document_t * document);

#endif
