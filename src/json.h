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

typedef struct json_object {

} json_object;

typedef struct json_value {
    json_type type;
    union value {
        bool boolean;
        json_string string;
        json_array array;
    };
} json_value;

#endif
