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
struct json_document_t;

typedef struct json_array_t {
    struct json_value_t ** elements;
    uint32_t length;
} json_array_t;

typedef struct json_key_pair_t {
    const char * key;
    json_value_t * value;
    json_value_t * object;
    struct json_key_pair_t * next;
} json_key_pair_t;

typedef struct json_value_t {
    struct json_document_t * document;
    json_type_t type;
    union {
        bool boolean;
        double number;
        const char * string;
        json_array_t array;
        json_hash_map_t * hash_map;
    };
} json_value_t;

typedef struct json_document_t {
    void * data_pointer;
    json_value_t * root;

    json_value_t ** arrays;

    json_key_pair_t ** buckets;
    uint32_t buckets_length;
    json_key_pair_t * key_pairs;
} json_document_t;

bool json_compile_regular_expressions();
json_value_t * json_object_get(const json_value_t * object, const char * string);
bool json_document_parse(const char * string, json_document_t * document);
void json_document_destroy(json_document_t * document);

#endif
