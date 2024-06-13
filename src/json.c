#include "json.h"

#include <regex.h>
#include <stdlib.h>
#include <string.h>

typedef enum json_token_type_t {
    JSON_TOKEN_TYPE_NULL,
    JSON_TOKEN_TYPE_FALSE,
    JSON_TOKEN_TYPE_TRUE,
    JSON_TOKEN_TYPE_NUMBER,
    JSON_TOKEN_TYPE_STRING,
    JSON_TOKEN_TYPE_OPEN_BRACE,
    JSON_TOKEN_TYPE_OPEN_BRACKET,
    JSON_TOKEN_TYPE_COLON,
    JSON_TOKEN_TYPE_COMMA,
    JSON_TOKEN_TYPE_CLOSE_BRACE,
    JSON_TOKEN_TYPE_CLOSE_BRACKET,
    JSON_TOKEN_TYPE_WHITESPACE
} json_token_type_t;

typedef struct json_token_t {
    json_token_type_t type;
    const char * string;
} json_token_t;

typedef struct json_parser_t {
    json_token_t * tokens;
    json_value_t * values;
    json_value_t ** arrays;
    json_key_pair_t ** buckets;
    uint32_t buckets_length;
    json_key_pair_t * key_pairs;
} json_parser_t;

typedef struct json_token_regex_t {
    json_token_type_t type;
    json_type_t json_type;
    const char * regex_string;
    regex_t regex;
} json_token_regex_t;

json_token_regex_t regexes[] = {
    { .type = JSON_TOKEN_TYPE_TRUE, .json_type = JSON_TYPE_BOOLEAN, .regex_string = "^true" }
};

bool json_compile_regular_expressions(){
    for (uint32_t i = 0; i < sizeof(regexes) / sizeof(regexes[0]); i++){
        if (regcomp(&regexes[i].regex, regexes[i].regex_string, REG_EXTENDED) != 0){
            return false;
        }
    }
    return true;
}

int tokenize(const char * string, json_token_t * tokens){
    int token_count = 0;
    while (*string != '\0'){
        for (uint32_t i = 0; i < sizeof(regexes) / sizeof(regexes[0]); i++){   
            regmatch_t match;
            int result = regexec(&regexes[i].regex, string, 1, &match, 0);
            if (result == 0){
                if (regexes[i].type != JSON_TOKEN_TYPE_WHITESPACE){
                    tokens[token_count] = (json_token_t){
                        .type = regexes[i].type,
                        .string = string,
                    };
                    token_count++;
                }
                string += match.rm_eo;
                break;
            }
        }
    }

    return token_count;
}

uint64_t hash(const json_value_t * object, const char * string) {
    // see: http://www.cs.yorku.ca/~oz/hash.html
    uint64_t hash = 5381;

    for (; *string != '\0'; string++){
        hash = ((hash << 5) + hash) + *string;
    }

    return hash ^ (uint64_t)(object);
}

json_value_t * json_object_get(const json_value_t * object, const char * string){
    json_document_t * document = object->document;
    uint64_t index = hash(object, string) % document->buckets_length;
    json_key_pair_t * start = document->buckets[index];
    
    for (json_key_pair_t * key_pair = start; key_pair != nullptr; key_pair = key_pair->next){
        if (key_pair->object == object && strcmp(string, key_pair->key) == 0){
            return key_pair->value;
        }
    }
    
    return nullptr;
}

json_key_pair_t json_object_insert(json_value_t * object, char * key, json_value_t * value){
    json_document_t * document = object->document;
    uint64_t index = hash(object, string) % document->buckets_length;
    json_key_pair_t * next = document->buckets[index];
    
    json_key_pair_t * this = document->key_pairs;
    *this = (json_key_pair_t){
        .key = key,
        .value = value,
        .object = object,
        .next = next
    };

    document->buckets[index] = this;
    document->key_pairs++;
}

bool parse_json_array(json_parser_t * parser, json_array_t * array){
    if (parser->tokens->type != JSON_TOKEN_TYPE_OPEN_BRACKET){
        return false;
    }
    parser->tokens++;
    
    array->elements = parser->arrays;
    for (array->length = 0;; array->length++){
        array->elements[array->length] = parse_json_value(parser);
        if (array->elements[array->length] == nullptr){
            return false;
        } else if (parser->tokens->type != JSON_TOKEN_TYPE_COMMA){
            break;
        }
        parser->tokens++;
    } 

    if (parser->tokens->type != JSON_TOKEN_TYPE_CLOSE_BRACKET){
        return false;
    }
    
    parser->arrays += array->length;
    parser->tokens++;
    return true;
}

json_value_t * parse_json_value(json_parser_t * parser){
    json_value_t * result = parser->values;
    result->type = parser->tokens->json_type;
    parser->values++;

    if (token.type == JSON_TOKEN_TYPE_NULL || token.type == JSON_TOKEN_TYPE_FALSE || token.type == JSON_TOKEN_TYPE_TRUE){
        result->boolean = token.type == JSON_TOKEN_TYPE_TRUE;
        return result;
    } else if (token.type == JSON_TOKEN_TYPE_NUMBER){
        result->number = atof(token.string);
        return result;
    } else if (token.type == JSON_TOKEN_TYPE_STRING){
        // TODO
        return result;
    } else if (parse_json_array(parser, &result->array)){
        return result;
    } 
        
    return nullptr;
}

bool json_document_parse(const char * string, json_document_t * document){
    size_t string_length = strlen(string);
    json_token_t * tokens = malloc(string_length * sizeof(json_token_t));

    if (tokens == nullptr){
        return false;
    }

    int token_count = tokenize(string, tokens);
    if (token_count < 0){
        free(tokens);
        return false;
    }

    uint32_t value_count = token_count;
    uint32_t object_count = token_count / 2;
    void * data_pointer = malloc(
        value_count * sizeof(json_value_t) + object_count * sizeof(json_object_t)
    );

    if (data_pointer == nullptr){
        free(tokens);
        return false;
    }

    json_value_t * values = data_pointer;
    json_object_t * objects = (json_object_t *)(values + value_count);

    json_token_t * token_pointer = tokens;
    json_value_t * root = parse_json_value(&token_pointer, &values);
    free(tokens);

    if (root == nullptr){
        free(data_pointer);
        return false;
    }
    
    *document = (json_document_t){
        .data_pointer = data_pointer,
        .root = root
    };

    return true;
}
